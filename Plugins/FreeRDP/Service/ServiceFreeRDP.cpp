// Author: Kang Lin <kl222@126.com>

#include "ServiceFreeRDP.h"
#include "ParameterServiceFreeRDP.h"
#ifdef HAVE_GUI
    #include "FrmParameterFreeRDP.h"
#endif

CServiceFreeRDP::CServiceFreeRDP(CPluginServiceFreeRDP *plugin)
    : CService(plugin),
      m_Log("FreeRDP"),
      m_pSettings(nullptr),
      m_pServer(nullptr),
      m_bServerInit(false)
{
    m_pPara = new CParameterServiceFreeRDP(this);
}

CServiceFreeRDP::~CServiceFreeRDP()
{
}

int CServiceFreeRDP::OnInit()
{
    int nRet = 0;
    shadow_subsystem_set_entry_builtin(NULL);

	m_pServer = shadow_server_new();
	if (!m_pServer)
	{
		qCritical(m_Log) << "Server new failed";
		return -1;
	}
    
    SetParameters();
    
    nRet = shadow_server_init(m_pServer);
    if(nRet < 0)
    {
        qCritical(m_Log) << "Server initialization failed.";
        return nRet;
    }
    m_bServerInit = true;
    
    if ((nRet = shadow_server_start(m_pServer)) < 0)
	{
		qCritical(m_Log) << "Failed to start server.";
		return nRet;
	}
     
    // Don't use OnProcess (qt event loop)
    // Because freerdp has new thread process loop
    return 1;
}

int CServiceFreeRDP::OnClean()
{
    int nRet = 0;

    if(m_pServer)
    {
        if(m_bServerInit)
            shadow_server_uninit(m_pServer);
        shadow_server_free(m_pServer);
        m_pServer = nullptr;
    }

    return nRet;
}

#ifdef HAVE_GUI
QWidget* CServiceFreeRDP::GetParameterWidget(QWidget *parent)
{
    return new CFrmParameterFreeRDP(
                dynamic_cast<CParameterServiceFreeRDP*>(GetParameters()), parent);
}
#endif

int CServiceFreeRDP::SetParameters()
{
    CParameterServiceFreeRDP* p = 
            dynamic_cast<CParameterServiceFreeRDP*>(GetParameters());

    m_pSettings = m_pServer->settings;
    m_pSettings->NlaSecurity = p->getNlaSecurity();
	m_pSettings->TlsSecurity = p->getTlsSecurity();
	m_pSettings->RdpSecurity = p->getRdpSecurity();
    m_pSettings->UseRdpSecurityLayer = m_pSettings->RdpSecurity;
    m_pSettings->ExtSecurity = p->getNlaExtSecurity();
    if(!p->getSamFile().isEmpty())
        freerdp_settings_set_string(m_pSettings, FreeRDP_NtlmSamFile,
                                    p->getSamFile().toStdString().c_str());
    
	m_pServer->authentication = p->getAuthentication();
     
    m_pServer->port = p->getPort();
    m_pServer->mayView = p->getMayView();
    m_pServer->mayInteract = p->getMayInteract();

    return 0;
}
