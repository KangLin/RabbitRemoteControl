// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTLAYERSSHTUNNEL_H
#define CONNECTLAYERSSHTUNNEL_H

#pragma once
#include "ConnectLayer.h"

class ConnectLayerSSHTunnel : public CConnectLayer
{
    Q_OBJECT
public:
    explicit ConnectLayerSSHTunnel(CConnectFreeRDP *connect = nullptr);
    virtual ~ConnectLayerSSHTunnel();

private:
    virtual int OnInit(rdpContext* context) override;
    virtual int OnClean() override;
    virtual int OnLayerRead(void* data, int bytes) override;
    virtual int OnLayerWrite(const void* data, int bytes) override;
    virtual BOOL OnLayerWait(BOOL waitWrite, DWORD timeout) override;
    virtual HANDLE OnLayerGetEvent() override;

private:
    CChannelSSHTunnel* m_pChannelSSH;
    HANDLE m_hSshSocket;
};

#endif // CONNECTLAYERSSHTUNNEL_H
