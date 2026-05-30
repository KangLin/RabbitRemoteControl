#include <QLoggingCategory>
#include "FrmViewServer.h"
#include "OperateServer.h"
#include "BackendServer.h"

static Q_LOGGING_CATEGORY(log, "Backend.Server")
CBackendServer::CBackendServer(COperateServer *pOperate, bool bStopSignal)
    : CBackend(pOperate, bStopSignal)
{
    qDebug(log) << Q_FUNC_INFO;
    SetConnect(pOperate);
}

CBackendServer::~CBackendServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

int CBackendServer::SetConnect(COperateServer* pOperate)
{
    int nRet = 0;
    Q_ASSERT(pOperate);
    auto *pView = qobject_cast<CFrmViewServer*>(pOperate->GetViewer());
    Q_ASSERT(pView);
    bool check = connect(this, SIGNAL(sigConnectCount(int,int,int)),
                         pView, SLOT(slotConnectCount(int,int,int)));
    Q_ASSERT(check);

    check = connect(this, SIGNAL(sigConnected(QString,quint16)),
                    pView, SLOT(slotConnected(QString,quint16)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnected(QString,quint16)),
                    pView, SLOT(slotDisconnected(QString,quint16)));
    Q_ASSERT(check);
    check = connect(pView, SIGNAL(sigDisconnect(QString,quint16)),
                    this, SLOT(slotDisconnect(QString,quint16)));
    Q_ASSERT(check);

    return nRet;
}
