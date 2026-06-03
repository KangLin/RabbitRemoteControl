// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateTemplateServer.h"
#include "BackendServer.h"

class CBackendTemplateServer : public CBackendServer
{
    Q_OBJECT
public:
    explicit CBackendTemplateServer(
        COperateTemplateServer *pOperate = nullptr, bool bStopSignal = true);
    virtual ~CBackendTemplateServer();

public Q_SLOTS:
    void slotDisconnect(const QString& szIp, const quint16 port) override;

    // CBackend interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;
};
