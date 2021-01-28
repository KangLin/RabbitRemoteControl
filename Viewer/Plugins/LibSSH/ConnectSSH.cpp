#include "ConnectSSH.h"
#include "RabbitCommonLog.h"

CConnectSSH::CConnectSSH(CConnecterSSH *pConnecter, QObject *parent)
    : CConnect(pConnecter, parent),
      m_pConnecter(pConnecter),
      m_pSession(nullptr),
      m_pChannel(nullptr),
      m_pEvent(nullptr),
      connector_in(nullptr),
      connector_out(nullptr),
      connector_err(nullptr)
{
    m_pPara = pConnecter->m_pPara;
}

int CConnectSSH::SetParamter(void *pPara)
{
    int nRet = 0;
    
    Q_UNUSED(pPara);
    
    if(m_pPara->szHost.isEmpty())
    {
        LOG_MODEL_ERROR("ConnectSSH", "The host isn't setting");
        emit sigError(-1, "The host isn't setting");
    }
    else if (ssh_options_set(m_pSession, SSH_OPTIONS_HOST,
                             m_pPara->szHost.toStdString().c_str()) < 0) {
        LOG_MODEL_ERROR("ConnectSSH", "The host set is fail");
        emit sigError(-2, "The host set is fail");
        return -2;
    }
    
    if(m_pPara->szUser.isEmpty())
    {
        LOG_MODEL_ERROR("ConnectSSH", "The user isn't setting");
        emit sigError(-3, "The user isn't setting");
    } else if (ssh_options_set(m_pSession, SSH_OPTIONS_USER,
                               m_pPara->szUser.toStdString().c_str()) < 0) {
        LOG_MODEL_ERROR("ConnectSSH", "The user set fail");
        emit sigError(-3, "The user set fail");
        return -3;
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
    
    nRet = SetParamter(m_pPara);

    return nRet;
}

int CConnectSSH::Clean()
{
    int nRet = 0;
    if(m_pSession)
    {
        ssh_disconnect(m_pSession);
        ssh_free(m_pSession);
        ssh_finalize();
    }
    return nRet;
}

int CConnectSSH::Connect()
{
    int nRet = 0;
    if (ssh_connect(m_pSession)) {
        LOG_MODEL_ERROR("ConnectSSH", "Connection failed : %s\n", ssh_get_error(m_pSession));
        return -1;
    }

    /*
    int state = verify_knownhost(m_pSession);
    if (state != 0) {
        return -1;
    }
    //*/
    
    nRet = ssh_userauth_none(m_pSession, NULL);
    if(nRet)
    {
        LOG_MODEL_ERROR("ConnectSSH", "User auth fail: %d", nRet);
        return -2;
    }
    char* banner = ssh_get_issue_banner(m_pSession);
    if (banner) {
        LOG_MODEL_ERROR("ConnectSSH", "%s\n", banner);
        free(banner);
    }
    
//    int auth = authenticate_console(m_pSession);
//    if (auth != SSH_AUTH_SUCCESS) {
//        return -1;
//    }
    
    m_pChannel = ssh_channel_new(m_pSession);
    if (m_pChannel == NULL) {
        LOG_MODEL_ERROR("ConnectSSH", "Don't create channel");
        return -1;
    }
    
    m_pEvent = ssh_event_new();
    
    /* stdin */
    connector_in = ssh_connector_new(m_pSession);
    if(connector_in && m_pChannel && m_pEvent)
    {
        nRet = ssh_connector_set_out_channel(connector_in, m_pChannel, SSH_CONNECTOR_STDINOUT);
        if(nRet) return -2;
        CFrmTermWidget* pView = qobject_cast<CFrmTermWidget*>(m_pConnecter->GetViewer());
        if(!pView) return -4;
        ssh_connector_set_in_fd(connector_in, pView->getPtySlaveFd());
        nRet = ssh_event_add_connector(m_pEvent, connector_in);
        if(nRet) return -3;
    }

    /* stdout */
    connector_out = ssh_connector_new(m_pSession);
    if(connector_out && m_pChannel && m_pEvent)
    {
        CFrmTermWidget* pView = qobject_cast<CFrmTermWidget*>(m_pConnecter->GetViewer());
        if(!pView) return -4;
        ssh_connector_set_out_fd(connector_out, pView->getPtySlaveFd());
        nRet = ssh_connector_set_in_channel(connector_out, m_pChannel, SSH_CONNECTOR_STDINOUT);
        if(nRet) return -4;
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

int CConnectSSH::Disconnect()
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
    
    return nRet;
}

int CConnectSSH::Process()
{
    int nRet = -1;
    
    if (ssh_channel_is_open(m_pChannel)) {
//        if (signal_delayed) {
//            sizechanged();
//        }
        nRet = ssh_event_dopoll(m_pEvent, 500);
        if (nRet == SSH_ERROR) {
            LOG_MODEL_ERROR("ConnectSSH", "Error in ssh_event_dopoll()\n");
            nRet = -1;
        } else if(nRet == SSH_EOF)
            nRet = 1;
        else if(SSH_AGAIN == nRet || SSH_OK == nRet)
            nRet = 0;
    } else
        LOG_MODEL_ERROR("ConnectSSH", "ssh channel isn't open");

    return nRet;
}

void CConnectSSH::slotClipBoardChange()
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
    LOG_MODEL_DEBUG("ConnectSSH", "prompt:%s\n; echo: %d; verify: %d", prompt, echo, verify);
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
    if(len > m_pPara->szPassword.toStdString().length())
        length = m_pPara->szPassword.toStdString().length();
    if(1 == verify)
        memcpy(buf, m_pPara->szPassword.toStdString().c_str(), length);

    return nRet;
}
