// Author: Kang Lin <kl222@126.com>

#pragma once

#include <freerdp/server/shadow.h>

#include "OperateFreeRDPServer.h"
#include "BackendServer.h"

class CBackendFreeRDPServer : public CBackendServer
{
    Q_OBJECT
public:
    explicit CBackendFreeRDPServer(
        COperateFreeRDPServer *pOperate = nullptr, bool bStopSignal = true);
    virtual ~CBackendFreeRDPServer();

public Q_SLOTS:
    void slotDisconnect(const QString& szIp, const quint16 port) override;

    // CBackend interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;

private:
    CParameterFreeRDPServer* m_pPara;
    rdpSettings* m_pSettings;
    rdpShadowServer* m_pServer;
    bool m_bServerInit;

    int SetParameters();
};
