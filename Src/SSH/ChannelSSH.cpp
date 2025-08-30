#include "ChannelSSH.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "SSH")
static Q_LOGGING_CATEGORY(logSSH, "SSH.log")

CChannelSSH::CChannelSSH(CBackend *pBackend, CParameterSSH *pPara,
                         bool bWakeUp, QObject *parent)
    : CChannel{parent}
    , m_Session(nullptr)
    , m_Channel(nullptr)
    , m_pBackend(pBackend)
    , m_pParameter(pPara)
    , m_pEvent(nullptr)
    , m_pcapFile(nullptr)
{
    if(bWakeUp)
        m_pEvent = new Channel::CEvent(this);
}

CChannelSSH::~CChannelSSH()
{
    qDebug(log) << Q_FUNC_INFO;
}

QString CChannelSSH::GetDetails()
{
    QString szDetail = "- " + tr("libssh version:") + " " + ssh_version(0) + "\n";
    szDetail += "  - " + tr("libssh protocol version:") + " " + QString::number(ssh_get_version(m_Session)) + "\n";
    szDetail += "  - " + tr("OpenSSH server:") + " " + QString::number(ssh_get_openssh_version(m_Session)) + "\n";
    return szDetail;
}

void CChannelSSH::cb_log(ssh_session session,
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

int CChannelSSH::WakeUp()
{
    if(!m_pEvent) return 0;
    return m_pEvent->WakeUp();
}

bool CChannelSSH::open(OpenMode mode)
{
    int nRet = 0;
    QString szErr;
    
    if(!m_pParameter) {
        qCritical(log) << "The parameter is null";
    }
    Q_ASSERT(m_pParameter);
    
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
        cb.userdata = this;
        cb.log_function = cb_log;
        ssh_callbacks_init(&cb);
        ssh_set_callbacks(m_Session, &cb);
        ssh_set_log_level(SSH_LOG_TRACE);

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
                   && m_pBackend) {
            emit m_pBackend->sigBlockShowWidget("CDlgUserPassword", nRet, &m_pParameter->m_Net);
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
        
        nRet = OnOpen(m_Session);
        if(nRet) break;
        
        emit sigConnected();
        
        return bRet;
    } while(0);
    
    ssh_disconnect(m_Session);
    ssh_free(m_Session);
    m_Session = NULL;
    return false;
}

void CChannelSSH::close()
{
    qDebug(log) << Q_FUNC_INFO;
    
    if(!isOpen()) return;
    
    OnClose();
    
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

int CChannelSSH::verifyKnownhost(ssh_session session)
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
        szErr = net.GetHost() + " " + tr("is not found in known host file.") + "\n";
        szErr += tr("If you accept the host key here, the file will be "
                    "automatically created.") + "\n";
        szErr += tr("Host key hash:") + "\n" + szHash;
        qDebug(log) << szErr;
        if(!m_pBackend) break;
        emit m_pBackend->sigBlockShowMessageBox(tr("Error"), szErr,
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
        if(!m_pBackend) break;
        emit m_pBackend->sigBlockShowMessageBox(tr("Error"), szErr,
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
int CChannelSSH::authentication(
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
        qInfo(log) << "banner:" << banner;
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

int CChannelSSH::authenticationPublicKey(
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
            szErr = tr("SSH failed: There is no public key file defined.");
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
            szErr = tr("SSH failed: There is no private key file defined.");
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

int CChannelSSH::OnOpen(ssh_session session)
{
    int nRet = 0;
    
    return nRet;
}

void CChannelSSH::OnClose()
{
}
