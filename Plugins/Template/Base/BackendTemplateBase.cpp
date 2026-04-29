#include <QLoggingCategory>
#include "BackendTemplateBase.h"

static Q_LOGGING_CATEGORY(log, "Backend.TemplateBase")
CBackendTemplateBase::CBackendTemplateBase(COperateTemplateBase* pOperate, bool bStopSignal)
    : CBackend(pOperate, bStopSignal)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackendTemplateBase::~CBackendTemplateBase()
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackend::OnInitReturnValue CBackendTemplateBase::OnInit()
{
    OnInitReturnValue ret = OnInitReturnValue::NotUseOnProcess;
    // TODO: Modify Initialization

    emit sigRunning();
    return ret;
}

int CBackendTemplateBase::OnClean()
{
    int nRet = 0;
    // TODO: Modify clean
    emit sigFinished();
    return nRet;
}