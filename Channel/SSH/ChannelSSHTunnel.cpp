#include "ChannelSSHTunnel.h"
#include <QLoggingCategory>
#include <QThread>
#include <QDateTime>
#include <QAbstractEventDispatcher>
#include <QtGlobal>

static Q_LOGGING_CATEGORY(log, "Channel.SSH.Tunnel")
static Q_LOGGING_CATEGORY(logSSH, "Channel.SSH.log")

CChannelSSHTunnel::CChannelSSHTunnel(
    QSharedPointer<CParameterChannelSSH> parameter,
    QObject *parent)
    : CChannel(parent),
    m_Session(NULL),
    m_Channel(NULL),
    m_pcapFile(NULL),
    m_Parameter(parameter),
    m_pSocketRead(nullptr),
    m_pSocketWrite(nullptr),
    m_pSocketException(nullptr)
{
    Q_ASSERT(m_Parameter);
}

qint64 CChannelSSHTunnel::readData(char *data, qint64 maxlen)
{
    /*
    qDebug(log) << "CChannelSSHTunnel::readData:"
                << maxlen << "nLen:" << m_readData.size();//*/
    qint64 nLen = 0;
    m_readMutex.lock();
    nLen = m_readData.size();
    nLen = qMin(nLen, maxlen);
    if(nLen > 0) {
        memcpy(data, m_readData.data(), nLen);
        m_readData.remove(0, nLen);
    }
    m_readMutex.unlock();
    return nLen;
}

qint64 CChannelSSHTunnel::writeData(const char *data, qint64 len)
{
    //qDebug(log) << "CChannelSSHTunnel::writeData:" << len;
    m_writeMutex.lock();
    m_writeData.append(data, len);
    m_writeMutex.unlock();
    //TODO: Add walkup
    return len;
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

bool CChannelSSHTunnel::open(OpenMode mode)
{
    int nRet = 0;
    QString szErr;
    
    m_Session = ssh_new();
    if(NULL == m_Session)
    {
        szErr = tr("SSH failed: ssh_new.");
        qCritical(log) << szErr;
        setErrorString(szErr);
        return false;
    }

    do{
        if(!m_Parameter) {
            qCritical(log) << "The parameter is null";
        }
        Q_ASSERT(m_Parameter);
        
        struct ssh_callbacks_struct cb;
        memset(&cb, 0, sizeof(struct ssh_callbacks_struct));
        cb.userdata = this,
        cb.log_function = cb_log;;
        ssh_callbacks_init(&cb);
        ssh_set_callbacks(m_Session, &cb);
        
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
        
        if(!m_Parameter->GetServer().isEmpty()){
            nRet = ssh_options_set(m_Session, SSH_OPTIONS_HOST,
                                   m_Parameter->GetServer().toStdString().c_str());
            if(nRet) {
                szErr = tr("SSH failed: Set host fail. host:")
                        + m_Parameter->GetServer() + ";"
                        + ssh_get_error(m_Session);
                qCritical(log) << szErr;
                setErrorString(szErr);
                break;
            }
        }

        uint nPort = m_Parameter->GetPort();
        nRet = ssh_options_set(m_Session, SSH_OPTIONS_PORT, &nPort);
        if(nRet) {
            szErr = tr("SSH failed: Set port fail. port:")
                    + QString::number(m_Parameter->GetPort()) + ";"
                    + ssh_get_error(m_Session);
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        }
        
        if(!m_Parameter->GetPcapFile().isEmpty())
        {
            m_pcapFile = ssh_pcap_file_new();
            if(m_pcapFile) {
                if (ssh_pcap_file_open(m_pcapFile,
                                       m_Parameter->GetPcapFile().toStdString().c_str())
                    == SSH_ERROR) {
                    qCritical(log) << "SSH failed: Error opening pcap file: "
                                   << m_Parameter->GetPcapFile();
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
                    + m_Parameter->GetServer()
                    + ":" + QString::number(m_Parameter->GetPort())
                    + " ("
                    + ssh_get_error(m_Session);
            szErr += ")";
            qCritical(log) << szErr;
            setErrorString(szErr);
            break;
        }
        
        nRet = verifyKnownhost(m_Session);
        if(nRet) break;
        
        nRet = authentication(m_Session,
                                  m_Parameter->GetUser(),
                                  m_Parameter->GetPassword(),
                                  m_Parameter->GetPassphrase(),
                                  m_Parameter->GetAuthenticationMethod());
        if(nRet) break;
        
        nRet = forward(m_Session);
        if(nRet) break;

        return QIODevice::open(mode);
    } while(0);

    ssh_disconnect(m_Session);
    ssh_free(m_Session);
    m_Session = NULL;
    return false;
}

void CChannelSSHTunnel::close()
{
    qDebug(log) << "CChannelSSHTunnel::close()";

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
    }
    
    if(m_pcapFile)
    {
        ssh_pcap_file_close(m_pcapFile);
        ssh_pcap_file_free(m_pcapFile);
        m_pcapFile = nullptr;
    }
    
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

    QIODevice::close();
}

int CChannelSSHTunnel::verifyKnownhost(ssh_session session)
{
    int nRet = -1;
    QString szErr;
    ssh_key srv_pubkey = NULL;
    nRet = ssh_get_server_publickey(session, &srv_pubkey);
    if (nRet < 0) {
        szErr = tr("SSH failed: Get server public key.") + "(";
        szErr += ssh_get_error(session);
        szErr += ")";
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
    
    QMessageBox::StandardButton btRet;
    bool checkBox = false;
    enum ssh_known_hosts_e state = ssh_session_is_known_server(session);
    switch(state) {
    case SSH_KNOWN_HOSTS_OK:
        nRet = 0;
        break;
    case SSH_KNOWN_HOSTS_CHANGED:
        nRet = -3;
        szErr = tr("Host key for server changed. it is now:") + "\n";
        szErr += szHash + "\n";
        szErr += tr("For security reasons, connection will be stopped.") + "\n";
        szErr += tr("Please look at the OpenSSL documentation on "
                  "how to add a private CA to the store.");
        qCritical(log) << szErr;
        setErrorString(szErr);
        break;
    case SSH_KNOWN_HOSTS_OTHER:
        nRet = -4;
        szErr = tr("The host key for this server was not found but an other type of key exists.") + "\n";
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
        szErr = tr("Could not find known host file.") + "\n";
        szErr += tr("If you accept the host key here, the file will be "
                    "automatically created.") + "\n";
        szErr += tr("Host key hash:") + "\n" + szHash;
        qDebug(log) << szErr;
        emit sigBlockShowMessageBox(tr("Error"), szErr,
                                    QMessageBox::Yes | QMessageBox::No,
                                    btRet, checkBox);
        if(QMessageBox::Yes == btRet) {
            nRet = ssh_session_update_known_hosts(session);
            if(nRet)
            {
                qCritical(log) << "ssh_session_update_known_hosts fail."
                               << ssh_get_error(session);
            }
        } else
            setErrorString(tr("Reject the host key"));
        break;
    case SSH_KNOWN_HOSTS_UNKNOWN:
        nRet = -6;
        szErr = tr("The server is unknown. Do you trust the host key?") + "\n";
        szErr += tr("Host key hash:") + "\n" + szHash;
        qDebug(log) << szErr;
        emit sigBlockShowMessageBox(tr("Error"), szErr,
                                    QMessageBox::Yes | QMessageBox::No,
                                    btRet, checkBox);
        if(QMessageBox::Yes == btRet) {
            nRet = ssh_session_update_known_hosts(session);
            if (SSH_OK != nRet) {
                qCritical(log) << "ssh_session_update_known_hosts fail."
                               << ssh_get_error(session);
            }
        } else
            setErrorString(tr("Reject the host key"));
        break;
    case SSH_KNOWN_HOSTS_ERROR:
        nRet = -7;
        szErr = tr("Error:") + ssh_get_error(session) + "\n";
        szErr += tr("Host key hash:") + "\n" + szHash + "\n";
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
        
        if(m_Parameter->GetUseSystemFile()) {
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
                m_Parameter->GetUser(),
                m_Parameter->GetPublicKeyFile(),
                m_Parameter->GetPrivateKeyFile(),
                m_Parameter->GetPassphrase());
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

    do{
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
                szErr += tr("The file doesn't exist or permission denied:");
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
                szErr += tr("The file doesn't exist or permission denied:");
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

    }while(0);

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
        m_Parameter->GetRemoteHost().toStdString().c_str(),
        m_Parameter->GetRemotePort(),
        m_Parameter->GetSourceHost().toStdString().c_str(),
        m_Parameter->GetSourcePort());
    if(SSH_OK != nRet) {
        ssh_channel_free(m_Channel);
        m_Channel = NULL;

        QString szErr;
        szErr = tr("SSH failed: open forward.") + ssh_get_error(session);
        szErr += "(" + m_Parameter->GetRemoteHost()
                + ":" + QString::number(m_Parameter->GetRemotePort()) + ")";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return nRet;
    }

    qInfo(log) << "Connected:"
               << m_Parameter->GetRemoteHost()
                      + ":" + QString::number(m_Parameter->GetRemotePort())
               << "with ssh turnnel:"
               << m_Parameter->GetServer()
                      + ":" + QString::number(m_Parameter->GetPort());

    emit sigConnected();
    
    //ssh_channel_set_blocking(m_Channel, 0);
    
    return nRet;
}

/*!
 * \return
 *   \li >= 0: continue, Interval call time (msec)
 *   \li = -1: stop
 *   \li < -1: error
 * \~
 */
int CChannelSSHTunnel::Process()
{
    int nRet = 0;
    
    if(!m_Channel || !ssh_channel_is_open(m_Channel)
        || ssh_channel_is_eof(m_Channel)) {
        qCritical(log) << "The channel is not open";
        return -1;
    }
    
    // Write data
    m_readMutex.lock();
    qint64 nLen = m_writeData.size();
    if(nLen > 0) {
        nRet = ssh_channel_write(m_Channel, m_writeData.data(), nLen);
        if(nRet > 0) {
            m_writeData.remove(0, nRet);
        }
    }
    m_readMutex.unlock();
    if(nRet < 0) {
        if(SSH_AGAIN != nRet) {
            QString szErr;
            szErr = "Write data from channel failed:";
            szErr += ssh_get_error(m_Session);
            qCritical(log) << szErr;
            return -2;
        }
    }
    
    struct timeval timeout = {0, 1000};
    ssh_channel channels[2];
    channels[0] = m_Channel;
    channels[1] = nullptr;
    nRet = ssh_channel_select(channels, NULL, NULL, &timeout);
    //qDebug(log) << "ssh_channel_select timeout" << nRet << QDateTime::currentDateTime();
    if(nRet < 0 && SSH_AGAIN != nRet) {
        QString szErr;
        szErr = "ssh_channel_select failed: " + QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        return nRet;
    }
    
    if(ssh_channel_is_eof(m_Channel)) {
        qDebug(log) << "Channel is eof";
        return -1;
    }

    if(!channels[0]){
        qDebug(log) << "There is not channel";
        return 0;
    }

    nRet = ssh_channel_poll(m_Channel, 0);
    //qDebug(log) << "ssh_channel_poll:" << nRet;
    if(nRet < 0) {
        QString szErr;
        szErr = "ssh_channel_poll failed:";
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        return -2;
    }
    if(nRet == 0) {
        //qDebug(log) << "There is not data in channel";
        return 0;
    }
    
    QSharedPointer<char> buf(new char[nRet]);
    nRet = ssh_channel_read_nonblocking(m_Channel, buf.get(), nRet, 0);
    if(nRet > 0) {
        //qDebug(log) << "Read data:" << nRet;
        m_readMutex.lock();
        m_readData.append(buf.get(), nRet);
        m_readMutex.unlock();
        emit this->readyRead();
    } else if(SSH_AGAIN != nRet){
        QString szErr;
        szErr = "Read data from channel failed:";
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        return nRet;
    }

    return 0;
}

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
                int nRet = 0;
                qDebug(log) << "Read socket" << fd;
                if(!m_Channel || !ssh_channel_is_open(m_Channel)
                    || ssh_channel_is_eof(m_Channel)) {
                    qCritical(log) << "The channel is not open";
                    return;
                }
                const int nLen = 1024;
                char buf[nLen];
                do {
                    int nRet = ssh_channel_read_nonblocking(m_Channel, buf, nLen, 0);
                    if(nRet < 0) {
                        QString szErr;
                        szErr = "Read data from channel failed:";
                        szErr += ssh_get_error(m_Session);
                        qCritical(log) << szErr;
                        emit sigError(nRet, szErr);
                        return;
                    }
                    m_readMutex.lock();
                    m_readData.append(buf, nLen);
                    m_readMutex.unlock();
                } while(nRet >= nLen);
                emit this->readyRead();
            });
        Q_ASSERT(check);
    }
    
    m_pSocketWrite = new QSocketNotifier(fd, QSocketNotifier::Write, this);
    if(m_pSocketWrite) {
        check = connect(
            m_pSocketWrite, &QSocketNotifier::activated,
            this, [&](int fd){
                int nRet = 0;
                qDebug(log) << "Write socket" << fd;
                return;
                if(!m_Channel || !ssh_channel_is_open(m_Channel)
                    || ssh_channel_is_eof(m_Channel)) {
                    qCritical(log) << "The channel is not open";
                    return;
                }
                m_readMutex.lock();
                qint64 nLen = m_writeData.size();
                if(nLen > 0) {
                    nRet = ssh_channel_write(m_Channel, m_writeData.data(), nLen);
                    if(nRet > 0) {
                        m_writeData.remove(0, nRet);
                    }
                }
                if(nRet == nLen) {
                    m_pSocketWrite->setEnabled(false);
                }
                m_readMutex.unlock();
                if(nRet < 0) {
                    if(SSH_AGAIN != nRet) {
                        QString szErr;
                        szErr = "Write data from channel failed:";
                        szErr += ssh_get_error(m_Session);
                        qCritical(log) << szErr;
                        emit sigError(nRet, szErr);
                    }
                }
            });
        Q_ASSERT(check);
    }
    
    m_pSocketException = new QSocketNotifier(fd, QSocketNotifier::Exception, this);
    if(m_pSocketException) {
        check = connect(
            m_pSocketException, &QSocketNotifier::activated,
            this, [&](int) {
                qDebug(log) << "Exception socket";
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