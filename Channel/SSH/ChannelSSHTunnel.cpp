#include "ChannelSSHTunnel.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Channel.SSH.Tunnel")

CChannelSSHTunnel::CChannelSSHTunnel(
        QSharedPointer<CParameterSSH> parameter,
        QObject *parent)
    : CChannel(parent),
    m_Session(NULL),
    m_Channel(NULL),
    m_Parameter(parameter)
{
    Q_ASSERT(m_Parameter);
}

qint64 CChannelSSHTunnel::readData(char *data, qint64 maxlen)
{
    if(!m_Channel || !ssh_channel_is_open(m_Channel))
        return 0;
    return ssh_channel_read(m_Channel, data, maxlen, 0);
}

qint64 CChannelSSHTunnel::writeData(const char *data, qint64 len)
{
    if(!m_Channel || !ssh_channel_is_open(m_Channel))
        return 0;
    return ssh_channel_write(m_Channel, data, len);
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
        
        nRet = verifyKnownhost(m_Session, m_Parameter->GetPublicKeyHashType());
        if(nRet) break;
        //*
        nRet = authenticationPublicKey(
            m_Session,
            m_Parameter->GetUser(),
            m_Parameter->GetPublicKeyFile(),
            m_Parameter->GetPrivateKeyFile(),
            m_Parameter->GetPassphrase());
        if(nRet) break;//*/
        /*
        nRet = authenticationUser(m_Session,
                                  m_Parameter->GetUser(),
                                  m_Parameter->GetPassword(),
                                  m_Parameter->GetPassphrase(),
                                  m_Parameter->GetAuthenticationMethod());
        if(nRet) break;
        //*/
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
    if(m_Channel) {
        if(ssh_channel_is_open(m_Channel))
            ssh_channel_close(m_Channel);
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

int CChannelSSHTunnel::verifyKnownhost(ssh_session session,
                                       const ssh_publickey_hash_type type)
{
    int nRet = 0;
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
                                  type,
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

    enum ssh_known_hosts_e state = ssh_session_is_known_server(session);
    switch(state) {
    case SSH_KNOWN_HOSTS_OK:
        /* OK */
        
        break;
    case SSH_KNOWN_HOSTS_CHANGED:
        szErr = tr("Host key for server changed: it is now:\n");
        szErr += szHash;
        szErr += tr("For security reasons, connection will be stopped");
        nRet = -3;
        emit sigError(nRet, szErr);
        break;
    case SSH_KNOWN_HOSTS_OTHER:
        szErr = tr("The host key for this server was not found but an other type of key exists.\n");
        szErr += tr("An attacker might change the default server key to "
                  "confuse your client into thinking the key does not exist\n");
        szErr += tr("For security reasons, connection will be stopped");
        nRet = -4;
        emit sigError(nRet, szErr);
        break;
    case SSH_KNOWN_HOSTS_NOT_FOUND:
        szErr = tr("Could not find known host file.\n");
        szErr += tr("If you accept the host key here, the file will be "
                    "automatically created.\n");
        szErr += tr("Public key hash: ") + szHash;
        //TODO: write know host file if accept
        nRet = ssh_session_update_known_hosts(session);
        if(nRet)
        {
            qCritical(log) << "ssh_session_update_known_hosts fail."
                           << ssh_get_error(session);
        }
        break;
    case SSH_KNOWN_HOSTS_UNKNOWN:
        szErr = tr("The server is unknown. Do you trust the host key?\n");
        szErr += tr("Public key hash: ") + szHash;
        //TODO: write know host file if accept
        nRet = ssh_session_update_known_hosts(session);
        if (SSH_OK != nRet) {
            qCritical(log) << "ssh_session_update_known_hosts fail."
                           << ssh_get_error(session);
        }
        break;
    case SSH_KNOWN_HOSTS_ERROR:
        szErr = tr("Error") + ssh_get_error(session);
        nRet = -7;
        emit sigError(nRet, szErr);
        break;
    }

    return nRet;
}

int CChannelSSHTunnel::authenticationUser(
    ssh_session session,
    const QString szUser,
    const QString szPassword,
    const QString szPassphrase,
    const int nMethod)
{
    int nRet = 0;
    int nServerMethod = nMethod;
    qDebug(log) << "Authentication mothod:" << nMethod;
    //* Get authentication list from ssh server
    nRet = ssh_userauth_none(m_Session,
                             szUser.toStdString().c_str());
    qDebug(log) << "ssh_userauth_none:" << nRet;
    if(SSH_AUTH_SUCCESS == nRet)
        return 0;

    nServerMethod = ssh_userauth_list(m_Session,
                                      szUser.toStdString().c_str());
    qDebug(log) << "ssh_userauth_list:" << nServerMethod;
    //*/

    if(nServerMethod & nMethod & SSH_AUTH_METHOD_PUBLICKEY) {
        qDebug(log) << "User authentication: publickey";
        nRet = ssh_userauth_publickey_auto(session,
                                           szUser.toStdString().c_str(),
                                           szPassphrase.toStdString().c_str());
        if(SSH_AUTH_SUCCESS == nRet)
            return 0;
        QString szErr = tr("SSH failed: Error authenticating with publickey:")
                        + ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
    }

    if(nServerMethod & nMethod & SSH_AUTH_METHOD_PASSWORD) {
        qDebug(log) << "User authentication: password";
        nRet = ssh_userauth_password(m_Session,
                                     szUser.toStdString().c_str(),
                                     szPassword.toStdString().c_str());
        if(nRet) {
            QString szErr = tr("Error authenticating with password:")
                            + ssh_get_error(m_Session);
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
    
    m_Channel = ssh_channel_new(session);
    if(NULL == m_Channel) {
        qCritical(log) << "ssh_channel_new fail." << ssh_get_error(session);
        return -1;
    }

    nRet = ssh_channel_open_forward(m_Channel,
                                    m_Parameter->GetRemoteHost().toStdString().c_str(),
                                    m_Parameter->GetRemotePort(),
                                    m_Parameter->GetSourceHost().toStdString().c_str(),
                                    m_Parameter->GetSourcePort());
    if(SSH_OK != nRet) {
        ssh_channel_free(m_Channel);
        m_Channel = NULL;

        QString szErr;
        szErr = tr("SSH failed: ") + ssh_get_error(session);
        szErr += "(" + m_Parameter->GetRemoteHost()
                + ":" + QString::number(m_Parameter->GetRemotePort()) + ")";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return nRet;
    }

    qInfo(log) << "Connected:"
               << m_Parameter->GetRemoteHost() + ":" + m_Parameter->GetRemotePort()
               << "with ssh turnnel:"
               << m_Parameter->GetServer() + ":" + m_Parameter->GetPort();
    emit sigConnected();
    return nRet;
}