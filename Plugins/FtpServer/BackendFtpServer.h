// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ftpserver.h"
#include "OperateFtpServer.h"
#include "BackendServer.h"

class COperateFtpServer;
class CBackendFtpServer : public CBackendServer, CFtpServerFilter
{
    Q_OBJECT
public:
    explicit CBackendFtpServer(COperateFtpServer *pOperate = nullptr, bool bStopSignal = true);
    virtual ~CBackendFtpServer();

    // CFtpServerFilter interface
    virtual bool onFilter(QSslSocket *socket) override;

public Q_SLOTS:
    void slotDisconnect(const QString& szIp, const quint16 port) override;

    // CBackend interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;

private Q_SLOTS:
    void slotDisconnected();

private:
    COperateFtpServer* m_pOperate;
    CParameterFtpServer* m_pPara;
    CFtpServer* m_pServer;
    QList<QSslSocket*> m_Sockets;
};
