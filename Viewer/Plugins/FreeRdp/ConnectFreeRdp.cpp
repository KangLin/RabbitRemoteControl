#include "ConnectFreeRdp.h"

#include <QDebug>

CConnectFreeRdp::CConnectFreeRdp(CFrmViewer *pView, QObject *parent)
    : CConnect(pView, parent),
      m_pContext(nullptr),
      m_pSettings(nullptr)
{
    ZeroMemory(&m_ClientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	m_ClientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
	m_ClientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
    
    RdpClientEntry(&m_ClientEntryPoints);
    
    rdpContext* p = freerdp_client_context_new(&m_ClientEntryPoints);
	if(p)
    {
        m_pContext = (ClientContext*)p;
        m_pContext->pConnect = this;
    }
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
    int nRet = 0;
    
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
    
    return nRet;
}

void CConnectFreeRdp::slotClipBoardChange()
{
}

BOOL CConnectFreeRdp::Client_global_init()
{
	setlocale(LC_ALL, "");

	return TRUE;
}

void CConnectFreeRdp::Client_global_uninit()
{
}

BOOL CConnectFreeRdp::Client_new(freerdp *instance, rdpContext *context)
{
    return TRUE;
}

void CConnectFreeRdp::Client_free(freerdp *instance, rdpContext *context)
{
    
}

int CConnectFreeRdp::Client_start(rdpContext *context)
{
    int nRet = 0;
    
    return nRet;
}

int CConnectFreeRdp::Client_stop(rdpContext *context)
{
    int nRet = 0;
    
    return nRet;
}
