// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "BackendTemplateDesktop.h"

static Q_LOGGING_CATEGORY(log, "Backend.TemplateDesktop")
CBackendTemplateDesktop::CBackendTemplateDesktop(COperateTemplateDesktop *pOperate)
    : CBackendDesktop(pOperate)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackendTemplateDesktop::~CBackendTemplateDesktop()
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackend::OnInitReturnValue CBackendTemplateDesktop::OnInit()
{
    OnInitReturnValue ret = OnInitReturnValue::NotUseOnProcess;
    // TODO: Modify Initialization
    emit sigRunning();
    return ret;
}

int CBackendTemplateDesktop::OnClean()
{
    int nRet = 0;
    // TODO: Modify clean
    emit sigFinished();
    return nRet;
}

void CBackendTemplateDesktop::slotClipBoardChanged()
{
    // TODO: add clipboard operate

}