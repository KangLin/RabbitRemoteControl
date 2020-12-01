#include "ConnectFreeRdp.h"

#include "log4cplus/loggingmacros.h"

#include <QDebug>

CConnectFreeRdp::CConnectFreeRdp(CFrmViewer *pView, QObject *parent)
    : CConnect(pView, parent),
      m_pContext(nullptr),
      m_pSettings(nullptr)
{
    logger = log4cplus::Logger::getInstance("FreeRdp");
    
    ZeroMemory(&m_ClientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	m_ClientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
	m_ClientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
    
    RdpClientEntry(&m_ClientEntryPoints);
    
    rdpContext* p = freerdp_client_context_new(&m_ClientEntryPoints);
	if(p)
    {
        m_pContext = (ClientContext*)p;
        
        m_pContext->pConnect = this;
        m_pSettings = m_pContext->Context.settings;
    } else 
        LOG4CPLUS_ERROR(logger, "freerdp_client_context_new fail");
    
    //TODO:
    this->SetServerName("fmpixel.f3322.net");
}

CConnectFreeRdp::~CConnectFreeRdp()
{
    qDebug() << "CConnectFreeRdp::~CConnectFreeRdp()";
    if(m_pContext)
    {
        freerdp_client_context_free((rdpContext*)m_pContext);
        m_pContext = nullptr;
    }
}

int CConnectFreeRdp::RdpClientEntry(RDP_CLIENT_ENTRY_POINTS* pEntryPoints)
{
	pEntryPoints->Version = 1;
	pEntryPoints->Size = sizeof(RDP_CLIENT_ENTRY_POINTS_V1);
	pEntryPoints->GlobalInit = Client_global_init;
	pEntryPoints->GlobalUninit = Client_global_uninit;
	pEntryPoints->ContextSize = sizeof(ClientContext);
	pEntryPoints->ClientNew = Client_new;
	pEntryPoints->ClientFree = Client_free;
	pEntryPoints->ClientStart = Client_start;
	pEntryPoints->ClientStop = Client_stop;
	return 0;
}

int CConnectFreeRdp::Connect()
{
    qDebug() << "CConnectFreeRdp::Connect()";
    int nRet = 0;
    freerdp* instance = m_pContext->Context.instance;
    Q_ASSERT(m_pSettings);
    if(nullptr == m_pSettings) return -1;

    if(!m_szHost.isEmpty())
        m_pSettings->ServerHostname = _strdup(m_szHost.toStdString().c_str());
    if(m_nPort)
        m_pSettings->ServerPort = static_cast<UINT32>(m_nPort);
    if(!m_szUser.isEmpty())
        m_pSettings->Username = _strdup(m_szUser.toStdString().c_str());
    if(!m_szPassword.isEmpty())
        m_pSettings->Password = _strdup(m_szPassword.toStdString().c_str());
    
    freerdp_client_start(&m_pContext->Context);
    
    BOOL status = freerdp_connect(instance);
    if (!status)
	{
		if (freerdp_get_last_error(instance->context) == FREERDP_ERROR_AUTHENTICATION_FAILED)
			nRet = -2;
		else if (freerdp_get_last_error(instance->context) ==
		         FREERDP_ERROR_SECURITY_NEGO_CONNECT_FAILED)
			nRet = -3;
		else
			nRet = -4;
        
        LOG4CPLUS_ERROR(logger, "freerdp_connect fail:" << nRet);
	}
    
    return nRet;
}

int CConnectFreeRdp::Disconnect()
{
    int nRet = 0;
    
    return nRet;
}

int CConnectFreeRdp::Process()
{
    int nRet = 0;
    
    
    
    freerdp_client_stop(&m_pContext->Context);
	freerdp_client_context_free(&m_pContext->Context);
    return nRet;
}

void CConnectFreeRdp::slotClipBoardChange()
{
}

BOOL CConnectFreeRdp::Client_global_init()
{
	qDebug() << "CConnectFreeRdp::Client_global_init()";

	return TRUE;
}

void CConnectFreeRdp::Client_global_uninit()
{
    qDebug() << "CConnectFreeRdp::Client_global_uninit()";
}

BOOL CConnectFreeRdp::Client_new(freerdp *instance, rdpContext *context)
{
    qDebug() << "CConnectFreeRdp::Client_new()";
    return TRUE;
}

void CConnectFreeRdp::Client_free(freerdp *instance, rdpContext *context)
{
    qDebug() << "CConnectFreeRdp::Client_free()";
}

int CConnectFreeRdp::Client_start(rdpContext *context)
{
    int nRet = 0;
    qDebug() << "CConnectFreeRdp::Client_start()";
    return nRet;
}

int CConnectFreeRdp::Client_stop(rdpContext *context)
{
    int nRet = 0;
    qDebug() << "CConnectFreeRdp::Client_stop()";
    return nRet;
}
