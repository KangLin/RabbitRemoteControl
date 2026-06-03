// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Backend.h"

class COperateServer;
class PLUGIN_EXPORT CBackendServer : public CBackend
{
    Q_OBJECT
public:
    explicit CBackendServer(COperateServer *pOperate = nullptr, bool bStopSignal = true);
    virtual ~CBackendServer();

public Q_SLOTS:
    virtual void slotDisconnect(const QString& szIp, const quint16 port) = 0;

Q_SIGNALS:
    void sigConnected(const QString& szIp, const quint16 port);
    void sigDisconnected(const QString& szIp, const quint16 port);

private:
    int SetConnect(COperateServer* pOperate);
};
