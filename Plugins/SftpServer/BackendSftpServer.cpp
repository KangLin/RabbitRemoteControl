// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QDir>
#include <QFile>

// Windows 平台需要包含不同的头文件
#if defined(Q_OS_WIN)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif
#include "BackendSftpServer.h"

#define DEF_STR_SIZE 1024
char authorizedkeys[DEF_STR_SIZE] = {0};
static Q_LOGGING_CATEGORY(log, "Backend.SftpServer")

// 获取客户端 IP 地址的通用函数
static QString GetClientAddress(ssh_session session)
{
    int sock_fd = ssh_get_fd(session);
    if (sock_fd < 0) {
        return QString();
    }

    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    if (getpeername(sock_fd, (struct sockaddr*)&addr, &addr_len) == 0) {
        char ip_str[INET6_ADDRSTRLEN];

        if (addr.ss_family == AF_INET) {
            // IPv4
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)&addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, sizeof(ip_str));
            return QString::fromUtf8(ip_str);
        } else if (addr.ss_family == AF_INET6) {
            // IPv6
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)&addr;
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ip_str, sizeof(ip_str));
            return QString::fromUtf8(ip_str);
        }
    }

    return QString();
}

// 同时获取 IP 和端口的函数
static bool GetClientAddressAndPort(ssh_session session, QString& ip, quint16& port)
{
    int sock_fd = ssh_get_fd(session);
    if (sock_fd < 0) {
        return false;
    }

    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    if (getpeername(sock_fd, (struct sockaddr*)&addr, &addr_len) == 0) {
        char ip_str[INET6_ADDRSTRLEN];

        if (addr.ss_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)&addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, sizeof(ip_str));
            ip = QString::fromUtf8(ip_str);
            port = ntohs(ipv4->sin_port);
            return true;
        } else if (addr.ss_family == AF_INET6) {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)&addr;
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ip_str, sizeof(ip_str));
            ip = QString::fromUtf8(ip_str);
            port = ntohs(ipv6->sin6_port);
            return true;
        }
    }

    return false;
}

CBackendSftpServer::CBackendSftpServer(COperateSftpServer *pOperate, bool bStopSignal)
    : CBackend(pOperate, bStopSignal)
    , m_pPara(pOperate->GetParameter())
    , m_event(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackendSftpServer::~CBackendSftpServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackend::OnInitReturnValue CBackendSftpServer::OnInit()
{
    OnInitReturnValue ret = OnInitReturnValue::Fail;
    if(!m_pPara) return ret;

    m_event = ssh_event_new();
    if(!m_event)
        return ret;

    QDir root(m_pPara->GetRoot());
    if (!root.exists() && !root.mkpath(".")) {
        qCritical(log) << "Failed to create root directory";
        return ret;
    }

    qint16 port = m_pPara->m_Net.GetPort();
    if(m_pPara->GetListenAll()) {
        Listen("0.0.0.0", port);
        qInfo(log) << "SFTP Server is listend on port:" << port
                   << "; Root path:" << root.absolutePath();
    } else if(!m_pPara->GetListen().isEmpty()){
        foreach (auto add, m_pPara->GetListen()) {
            Listen(add, port);
        }
        qInfo(log) << "SFTP Server is listend on port:" << port << m_pPara->GetListen()
                   << "; Root path:" << root.absolutePath();
    } else
        return ret;

    emit sigRunning();
    return OnInitReturnValue::UseOnProcess;
}

int CBackendSftpServer::OnClean()
{
    int nRet = 0;

    foreach(auto d, m_lstListen) {
        ssh_bind_free(d->sshBind);
        delete d->pListenNotifier;
        delete d;
    }
    m_lstListen.clear();

    if(m_event)
        ssh_event_free(m_event);

    foreach (auto c, m_lstClients) {
        RemoveClient(c);
    }
    m_lstClients.clear();

    emit sigFinished();
    return nRet;
}

int CBackendSftpServer::WakeUp()
{
    int nRet = 0;
    return nRet;
}

/*!
 * \~chinese 具体操作处理
 * \return
 *       \li >= 0: 继续。再次调用间隔时间，单位毫秒
 *       \li = -1: 停止
 *       \li < -1: 错误
 *
 * \~english Specific operation processing of plug-in
 * \return
 *       \li >= 0: continue, Interval call time (msec)
 *       \li = -1: stop
 *       \li < -1: error
 * \~
 * \see Start() slotTimeOut()
 */
int CBackendSftpServer::OnProcess()
{
    int nRet = 0;

    if(m_lstClients.isEmpty())
        return 10;

    nRet = ssh_event_dopoll(m_event, 100);
    if (SSH_ERROR == nRet) {
        qDebug(log) << "Failed: ssh_event_dopoll. nRet:" << nRet;
        return -1;
    }

    QList<sClientData*> toRemove;
    foreach(auto pData, m_lstClients) {
        if(!pData) {
            toRemove.push_back(pData);
            continue;
        }

        if(nullptr == pData->channel) {
            // Authenticate
            if(pData->pPara->GetAuthenticateAttempts() < pData->m_nAuthAttempts
                || pData->pPara->GetAuthenticateTime()
                       < pData->m_Time.msecsTo(QTime::currentTime())) {
                toRemove.push_back(pData);
                continue;
            }
        } else if(pData->m_Time.isValid()) {
            ssh_set_channel_callbacks(pData->channel, &pData->channel_cb);
            pData->m_Time = QTime();
        }

        if(pData->channel && !ssh_channel_is_open(pData->channel)) {
            qWarning(log) << "Then channel is not open";
        }
    }

    foreach(auto pData, toRemove) {
        m_lstClients.removeAll(pData);
        RemoveClient(pData);
    }

    return 0;
}

bool CBackendSftpServer::InitHostKey(ssh_bind ssdBind)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!ssdBind) return false;

    // 检查主机密钥文件
    QString szPath;
    szPath = m_pPara->GetHostKeyFile();
    if (!QFile::exists(szPath)) {
        qCritical(log) << "Host key not found:" << szPath;
        return false;
    }

    // 尝试加载 RSA 密钥
    if (ssh_bind_options_set(ssdBind, SSH_BIND_OPTIONS_HOSTKEY,
                             szPath.toStdString().c_str()) < 0) {
        qCritical(log) << "Failed to set RSA host key" << ssh_get_error(ssdBind);
        return false;
    }

    return true;
}

int CBackendSftpServer::Listen(const QString &szIp, qint16 nPort)
{
    int ret = -1;
    sBindData* pData = new sBindData();
    if(!pData) return ret;
    do {
        auto sshBind = ssh_bind_new();
        if (!sshBind) {
            qCritical(log) << "Failed to create SSH bind";
            return ret;
        }

        auto &net = m_pPara->m_Net;
        quint16 port = net.GetPort();
        ssh_bind_options_set(sshBind, SSH_BIND_OPTIONS_BINDPORT, &port);
        ssh_bind_options_set(sshBind, SSH_BIND_OPTIONS_BINDADDR, szIp.toStdString().c_str());

        if (!InitHostKey(sshBind)) {
            ssh_bind_free(sshBind);
            return ret;
        }

        if (ssh_bind_listen(sshBind) < 0) {
            qCritical(log) << "Failed to bind:" << ssh_get_error(sshBind);
            ssh_bind_free(sshBind);
            return ret;
        }

        auto fd = ssh_bind_get_fd(sshBind);
        auto pListenNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        if(pListenNotifier) {
            bool check = connect(pListenNotifier, &QSocketNotifier::activated,
                                 this, &CBackendSftpServer::slotNewConnection);
            Q_ASSERT(check);
        }
        pData->sshBind = sshBind;
        pData->pListenNotifier = pListenNotifier;
        m_lstListen.push_back(pData);
        return 0;
    } while(0);
    delete pData;
    return -1;
}

ssh_bind CBackendSftpServer::GetSshBind(QSocketNotifier* pNotifier)
{
    foreach(auto d, m_lstListen) {
        if(d->pListenNotifier == pNotifier)
            return d->sshBind;
    }
    return nullptr;
}

void CBackendSftpServer::slotNewConnection()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = SSH_ERROR;

    QSocketNotifier* pSocketNotifier = qobject_cast<QSocketNotifier*>(sender());
    if(!pSocketNotifier) return;

    // 接受新连接
    ssh_session session = ssh_new();
    if (!session) {
        qCritical(log) << "Failed to create SSH session";
        return;
    }

    struct sClientData* pData = new sClientData();
    memset(pData, 0, sizeof(struct sClientData));
    pData->session = session;
    pData->m_Time = QTime::currentTime();
    pData->pPara = m_pPara;
    m_lstClients.push_back(pData);

    memset(&pData->channel_cb, 0, sizeof(ssh_channel_callbacks_struct));
    pData->channel_cb.userdata = &(pData->sftp);
    pData->channel_cb.channel_data_function = sftp_channel_default_data_callback;
    pData->channel_cb.channel_subsystem_request_function = sftp_channel_default_subsystem_request;
    ssh_callbacks_init(&pData->channel_cb);

    memset(&pData->server_cb, 0, sizeof(ssh_server_callbacks_struct));
    pData->server_cb.userdata = pData;
    pData->server_cb.auth_password_function = cbAuthPassword;
    pData->server_cb.channel_open_request_session_function = cbChannelOpen;
    ssh_callbacks_init(&pData->server_cb);

    if (authorizedkeys[0])
    {
        pData->server_cb.auth_pubkey_function = cbAuthPublickey;
        ssh_set_auth_methods(session, SSH_AUTH_METHOD_PASSWORD | SSH_AUTH_METHOD_PUBLICKEY);
    }
    else
        ssh_set_auth_methods(session, SSH_AUTH_METHOD_PASSWORD);

    nRet = ssh_set_server_callbacks(session, &pData->server_cb);
    if(SSH_OK != nRet)
        qCritical(log) << "Failed: Set server callbacks";

    // 设置为非阻塞模式
    ssh_set_blocking(session, 0);

    ssh_bind sshBind = GetSshBind(pSocketNotifier);
    if (ssh_bind_accept(sshBind, session) == SSH_ERROR) {
        qCritical(log) << "Failed to accept connection:" << ssh_get_error(sshBind);
        ssh_free(session);
        return;
    }

    if (ssh_handle_key_exchange(session) != SSH_OK)
    {
        qCritical(log) << "Failed: Key exchange:" << ssh_get_error(session);
        //return;
    }

    nRet = ssh_event_add_session(m_event, session);
    if(SSH_OK != nRet)
        qCritical(log) << "Failed: event add session";
}

int CBackendSftpServer::cbAuthPassword(ssh_session session, const char *userName,
                         const char *pass, void *userdata)
{
    qDebug(log) << Q_FUNC_INFO;
    struct sClientData *pData = (struct sClientData *)userdata;

    CParameterSftpServer* para = pData->pPara;
    if(!para) return SSH_AUTH_DENIED;
    auto &user = para->m_Net.m_User;
    if (userName == user.GetUser() && pass == user.GetPassword())
    {
        SendBanner(session, userName);
        return SSH_AUTH_SUCCESS;
    }
    pData->m_nAuthAttempts++;
    return SSH_AUTH_DENIED;
}

int CBackendSftpServer::cbAuthPublickey(ssh_session session,
                          const char *user,
                          struct ssh_key_struct *pubkey,
                          char signature_state,
                          void *userdata)
{
    qDebug(log) << Q_FUNC_INFO;
    struct sClientData *pData = (struct sClientData *)userdata;

    if (signature_state == SSH_PUBLICKEY_STATE_NONE)
    {
        SendBanner(session, user);
        return SSH_AUTH_SUCCESS;
    }

    if (signature_state != SSH_PUBLICKEY_STATE_VALID)
    {
        return SSH_AUTH_DENIED;
    }

    /*
    // valid so far.  Now look through authorized keys for a match
    if (authorizedkeys[0])
    {
        ssh_key key = NULL;
        int result;
        struct stat buf;

        if (stat(authorizedkeys, &buf) == 0)
        {
            result = ssh_pki_import_pubkey_file(authorizedkeys, &key);
            if ((result != SSH_OK) || (key == NULL))
            {
                qDebug(log) << "Unable to import public key file:" << authorizedkeys;
            }
            else
            {
                result = ssh_key_cmp(key, pubkey, SSH_KEY_CMP_PUBLIC);
                ssh_key_free(key);
                if (result == 0)
                {
                    SendBanner(session);
                    return SSH_AUTH_SUCCESS;
                }
            }
        }
    }//*/

    // no matches
    return SSH_AUTH_DENIED;
}

ssh_channel CBackendSftpServer::cbChannelOpen(ssh_session session, void *userdata)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!session || !userdata) {
        return nullptr;
    }
    struct sClientData *pData = (struct sClientData *)userdata;
    if(!pData)
        return nullptr;
    pData->channel = ssh_channel_new(session);
    return pData->channel;
}

int CBackendSftpServer::RemoveClient(sClientData *pClient)
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    if(!pClient)
        return -1;

    if(pClient->channel && ssh_channel_is_open(pClient->channel)) {
        ssh_channel_send_eof(pClient->channel);
        ssh_channel_close(pClient->channel);
    }

    if(pClient->session) {
        if(ssh_is_connected(pClient->session))
            ssh_disconnect(pClient->session);
        ssh_free(pClient->session);
    }

    delete pClient;
    return nRet;
}

void CBackendSftpServer::SendBanner(ssh_session session, const QString& user)
{
    QString szBanner = "\n" + tr("Welcome to \"Rabbit Remote Control - SFTP Server\"") + "\n";
    szBanner += " * " + tr("Version:") + " " + QString(SftpServer_VERSION) + "\n";
    szBanner += " * " + tr("Home page") + ": https://github.com/KangLin/RabbitRemoteControl.git\n";
    szBanner += " * " + tr("Author: Kang Lin") + " <kl222@126.com>\n";
    szBanner += " * " + tr("Support") + ": https://github.com/KangLin/RabbitRemoteControl/issues\n";
    szBanner += tr("User \"%1\" logged in %2").arg(user, QDateTime::currentDateTime().toString()) + "\n\n";
    ssh_string banner = ssh_string_from_char(szBanner.toStdString().c_str());
    if (banner) {
        ssh_send_issue_banner(session, banner);
        ssh_string_free(banner);
    }
}
