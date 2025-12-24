// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Backend.h"
#include "ftpserver.h"
#include "ParameterFtpServer.h"

class CBackendFtpServer : public CBackend, CFtpServerFilter
{
    Q_OBJECT

public:
    explicit CBackendFtpServer(COperate *pOperate = nullptr);
    ~CBackendFtpServer() override;
    
    // CFtpServerFilter interface
    virtual bool onFilter(QSslSocket *socket) override;
public Q_SLOTS:
    void slotDisconnect(const QString& szIp, const quint16 port);
Q_SIGNALS:
    void sigConnectCount(int nTotal, int nConnect, int nDisconnect);
    void sigConnected(const QString& szIp, const quint16 port);
    void sigDisconnected(const QString& szIp, const quint16 port);

protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;

private Q_SLOTS:
    void slotDisconnected();

private:
    FtpServer* m_pServer;
    QSharedPointer<CParameterFtpServer> m_Para;
    QList<QSslSocket*> m_Sockets;
    int m_nTotal;
    int m_nDisconnect;
};
