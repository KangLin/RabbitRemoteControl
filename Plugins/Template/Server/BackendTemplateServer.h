// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Backend.h"
#include "OperateTemplateServer.h"

class CBackendTemplateServer : public CBackend
{
    Q_OBJECT
public:
    explicit CBackendTemplateServer(COperateTemplateServer *pOperate = nullptr, bool bStopSignal = true);
    virtual ~CBackendTemplateServer();

public Q_SLOTS:
    void slotDisconnect(const QString& szIp, const quint16 port);
Q_SIGNALS:
    void sigConnectCount(int nTotal, int nConnect, int nDisconnect);
    void sigConnected(const QString& szIp, const quint16 port);
    void sigDisconnected(const QString& szIp, const quint16 port);

    // CBackend interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
};
