#include <QLoggingCategory>
#include "BackendTemplateServer.h"

static Q_LOGGING_CATEGORY(log, "Backend.TemplateServer")
CBackendTemplateServer::CBackendTemplateServer(COperateTemplateServer* pOperate, bool bStopSignal)
    : CBackend(pOperate, bStopSignal)
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

    emit sigRunning();
    return ret;
}

int CBackendTemplateServer::OnClean()
{
    int nRet = 0;
    // TODO: Modify clean
    emit sigFinished();
    return nRet;
}