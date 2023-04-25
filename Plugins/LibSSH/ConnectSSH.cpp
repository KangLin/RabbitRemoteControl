#include "ConnectSSH.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ssh)

CConnectSSH::CConnectSSH(CConnecterSSH *pConnecter, QObject *parent)
    : CConnect(pConnecter, parent),
      m_pConnecter(pConnecter),
      m_pPara(nullptr),
      m_pSession(nullptr),
      m_pChannel(nullptr),
      m_pEvent(nullptr),
      connector_in(nullptr),
      connector_out(nullptr),
      connector_err(nullptr),
      m_pPcapFile(nullptr)
{
    m_pPara = dynamic_cast<CParameterSSH*>(pConnecter->m_pPara);
}

int CConnectSSH::SetParameter(void *pPara)
{
    int nRet = 0;

    Q_UNUSED(pPara);

    if(m_pPara->GetHost().isEmpty())
    {
        qCritical(ssh) << "The host isn't setting";
        emit sigError(-1, "The host isn't setting");
    }
    else if (ssh_options_set(m_pSession, SSH_OPTIONS_HOST,
                             m_pPara->GetHost().toStdString().c_str()) < 0) {
        qCritical(ssh) << "The host set is fail";
        emit sigError(-2, "The host set is fail");
        return -2;
    }

    if(m_pPara->GetUser().isEmpty())
    {
        qCritical(ssh) << "The user isn't setting";
        emit sigError(-3, "The user isn't setting");
    } else if (ssh_options_set(m_pSession, SSH_OPTIONS_USER,
                               m_pPara->GetUser().toStdString().c_str()) < 0) {
        qCritical(ssh) << "The user set fail";
        emit sigError(-3, "The user set fail");
        return -3;
    }

    if(!m_pPara->captrueFile.isEmpty())
    {
        m_pPcapFile = ssh_pcap_file_new();
        if (m_pPcapFile == NULL) {
            return -4;
        }
        
        if (ssh_pcap_file_open(m_pPcapFile,
                               m_pPara->captrueFile.toStdString().c_str())
                == SSH_ERROR) {
            qCritical(ssh) << "Error opening pcap file:" << m_pPara->captrueFile;
            ssh_pcap_file_free(m_pPcapFile);
            m_pPcapFile = NULL;
            return -5;
        }
        ssh_set_pcap_file(m_pSession, m_pPcapFile);
    }

    return nRet;
}

int CConnectSSH::Initialize()
{
    int nRet = 0;
    ssh_init();
    m_pSession = ssh_new();
    if(!m_pSession) return -1;

    m_pCb = new ssh_callbacks_struct();
    if(m_pCb)
    {
        m_pCb->userdata = this;
        m_pCb->auth_function = cbAuthCallback;
    }
    ssh_callbacks_init(m_pCb);
    nRet = ssh_set_callbacks(m_pSession, m_pCb);
    if(nRet) return nRet;

    nRet = SetParameter(m_pPara);

    return nRet;
}

int CConnectSSH::OnInit()
{
    int nRet = 0;

    nRet = Initialize();
    if(nRet) return nRet;
    
    CFrmTermWidget* pConsole = qobject_cast<CFrmTermWidget*>(m_pConnecter->GetViewer());
    if(!pConsole) return -4;

    //pConsole->startTerminalTeletype();
//    bool check = pConsole->connect(pConsole, SIGNAL(sendData(const char *,int)),
//                      this, SLOT(slotSendData(const char *,int)));
//    Q_ASSERT(check);
//    check = pConsole->connect(pConsole, SIGNAL(receivedData(const QString &)),
//                              this, SLOT(slotReceivedData(const QString &)));
//    Q_ASSERT(check);

    if (ssh_connect(m_pSession)) {
        qCritical(ssh) << "Connection failed:" << ssh_get_error(m_pSession);
        return -1;
    }

    int state = VerifyKnownhost(m_pSession);
    if (state != 0) {
        return -2;
    }

    ssh_userauth_none(m_pSession, NULL);
    char* banner = ssh_get_issue_banner(m_pSession);
    if (banner) {
        qCritical(ssh) << banner;
        free(banner);
    }

    int auth = Authenticate(m_pSession);
    if (auth != SSH_AUTH_SUCCESS) {
        return -1;
    }

    m_pChannel = ssh_channel_new(m_pSession);
    if (m_pChannel == NULL) {
        qCritical(ssh) << "Don't create channel:" << ssh_get_error(m_pSession);
        return -1;
    }

    if (ssh_channel_open_session(m_pChannel)) {
        qCritical(ssh) << "Error opening channel:" << ssh_get_error(m_pSession);
        return -1;
    }

    ssh_channel_request_pty(m_pChannel);
    ssh_channel_change_pty_size(m_pChannel, pConsole->screenColumnsCount(), pConsole->screenLinesCount());
    qDebug(ssh) << "row:" << pConsole->screenLinesCount()
                << ";col:" << pConsole->screenColumnsCount();

    if (ssh_channel_request_shell(m_pChannel)) {
        qCritical(ssh) << "Requesting shell:" << ssh_get_error(m_pSession);
        return -1;
    }

    m_pEvent = ssh_event_new();

    /* stdin */
    connector_in = ssh_connector_new(m_pSession);
    if(connector_in && m_pChannel && m_pEvent)
    {
        nRet = ssh_connector_set_out_channel(connector_in, m_pChannel, SSH_CONNECTOR_STDOUT);
        if(nRet) return -2;
        ssh_connector_set_in_fd(connector_in, pConsole->getPtySlaveFd());
        nRet = ssh_event_add_connector(m_pEvent, connector_in);
        if(nRet) return -3;
    }

    /* stdout */
    connector_out = ssh_connector_new(m_pSession);
    if(connector_out && m_pChannel && m_pEvent)
    {
#ifndef SSH_CONNECTOR_STDINOUT
#define SSH_CONNECTOR_STDINOUT SSH_CONNECTOR_STDOUT
#endif
        nRet = ssh_connector_set_in_channel(connector_out, m_pChannel, SSH_CONNECTOR_STDINOUT);
        if(nRet) return -4;
       ssh_connector_set_out_fd(connector_out, pConsole->getPtySlaveFd());
        nRet = ssh_event_add_connector(m_pEvent, connector_out);
        if(nRet) return -5;
    }

    /* stderr */
    connector_err = ssh_connector_new(m_pSession);
    if(connector_err && m_pChannel && m_pEvent)
    {
        ssh_connector_set_out_fd(connector_err, 2);
        nRet = ssh_connector_set_in_channel(connector_err, m_pChannel, SSH_CONNECTOR_STDERR);
        if(nRet) return -6;
        nRet = ssh_event_add_connector(m_pEvent, connector_err);
        if(nRet) return -7;
    }

    return nRet;
}

int CConnectSSH::OnClean()
{
    int nRet = 0;

    if(m_pEvent)
    {
        if(connector_in)
        {
            ssh_event_remove_connector(m_pEvent, connector_in);
            ssh_connector_free(connector_in);
        }
        if(connector_out)
        {
            ssh_event_remove_connector(m_pEvent, connector_out);
            ssh_connector_free(connector_out);
        }
        if(connector_err)
        {
            ssh_event_remove_connector(m_pEvent, connector_err);
            ssh_connector_free(connector_err);
        }
        
        ssh_event_free(m_pEvent);
    }

    if(m_pChannel)
        ssh_channel_free(m_pChannel);

    if(m_pSession)
    {
        ssh_disconnect(m_pSession);
        ssh_free(m_pSession);
        ssh_finalize();
    }
    
    if(m_pPcapFile)
    {
        ssh_pcap_file_free(m_pPcapFile);
        m_pPcapFile = nullptr;
    }
    
    return nRet;
}

int CConnectSSH::OnProcess()
{
    int nRet = -1;
    
    if (ssh_channel_is_open(m_pChannel)) {
        nRet = ssh_event_dopoll(m_pEvent, 500);
        if (nRet == SSH_ERROR) {
            qCritical(ssh) << "Error in ssh_event_dopoll()";
            nRet = -1;
        } else if(nRet == SSH_EOF)
            nRet = 1;
        else if(SSH_AGAIN == nRet || SSH_OK == nRet)
            nRet = 0;
    } else
        qCritical(ssh) << "ssh channel isn't open";

    return nRet;
}

void CConnectSSH::slotClipBoardChanged()
{
}

int CConnectSSH::cbAuthCallback(const char *prompt,
                         char *buf,
                         size_t len,
                         int echo,
                         int verify,
                         void *userdata)
{
    CConnectSSH* pThis = (CConnectSSH*)userdata;
    qCritical(ssh) << "prompt:" << prompt << ";echo:" << echo << ";verify:" << verify;
    return pThis->GetPassword(prompt, buf, len, echo, verify);
}

int CConnectSSH::GetPassword(const char *prompt,
                             char *buf,
                             size_t len,
                             int echo,
                             int verify)
{
    Q_UNUSED(prompt);
    Q_UNUSED(echo);
    
    int nRet = 0;
    int length = len;
    if(len > m_pPara->GetPassword().toStdString().length())
        length = m_pPara->GetPassword().toStdString().length();
    if(1 == verify)
        memcpy(buf, m_pPara->GetPassword().toStdString().c_str(), length);

    return nRet;
}

int CConnectSSH::VerifyKnownhost(ssh_session session)
{
    /*TODO:
    enum ssh_known_hosts_e state = SSH_KNOWN_HOSTS_UNKNOWN;
    char buf[10];
    unsigned char *hash = NULL;
    size_t hlen = 0;
    ssh_key srv_pubkey = nullptr;
    int rc = 0;
    QString szFinger;
    QString szErr;
    
    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0) {
        qCritical(ssh) << "ssh_get_server_publickey fail";
        return -1;
    }

    rc = ssh_get_publickey_hash(srv_pubkey,
                                SSH_PUBLICKEY_HASH_SHA256,
                                &hash,
                                &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) {
        qCritical(ssh) << "ssh_get_publickey_hash fail";
        return -1;
    }

    char *fingerprint = NULL;
    fingerprint = ssh_get_fingerprint_hash(SSH_PUBLICKEY_HASH_SHA256,
                                           hash,
                                           hlen);
    ssh_clean_pubkey_hash(&hash);
    if (fingerprint) {
        szFinger = fingerprint;
        if(fingerprint) free(fingerprint);
    }

    state = ssh_session_is_known_server(session);

    switch(state) {
    case SSH_KNOWN_HOSTS_CHANGED:
        szErr = tr("Host key for server changed : server's one is now :\n");
        szErr += szFinger + "\n";
        szErr += tr("For security reason, connection will be stopped\n");
        qCritical(ssh) << szErr;
        return -1;
    case SSH_KNOWN_HOSTS_OTHER:
        szErr = tr("The host key for this server was not found but an other type of key exists.\n");
        szErr += tr("An attacker might change the default server key to confuse your client"
                "into thinking the key does not exist\n"
                "We advise you to rerun the client with -d or -r for more safety.\n");
        qCritical(ssh) << szErr;
        return -1;
    case SSH_KNOWN_HOSTS_NOT_FOUND:
        szErr = tr("Could not find known host file. If you accept the host key here,\n");
        szErr += tr("the file will be automatically created.\n");
        qCritical(ssh) << szErr;
    case SSH_SERVER_NOT_KNOWN:
        szErr = tr("The server is unknown. This new key will be written on disk for further usage.\n");
        szErr += szFinger;
        qCritical(ssh) << szErr;

        rc = ssh_session_update_known_hosts(session);
        if (rc != SSH_OK) {
            qCritical(ssh) << "error:" << strerror(errno);
            return -1;
        }
        
//        ssh_print_hash(SSH_PUBLICKEY_HASH_SHA256, hash, hlen);

//        if (fgets(buf, sizeof(buf), stdin) == NULL) {
//            ssh_clean_pubkey_hash(&hash);
//            return -1;
//        }
//        if(strncasecmp(buf,"yes",3)!=0){
//            ssh_clean_pubkey_hash(&hash);
//            return -1;
//        }
//        fprintf(stderr,"This new key will be written on disk for further usage. do you agree ?\n");
//        if (fgets(buf, sizeof(buf), stdin) == NULL) {
//            ssh_clean_pubkey_hash(&hash);
//            return -1;
//        }
//        if(strncasecmp(buf,"yes",3)==0){
//            rc = ssh_session_update_known_hosts(session);
//            if (rc != SSH_OK) {
//                ssh_clean_pubkey_hash(&hash);
//                fprintf(stderr, "error %s\n", strerror(errno));
//                return -1;
//            }
//        }

        break;
    case SSH_KNOWN_HOSTS_ERROR:
        qCritical(ssh) << "Known host error:" << ssh_get_error(session);
        return -1;
    case SSH_KNOWN_HOSTS_OK:
        break; // ok 
    }
    //*/
    return 0;
}

void CConnectSSH::error(ssh_session session)
{
    qCritical(ssh) << "Authentication failed:" << ssh_get_error(session);
}

int CConnectSSH::Authenticate(ssh_session session)
{
    int rc = SSH_AUTH_SUCCESS;
    int method = SSH_AUTH_METHOD_UNKNOWN;
    char password[128] = {0};
    char *banner = nullptr;

    // Try to authenticate
    rc = ssh_userauth_none(session, NULL);
    if (rc == SSH_AUTH_ERROR) {
        error(session);
        return rc;
    }

    method = ssh_userauth_list(session, NULL);
    while (rc != SSH_AUTH_SUCCESS) {
        if (method & SSH_AUTH_METHOD_GSSAPI_MIC){
            rc = ssh_userauth_gssapi(session);
            if(rc == SSH_AUTH_ERROR) {
                error(session);
                return rc;
            } else if (rc == SSH_AUTH_SUCCESS) {
                break;
            }
        }
        // Try to authenticate with public key first
        if (method & SSH_AUTH_METHOD_PUBLICKEY) {
            rc = ssh_userauth_publickey_auto(session, NULL, NULL);
            if (rc == SSH_AUTH_ERROR) {
                error(session);
                return rc;
            } else if (rc == SSH_AUTH_SUCCESS) {
                break;
            }
       
        /*TODO: add pubkey from file
       
            char buffer[128] = {0};
            char *p = NULL;

            printf("Automatic pubkey failed. "
                   "Do you want to try a specific key? (y/n)\n");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                break;
            }
            if ((buffer[0]=='Y') || (buffer[0]=='y')) {
                printf("private key filename: ");

                if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                    return SSH_AUTH_ERROR;
                }

                buffer[sizeof(buffer) - 1] = '\0';
                if ((p = strchr(buffer, '\n'))) {
                    *p = '\0';
                }

                //rc = auth_keyfile(session, buffer);

                if(rc == SSH_AUTH_SUCCESS) {
                    break;
                }
                fprintf(stderr, "failed with key\n");
            }//*/
        }

        // Try to authenticate with keyboard interactive";
        if (method & SSH_AUTH_METHOD_INTERACTIVE) {
            //rc = authenticate_kbdint(session, NULL);
            if (rc == SSH_AUTH_ERROR) {
                error(session);
                return rc;
            } else if (rc == SSH_AUTH_SUCCESS) {
                break;
            }
        }

        // Try to authenticate with password
        if (method & SSH_AUTH_METHOD_PASSWORD) {
            memcpy(password, m_pPara->GetPassword().toStdString().c_str(),
                   m_pPara->GetPassword().toStdString().length());
            rc = ssh_userauth_password(session, NULL, password);
            if (rc == SSH_AUTH_ERROR) {
                error(session);
                return rc;
            } else if (rc == SSH_AUTH_SUCCESS) {
                break;
            }
        }
        memset(password, 0, sizeof(password));
    }

    banner = ssh_get_issue_banner(session);
    if (banner) {
        qInfo(ssh) << banner;
        /* TODO:
        SSH_STRING_FREE_CHAR(banner); //*/
    }

    return rc;
}

void CConnectSSH::slotSendData(const char *buf,int len)
{
    CFrmTermWidget* pConsole = qobject_cast<CFrmTermWidget*>(m_pConnecter->GetViewer());
    if(!pConsole) return;
    
    for(int i = 0; i < len; i++)
        qDebug() << buf[i];
    
    write(pConsole->getPtySlaveFd(), buf, len);
}

void CConnectSSH::slotReceivedData(const QString &text)
{
    qDebug(ssh) << "Receive data:" << text;
}
