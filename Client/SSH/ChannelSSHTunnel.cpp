// Author: Kang Lin <kl222@126.com>

#include "libssh/libssh.h"
#include "libssh/callbacks.h"

#include "ChannelSSHTunnel.h"
#include <QLoggingCategory>
#include <QThread>
#include <QDateTime>
#include <QAbstractEventDispatcher>
#include <QScopedArrayPointer>
#include <QtGlobal>
#if defined(Q_OS_LINUX)
    #include <sys/eventfd.h>
#endif
#if defined(Q_OS_WIN)
    #include <WinSock2.h>
    //#pragma comment(lib,"ws2_32.lib")
#endif

static Q_LOGGING_CATEGORY(log, "Channel.SSH.Tunnel")
static Q_LOGGING_CATEGORY(logSSH, "Channel.SSH.log")

CChannelSSHTunnel::CChannelSSHTunnel(
    CParameterSSHTunnel* parameter,
    CParameterNet *remote,
    CConnect *pConnect,
    bool bWakeUp,
    QObject *parent)
    : CChannel(parent),
    m_Session(NULL),
    m_Channel(NULL),
    m_pConnect(pConnect),
    m_pcapFile(NULL),
    m_pParameter(parameter),
    m_pRemoteNet(remote),
/*
    m_pSocketRead(nullptr),
    m_pSocketWrite(nullptr),
    m_pSocketException(nullptr),
*/
    m_pEvent(nullptr)
{
    qDebug(log) << "CChannelSSHTunnel::CChannelSSHTunnel()";
    qDebug(log) << "libssh version:" << ssh_version(0);

    if(bWakeUp)
        m_pEvent = new Channel::CEvent(this);
}

CChannelSSHTunnel::~CChannelSSHTunnel()
{
    qDebug(log) << "CChannelSSHTunnel::~CChannelSSHTunnel()";
    if(m_pEvent)
        delete m_pEvent;
}

QString CChannelSSHTunnel::GetDetails()
{
    return tr("- libssh version: ") + ssh_version(0);
}

void CChannelSSHTunnel::cb_log(ssh_session session,
                               int priority,
                               const char *message,
                               void *userdata)
{
    switch (priority) {
    case SSH_LOG_WARN:
        qWarning(logSSH) << message;
        break;
    case SSH_LOG_INFO:
        qInfo(logSSH) << message;
    case SSH_LOG_DEBUG:
    case SSH_LOG_TRACE:
        qDebug(logSSH) << message;
    default:
        break;
    }   
}

int CChannelSSHTunnel::GetSocket()
{
    if(m_Session)
        return ssh_get_fd(m_Session);
    return SSH_INVALID_SOCKET;
}

int CChannelSSHTunnel::WakeUp()
{
    if(!m_pEvent) return 0;
    return m_pEvent->WakeUp();
}

bool CChannelSSHTunnel::open(OpenMode mode)
{
    int nRet = 0;
    QString szErr;

    if(!m_pParameter) {
        qCritical(log) << "The parameter is null";
    }
    if(!m_pRemoteNet)
        qCritical(log) << "The remote net parameter is null";
    Q_ASSERT(m_pParameter);
    Q_ASSERT(m_pRemoteNet);

    bool bRet = QIODevice::open(mode);
    if(!bRet)
        return bRet;

    m_Session = ssh_new();
    if(NULL == m_Session)
    {
        szErr = tr("SSH failed: ssh_new.");
        qCritical(log) << szErr;
        setErrorString(szErr);
        return false;
    }

    do{
        struct ssh_callbacks_struct cb;
        memset(&cb, 0, sizeof(struct ssh_callbacks_struct));
        cb.userdata = this,
        cb.log_function = cb_log;;
        ssh_callbacks_init(&cb);
        ssh_set_callbacks(m_Session, &cb);

        /*
        int value = 1;
        ssh_options_set(m_Session, SSH_OPTIONS_NODELAY, (const void*)&value);//*/

        /*
            See: [ssh_options_set()](https://api.libssh.org/stable/group__libssh__session.html#ga7a801b85800baa3f4e16f5b47db0a73d)
            SSH_LOG_NOLOG: No logging
            SSH_LOG_WARNING: Only warnings
            SSH_LOG_PROTOCOL: High level protocol information
            SSH_LOG_PACKET: Lower level protocol information, packet level
            SSH_LOG_FUNCTIONS: Every function path The default is SSH_LOG_NOLOG.
         */
        int verbosity = SSH_LOG_NOLOG;
        ssh_options_set(m_Session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);

        auto &net = m_pParameter->m_Net;
        if(net.GetHost().isEmpty()) {
            szErr = tr("SSH failed: the server is empty");
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        } else {
            nRet = ssh_options_set(m_Session, SSH_OPTIONS_HOST,
                                   net.GetHost().toStdString().c_str());
            if(nRet) {
                szErr = tr("SSH failed: Set host fail. host:")
                        + net.GetHost() + ";"
                        + ssh_get_error(m_Session);
                qCritical(log) << szErr;
                setErrorString(szErr);
                break;
            }
        }

        uint nPort = net.GetPort();
        nRet = ssh_options_set(m_Session, SSH_OPTIONS_PORT, &nPort);
        if(nRet) {
            szErr = tr("SSH failed: Set port fail. port:")
                    + QString::number(net.GetPort()) + ";"
                    + ssh_get_error(m_Session);
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        }

        if(!m_pParameter->GetPcapFile().isEmpty())
        {
            m_pcapFile = ssh_pcap_file_new();
            if(m_pcapFile) {
                if (ssh_pcap_file_open(
                        m_pcapFile, m_pParameter->GetPcapFile().toStdString().c_str())
                    == SSH_ERROR) {
                    qCritical(log) << "SSH failed: Error opening pcap file: "
                                   << m_pParameter->GetPcapFile();
                    ssh_pcap_file_free(m_pcapFile);
                    m_pcapFile = nullptr;
                }
                if(m_pcapFile)
                    ssh_set_pcap_file(m_Session, m_pcapFile);
            } else {
                szErr = tr("SSH failed: ssh_pcap_file_new: ")
                       + ssh_get_error(m_Session);
                qCritical(log) << szErr;
            }
        }

        nRet = ssh_connect(m_Session);
        if(nRet) {
            szErr = tr("SSH failed: ssh connect ")
                    + net.GetHost()
                    + ":" + QString::number(net.GetPort())
                    + " ("
                    + ssh_get_error(m_Session);
            szErr += ")";
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        }

        nRet = verifyKnownhost(m_Session);
        if(nRet){
            break;
        }

        auto &user = m_pParameter->m_Net.m_User;
        if(((user.GetUsedType() == CParameterUser::TYPE::UserPassword)
             && (user.GetPassword().isEmpty() || user.GetUser().isEmpty()))
            || ((user.GetUsedType() == CParameterUser::TYPE::PublicKey)
                && user.GetPassphrase().isEmpty())
            && m_pConnect) {
            emit m_pConnect->sigBlockShowWidget("CDlgUserPassword", nRet, &m_pParameter->m_Net);
            if(QDialog::Accepted != nRet)
            {
                setErrorString(tr("User cancel"));
                return false;
            }
        }

        int nMeth = SSH_AUTH_METHOD_PUBLICKEY;
        switch(user.GetUsedType()) {
        case CParameterUser::TYPE::UserPassword:
            nMeth = SSH_AUTH_METHOD_PASSWORD;
            break;
        case CParameterUser::TYPE::PublicKey:
            nMeth = SSH_AUTH_METHOD_PUBLICKEY;
            break;
        }

        nRet = authentication(m_Session,
                              user.GetUser(),
                              user.GetPassword(),
                              user.GetPassphrase(),
                              nMeth);
        if(nRet) break;

        nRet = forward(m_Session);
        if(nRet) break;

        emit sigConnected();

        return bRet;
    } while(0);

    ssh_disconnect(m_Session);
    ssh_free(m_Session);
    m_Session = NULL;
    return false;
}

void CChannelSSHTunnel::close()
{
    qDebug(log) << "CChannelSSHTunnel::close()";

    if(!isOpen()) return;

    WakeUp();

    /*
    QAbstractEventDispatcher* pDispatcher = QAbstractEventDispatcher::instance();
    if(m_pSocketRead) {
        pDispatcher->unregisterSocketNotifier(m_pSocketRead);
        m_pSocketRead->deleteLater();
        m_pSocketRead = nullptr;
    }
    if(m_pSocketWrite) {
        pDispatcher->unregisterSocketNotifier(m_pSocketWrite);
        m_pSocketWrite->deleteLater();
        m_pSocketWrite = nullptr;
    }
    if(m_pSocketException) {
        pDispatcher->unregisterSocketNotifier(m_pSocketException);
        m_pSocketException->deleteLater();
        m_pSocketException = nullptr;
    }//*/

    if(m_Channel) {
        if(ssh_channel_is_open(m_Channel)) {
            ssh_channel_close(m_Channel);
        }
        ssh_channel_free(m_Channel);
        m_Channel = NULL;
    }

    if(m_Session) {
        if(ssh_is_connected(m_Session))
            ssh_disconnect(m_Session);
        ssh_free(m_Session);
        m_Session = NULL;
    }

    if(m_pcapFile)
    {
        ssh_pcap_file_close(m_pcapFile);
        ssh_pcap_file_free(m_pcapFile);
        m_pcapFile = nullptr;
    }

    QIODevice::close();
}

int CChannelSSHTunnel::verifyKnownhost(ssh_session session)
{
    int nRet = -1;
    QString szErr;
    ssh_key srv_pubkey = NULL;

    auto &net = m_pParameter->m_Net;

    nRet = ssh_get_server_publickey(session, &srv_pubkey);
    if (nRet < 0) {
        szErr = tr("SSH failed: Get server public key.") + " " + net.GetHost() + "; [";
        szErr += ssh_get_error(session);
        szErr += "]";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }
    unsigned char *hash = NULL;
    size_t nLen = 0;
    nRet  = ssh_get_publickey_hash(srv_pubkey,
                                  SSH_PUBLICKEY_HASH_SHA1,
                                  &hash,
                                  &nLen);
    ssh_key_free(srv_pubkey);
    if(nRet) {
        szErr = tr("SSH failed: Get public key hash value fail.");
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -2;
    }
    QByteArray baHash((const char*)hash, nLen);
    QString szHash = baHash.toHex(':').toStdString().c_str();
    ssh_clean_pubkey_hash(&hash);

    QMessageBox::StandardButton btRet = QMessageBox::Yes;
    bool checkBox = false;
    enum ssh_known_hosts_e state = ssh_session_is_known_server(session);
    switch(state) {
    case SSH_KNOWN_HOSTS_OK:
        nRet = 0;
        break;
    case SSH_KNOWN_HOSTS_CHANGED:
        nRet = -3;
        szErr = net.GetHost() + " " + tr("the host key for server changed. it is now:") + "\n";
        szErr += szHash + "\n";
        szErr += tr("For security reasons, connection will be stopped.") + "\n";
        szErr += tr("Please look at the OpenSSL documentation on "
                  "how to add a private CA to the store.");
        qCritical(log) << szErr;
        setErrorString(szErr);
        break;
    case SSH_KNOWN_HOSTS_OTHER:
        nRet = -4;
        szErr = net.GetHost() + " " + tr("the host key for this server was not found but an other type of key exists.") + "\n";
        szErr += tr("An attacker might change the default server key to "
                  "confuse your client into thinking the key does not exist") + "\n";
        szErr += tr("For security reasons, connection will be stopped.") + "\n";
        szErr += tr("Please look at the OpenSSL documentation on "
                    "how to add a private CA to the store.");
        qCritical(log) << szErr;
        setErrorString(szErr);
        break;
    case SSH_KNOWN_HOSTS_NOT_FOUND:
        nRet = -5;
        szErr = net.GetHost() + " " + tr("is not find in known host file.") + "\n";
        szErr += tr("If you accept the host key here, the file will be "
                    "automatically created.") + "\n";
        szErr += tr("Host key hash:") + "\n" + szHash;
        qDebug(log) << szErr;
        if(!m_pConnect) break;
        emit m_pConnect->sigBlockShowMessageBox(tr("Error"), szErr,
#ifndef Q_OS_ANDROID
                                    QMessageBox::Yes |
#endif
                                    QMessageBox::No | QMessageBox::Ignore,
                                    btRet, checkBox);
        if(QMessageBox::Yes == btRet) {
            nRet = ssh_session_update_known_hosts(session);
            if(nRet)
            {
                qCritical(log) << "ssh_session_update_known_hosts fail."
                               << ssh_get_error(session);
            }
        } if(QMessageBox::Ignore == btRet)
            nRet = 0;
        else
            setErrorString(tr("Reject the host key"));
        break;
    case SSH_KNOWN_HOSTS_UNKNOWN:
        nRet = -6;
        szErr = net.GetHost() + " " + tr("is unknown. Do you trust the host key?") + "\n";
        szErr += tr("Host key hash:") + "\n" + szHash;
        qDebug(log) << szErr;
        if(!m_pConnect) break;
        emit m_pConnect->sigBlockShowMessageBox(tr("Error"), szErr,
#ifndef Q_OS_ANDROID
                                    QMessageBox::Yes |
#endif
                                    QMessageBox::No | QMessageBox::Ignore,
                                    btRet, checkBox);
        if(QMessageBox::Yes == btRet) {
            nRet = ssh_session_update_known_hosts(session);
            if (SSH_OK != nRet) {
                qCritical(log) << "ssh_session_update_known_hosts fail."
                               << ssh_get_error(session);
            }
        } if(QMessageBox::Ignore == btRet)
            nRet = 0;
        else
            setErrorString(tr("Reject the host key"));
        break;
    case SSH_KNOWN_HOSTS_ERROR:
        nRet = -7;
        szErr = tr("Error:") + ssh_get_error(session) + "\n";
        szErr += net.GetHost() + " " + tr("the host key hash:") + "\n" + szHash + "\n";
        szErr += tr("Will be stopped.");
        qCritical(log) << szErr;
        setErrorString(szErr);
        emit sigError(nRet, szErr);
        break;
    }

    return nRet;
}

//! \see: https://api.libssh.org/stable/libssh_tutor_authentication.html
int CChannelSSHTunnel::authentication(
    ssh_session session,
    const QString szUser,
    const QString szPassword,
    const QString szPassphrase,
    const int nMethod)
{
    int nRet = 0;
    int nServerMethod = nMethod;

    qDebug(log) << "Authentication method:" << nMethod;
    //* Get authentication list from ssh server
    nRet = ssh_userauth_none(session,
                             szUser.toStdString().c_str());
    qDebug(log) << "ssh_userauth_none:" << nRet;
    if(SSH_AUTH_SUCCESS == nRet)
        return 0;
    
    char *banner = nullptr;
    banner = ssh_get_issue_banner(session);
    if (banner)
    {
        qDebug(log) << "banner:" << banner;
        free(banner);
    }

    nServerMethod = ssh_userauth_list(session,
                                      szUser.toStdString().c_str());
    qDebug(log) << "ssh_userauth_list:" << nServerMethod;
    //*/

    if(nServerMethod & nMethod & SSH_AUTH_METHOD_PUBLICKEY) {
        auto &user = m_pParameter->m_Net.m_User;
        if(user.GetUseSystemFile()) {
            qDebug(log) << "User authentication with ssh_userauth_publickey_auto";
            nRet = ssh_userauth_publickey_auto(session,
                                               szUser.toStdString().c_str(),
                                               szPassphrase.toStdString().c_str());
            if(SSH_AUTH_SUCCESS == nRet)
                return 0;
            QString szErr = tr("SSH failed: Failed authenticating with publickey:")
                            + ssh_get_error(m_Session);
            qCritical(log) << szErr;
            setErrorString(szErr);
        } else {
            qDebug(log) << "User authentication with publickey";
            nRet = authenticationPublicKey(
                m_Session,
                user.GetUser(),
                user.GetPublicKeyFile(),
                user.GetPrivateKeyFile(),
                user.GetPassphrase());
            if(SSH_AUTH_SUCCESS == nRet)
                return 0;
        }
    }

    if(nServerMethod & nMethod & SSH_AUTH_METHOD_PASSWORD) {
        qDebug(log) << "User authentication with password";

        nRet = ssh_userauth_password(session,
                                     szUser.toStdString().c_str(),
                                     szPassword.toStdString().c_str());
        if(nRet) {
            QString szErr = tr("Failed authenticating with password. User: ")
                            + szUser + "; "
                            + ssh_get_error(session);
            qCritical(log) << szErr;
            setErrorString(szErr);
            return nRet;
        }
    }

    return nRet;
}

int CChannelSSHTunnel::authenticationPublicKey(
    ssh_session session,
    const QString szUser,
    const QString szPublicKeyFile,
    const QString szPrivateKeyFile,
    const QString szPassphrase)
{
    int nRet = 0;
    QString szErr;
    ssh_key publicKey = NULL;
    ssh_key privateKey = NULL;

    do {
        if(szPublicKeyFile.isEmpty())
        {
            szErr = tr("SSH failed: There is not set public key file.");
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        }
        nRet = ssh_pki_import_pubkey_file(
            szPublicKeyFile.toStdString().c_str(),
            &publicKey);
        if(SSH_OK != nRet) {
            szErr = tr("SSH failed: Import public key fail.") + szPublicKeyFile;
            if(SSH_EOF == nRet)
                szErr += "\n" + tr("The file doesn't exist or permission denied:");
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        }

        nRet = ssh_userauth_try_publickey(
            session,
            szUser.toStdString().c_str(),
            publicKey);
        if(SSH_AUTH_SUCCESS != nRet)
        {
            szErr = tr("SSH failed: Authentication failed. User:") + szUser + "\n";
            szErr += ssh_get_error(session);
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        }
        
        if(szPrivateKeyFile.isEmpty())
        {
            szErr = tr("SSH failed: There is not set private key file.");
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        }
        nRet = ssh_pki_import_privkey_file(
            szPrivateKeyFile.toStdString().c_str(),
            szPassphrase.toStdString().c_str(),
            NULL, NULL, &privateKey);
        if(SSH_OK != nRet) {
            
            szErr = tr("SSH failed: Import private key fail.") + szPrivateKeyFile;
            if(SSH_EOF == nRet)
                szErr += "\n" + tr("The file doesn't exist or permission denied:");
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        }

        nRet = ssh_userauth_publickey(
            session,
            szUser.toStdString().c_str(),
            privateKey);
        if(SSH_AUTH_SUCCESS != nRet) {
            szErr = tr("SSH failed: Authentication failed. User:") + szUser + "\n";
            szErr += ssh_get_error(session);            
            qCritical(log) << szErr;
            setErrorString(szErr);
        }

    } while(0);

    if(publicKey)
        ssh_key_free(publicKey);
    if(privateKey)
        ssh_key_free(privateKey);

    return nRet;
}

int CChannelSSHTunnel::forward(ssh_session session)
{
    int nRet = 0;

    Q_ASSERT(session);
    
    m_Channel = ssh_channel_new(session);
    if(NULL == m_Channel) {
        qCritical(log) << "ssh_channel_new fail." << ssh_get_error(session);
        return -1;
    }

    nRet = ssh_channel_open_forward(
        m_Channel,
        m_pRemoteNet->GetHost().toStdString().c_str(),
        m_pRemoteNet->GetPort(),
        m_pParameter->GetSourceHost().toStdString().c_str(),
        m_pParameter->GetSourcePort());
    if(SSH_OK != nRet) {
        ssh_channel_free(m_Channel);
        m_Channel = NULL;

        QString szErr;
        szErr = tr("SSH failed: open forward.") + ssh_get_error(session);
        szErr += "(" + m_pRemoteNet->GetHost()
                + ":" + QString::number(m_pRemoteNet->GetPort()) + ")";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return nRet;
    }

    qInfo(log) << "Connected:"
               << m_pRemoteNet->GetHost()
                      + ":" + QString::number(m_pRemoteNet->GetPort())
               << "with ssh turnnel:"
               << m_pParameter->m_Net.GetHost()
                      + ":" + QString::number(m_pParameter->m_Net.GetPort());

    //ssh_channel_set_blocking(m_Channel, 0);

    return nRet;
}

/*!
 * \return
 *   \li >= 0: continue, Interval call time (msec)
 *   \li = -1: stop
 *   \li < -1: error
 */
int CChannelSSHTunnel::Process()
{
    int nRet = 0;
    
    if(!m_Channel || !ssh_channel_is_open(m_Channel)
        || ssh_channel_is_eof(m_Channel)) {
        QString szErr = "The channel is not open";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }

    struct timeval timeout = {0, 5000000};
    ssh_channel channels[2], channel_out[2];
    channels[0] = m_Channel;
    channels[1] = nullptr;
    
    fd_set set;
    FD_ZERO(&set);
    socket_t fd = SSH_INVALID_SOCKET;
    if(m_pEvent)
        fd = m_pEvent->GetFd();
    if(SSH_INVALID_SOCKET != fd)
        FD_SET(fd, &set);
    
    //qDebug(log) << "ssh_select:" << fd;
    nRet = ssh_select(channels, channel_out, fd + 1, &set, &timeout);
    //qDebug(log) << "ssh_select end:" << nRet;
    if(EINTR == nRet)
        return 0;

    if(SSH_OK != nRet) {
        QString szErr;
        szErr = "ssh_channel_select failed: " + QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -3;
    }

    if(SSH_INVALID_SOCKET != fd && FD_ISSET(fd, &set)) {
        //qDebug(log) << "fires event";
        if(m_pEvent) {
            nRet = m_pEvent->Reset();
            if(nRet) return -4;
        }
    }

    if(!channel_out[0]) {
        //qDebug(log) << "The channel is not select";
        return 0;
    }

    if(ssh_channel_is_eof(m_Channel)) {
        qWarning(log) << "Channel is eof";
        setErrorString(tr("The channel is eof"));
        // Stop
        return -1;
    }

    // Get channel data length
    nRet = ssh_channel_poll(m_Channel, 0);
    //qDebug(log) << "Get channel data length:" << nRet;
    if(SSH_ERROR == nRet)
    {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        return -6;
    } else if(SSH_EOF == nRet) {
        // Stop
        return -1;
    } else if(0 > nRet) {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        // Error
        return -7;
    } else if(0 == nRet) {
        //qDebug(log) << "The channel has not data";
        return 0;
    }

    emit readyRead();

    return 0;
}

// Because is same thread
qint64 CChannelSSHTunnel::readData(char *data, qint64 maxlen)
{
    qint64 nRet = 0;

    //qDebug(log) << Q_FUNC_INFO << maxlen;

    Q_ASSERT(data && maxlen >= 0);
    if(nullptr == data || 0 > maxlen) {
        qCritical(log) << Q_FUNC_INFO << "The parameters is invalid" << maxlen;
        return -1;
    }

    if(0 == maxlen) {
        qCritical(log) << Q_FUNC_INFO << "maxlen:" << maxlen;
        return 0;
    }

    if(!m_Channel || !ssh_channel_is_open(m_Channel))
    {
        QString szErr;
        szErr = "The channel is not opened";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }

    nRet = ssh_channel_read_nonblocking(m_Channel, data, maxlen, 0);
    if(SSH_AGAIN == nRet) {
        qDebug(log) << Q_FUNC_INFO << "ssh again read";
        return 0;
    } else if(0 > nRet) {
        QString szErr;
        szErr = "Read data from channel failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        return nRet;
    }

    return nRet;
}

qint64 CChannelSSHTunnel::writeData(const char *data, qint64 len)
{
    qint64 nRet = 0;

    Q_ASSERT(data && len >= 0);
    if(nullptr == data || 0 > len) {
        qCritical(log) << Q_FUNC_INFO << "The parameters is invalid" << len;
        return -1;
    }

    if(0 == len) {
        qCritical(log) << Q_FUNC_INFO << "len:" << len;
        return 0;
    }

    if(!m_Channel || !ssh_channel_is_open(m_Channel) || ssh_channel_is_eof(m_Channel))
    {
        QString szErr;
        szErr = "The channel is not opened";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }

    nRet = ssh_channel_write(m_Channel, data, len);
    if(SSH_AGAIN == nRet) {
        qDebug(log) << Q_FUNC_INFO << "ssh again write";
        return 0;
    } else if(nRet < 0) {
        QString szErr;
        szErr = "Write data from channel failed:";
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -2;
    }

    return nRet;
}

int CChannelSSHTunnel::DoWait(bool bWrite, int timeout)
{
    int nRet = 0;
    if(!m_Channel || !ssh_channel_is_open(m_Channel)
        || ssh_channel_is_eof(m_Channel)) {
        QString szErr = "The channel is not open";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }
    
    fd_set set;
    FD_ZERO(&set);

    struct timeval tm = {0, timeout};
    ssh_channel channels[2], channel_out[2];
    channels[0] = m_Channel;
    channels[1] = nullptr;
    
    if(bWrite) {
        socket_t fd = SSH_INVALID_SOCKET;
        if(m_pEvent)
            fd = GetSocket();
        if(SSH_INVALID_SOCKET != fd)
            FD_SET(fd, &set);
        nRet = select(fd + 1, nullptr, &set, nullptr, &tm);
        if(0 > nRet) return nRet;
        return 0;
    }

    //qDebug(log) << "ssh_select:" << fd;
    nRet = ssh_select(channels, channel_out, 1, &set, &tm);
    //qDebug(log) << "ssh_select end:" << nRet;
    if(EINTR == nRet)
        return 0;
    
    if(SSH_OK != nRet) {
        QString szErr;
        szErr = "ssh_channel_select failed: " + QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -3;
    }
    
    if(!channel_out[0]) {
        //qDebug(log) << "The channel is not select";
        return 0;
    }

    if(ssh_channel_is_eof(m_Channel)) {
        qWarning(log) << "Channel is eof";
        setErrorString(tr("The channel is eof"));
        // Stop
        return -1;
    }
    
    // Get channel data length
    nRet = ssh_channel_poll(m_Channel, 0);
    //qDebug(log) << "Get channel data length:" << nRet;
    if(SSH_ERROR == nRet)
    {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        return -6;
    } else if(SSH_EOF == nRet) {
        // Stop
        return -1;
    } else if(0 > nRet) {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        // Error
        return -7;
    } else if(0 == nRet) {
        //qDebug(log) << "The channel has not data";
        return 0;
    }
    return 0;
}

/*
int CChannelSSHTunnel::ProcessSocket()
{
    int nRet = 0;
    bool check = false;
    socket_t fd = ssh_get_fd(m_Session);
    m_pSocketRead = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    if(m_pSocketRead) {
        check = connect(
            m_pSocketRead, &QSocketNotifier::activated,
            this, [&](int fd) {
                qDebug(log) << "QSocketNotifier::activated: read";
                Q_UNUSED(fd)
                emit this->readyRead();
            });
        Q_ASSERT(check);
    }
    
    // m_pSocketWrite = new QSocketNotifier(fd, QSocketNotifier::Write, this);
    // if(m_pSocketWrite) {
    //     check = connect(
    //         m_pSocketWrite, &QSocketNotifier::activated,
    //         this, [&](int fd){
    //             Q_UNUSED(fd)
    //             qDebug(log) << "QSocketNotifier::activated: write";
    //         });
    //     Q_ASSERT(check);
    // }
    
    m_pSocketException = new QSocketNotifier(fd, QSocketNotifier::Exception, this);
    if(m_pSocketException) {
        check = connect(
            m_pSocketException, &QSocketNotifier::activated,
            this, [&](int) {
                qDebug(log) << "QSocketNotifier::activated: Exception";
                QString szErr;
                szErr = "Channel exception:";
                szErr += ssh_get_error(m_Session);
                qCritical(log) << szErr;
                emit sigError(-1, szErr);
            });
        Q_ASSERT(check);
    }
    
    return nRet;
}
//*/
