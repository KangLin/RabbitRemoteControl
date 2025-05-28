// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTLAYER_H
#define CONNECTLAYER_H

#pragma once

#include <QObject>

#include "ConnectFreeRDP.h"

/*!
 * \brief The connect layer class
 * \see https://github.com/FreeRDP/FreeRDP/issues/8665
 * \see https://github.com/KangLin/Documents/blob/master/net/FreeRDP/freerdp_transport.drawio
 */
class CConnectLayer : public QObject
{
    Q_OBJECT
public:
    explicit CConnectLayer(CConnectFreeRDP* connect);
    virtual ~CConnectLayer();

    int Initialize(rdpContext* context);
    int Clean();
    
private:
    virtual int OnInit(rdpContext* context) = 0;
    virtual int OnClean() = 0;
    virtual int OnLayerRead(void* data, int bytes) = 0;
    virtual int OnLayerWrite(const void* data, int bytes) = 0;
    virtual BOOL OnLayerWait(BOOL waitWrite, DWORD timeout) = 0;
    virtual HANDLE OnLayerGetEvent() = 0;

    static rdpTransportLayer* cb_transport_connect_layer(
        rdpTransport* transport,
        const char* hostname, int port, DWORD timeout);
    virtual rdpTransportLayer* OnTransportConnectLayer(rdpContext *context);
    static int cbLayerRead(void* userContext, void* data, int bytes);
    static int cbLayerWrite(void* userContext, const void* data, int bytes);
    static BOOL cbLayerClose(void* userContext);
    virtual BOOL OnLayerClose();
    static BOOL cbLayerWait(void* userContext, BOOL waitWrite, DWORD timeout);
    static HANDLE cbLayerGetEvent(void* userContext);

protected:
    CConnectFreeRDP* m_pConnect;
    CParameterFreeRDP* m_pParameter;

private:
    struct LayerUserData
    {
        CConnectLayer* pThis;
    };
};

#endif // CONNECTLAYER_H
