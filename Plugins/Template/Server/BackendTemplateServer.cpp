#include <QLoggingCategory>
#include "BackendTemplateServer.h"
#include "Backend.h"

static Q_LOGGING_CATEGORY(log, "Backend.TemplateServer")
CBackendTemplateServer::CBackendTemplateServer(COperateTemplateServer* pOperate, bool bStopSignal)
    : CBackendServer(pOperate, bStopSignal)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackendTemplateServer::~CBackendTemplateServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackend::OnInitReturnValue CBackendTemplateServer::OnInit()
{
    OnInitReturnValue ret = OnInitReturnValue::NotUseOnProcess;
    // TODO: Modify Initialization

    return ret;
}

int CBackendTemplateServer::OnClean()
{
    int nRet = 0;
    // TODO: Modify clean

    return nRet;
}

void CBackendTemplateServer::slotDisconnect(const QString &szIp, const quint16 port)
{
    // TODO: Disconnect
}
