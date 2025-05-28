// Author: Kang Lin <kl222@126.com>

#include "ConnectLayer.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "FreeRDP.Connect.Layer")

CConnectLayer::CConnectLayer(CConnectFreeRDP *connect)
    : QObject{connect}
    , m_pConnect(connect)
{
    Q_ASSERT(connect);
    if(connect)
        m_pParameter = connect->m_pParameter;
}

CConnectLayer::~CConnectLayer()
{
    qDebug(log) << Q_FUNC_INFO;
}

int CConnectLayer::Initialize(rdpContext* context)
{
    bool bRet = false;
    // See: - https://github.com/FreeRDP/FreeRDP/issues/8665
    //      - https://github.com/KangLin/Documents/blob/master/net/FreeRDP/freerdp_transport.drawio
    auto cb = freerdp_get_io_callbacks(context);
    if(!cb) return -1;
    rdpTransportIo io = *cb;
    io.ConnectLayer = cb_transport_connect_layer;
    bRet = freerdp_set_io_callbacks(context, &io);
    if(!bRet) {
        qCritical(log) << "Set freerdp io callback fail";
        return -2;
    }
    bRet = freerdp_set_io_callback_context(context, this);
    if(!bRet) {
        qCritical(log) << "Set freerdp io callback context fail";
        return -3;
    }
    return OnInit(context);
}

int CConnectLayer::Clean()
{
    int nRet = 0;
    nRet = OnClean();
    return nRet;
}

rdpTransportLayer* CConnectLayer::cb_transport_connect_layer(
    rdpTransport* transport,
    const char* hostname, int port,
    DWORD timeout)
{
    rdpContext* context = transport_get_context(transport);
    Q_ASSERT(context);
    Q_UNUSED(hostname);
    Q_UNUSED(port);
    Q_UNUSED(timeout);
    CConnectLayer* pThis = (CConnectLayer*)freerdp_get_io_callback_context(context);
    if(!pThis) return nullptr;
    return pThis->OnTransportConnectLayer(context);
}

rdpTransportLayer* CConnectLayer::OnTransportConnectLayer(rdpContext* context)
{
    //qDebug(log) << Q_FUNC_INFO;
    rdpTransportLayer* layer = nullptr;
    layer = transport_layer_new(freerdp_get_transport(context), sizeof(LayerUserData));
    if (!layer)
        return nullptr;
    LayerUserData* userData = (LayerUserData*)layer->userContext;
    userData->pThis = this;
    layer->Read = cbLayerRead;
    layer->Write = cbLayerWrite;
    layer->Close = cbLayerClose;
    layer->Wait = cbLayerWait;
    layer->GetEvent = cbLayerGetEvent;
    return layer;
}

int CConnectLayer::cbLayerRead(void *userContext, void *data, int bytes)
{
    //qDebug(log) << Q_FUNC_INFO;
    if(!userContext) return -1;
    if (!data || !bytes)
        return 0;
    CConnectLayer* pThis = ((LayerUserData*)userContext)->pThis;
    return pThis->OnLayerRead(data, bytes);
}

int CConnectLayer::cbLayerWrite(void *userContext, const void *data, int bytes)
{
    if(!userContext) return -1;
    if (!data || !bytes)
        return 0;
    CConnectLayer* pThis = ((LayerUserData*)userContext)->pThis;
    return pThis->OnLayerWrite(data, bytes);
}

BOOL CConnectLayer::cbLayerClose(void *userContext)
{
    //qDebug(log) << Q_FUNC_INFO;
    if(!userContext) return FALSE;
    CConnectLayer* pThis = ((LayerUserData*)userContext)->pThis;
    return pThis->OnLayerClose();
}

BOOL CConnectLayer::OnLayerClose()
{
    int nRet = Clean();
    if(nRet) return false;
    return true;
}

BOOL CConnectLayer::cbLayerWait(void *userContext, BOOL waitWrite, DWORD timeout)
{
    //qDebug(log) << Q_FUNC_INFO;
    CConnectLayer* pThis = ((LayerUserData*)userContext)->pThis;
    return pThis->OnLayerWait(waitWrite, timeout);
}

HANDLE CConnectLayer::cbLayerGetEvent(void *userContext)
{
    //qDebug(log) << Q_FUNC_INFO;
    CConnectLayer* pThis = ((LayerUserData*)userContext)->pThis;
    return pThis->OnLayerGetEvent();
}
