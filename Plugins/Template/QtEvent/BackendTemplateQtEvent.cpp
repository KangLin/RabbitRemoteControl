// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "BackendTemplateQtEvent.h"

static Q_LOGGING_CATEGORY(log, "Backend.TemplateQtEvent")
CBackendTemplateQtEvent::CBackendTemplateQtEvent(
    COperateTemplateQtEvent* pOperate, bool bStopSignal)
    : CBackend(pOperate, bStopSignal)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackendTemplateQtEvent::~CBackendTemplateQtEvent()
{
    qDebug(log) << Q_FUNC_INFO;
}

/*!
 * \~chinese 初始化
 * \return
 * \li OnInitReturnValue::Fail: 错误
 * \li OnInitReturnValue::Success/OnInitReturnValue::UseOnProcess: 使用 OnProcess() (非 Qt 事件循环)
 * \li OnInitReturnValue::NotUseOnProcess: 不使用 OnProcess() (qt 事件循环)
 *
 * \~english Initialization
 * \return CBackend::OnInitReturnValue
 * \li OnInitReturnValue::Fail: error
 * \li OnInitReturnValue::Success/OnInitReturnValue::UseOnProcess: Use OnProcess() (non-Qt event loop)
 * \li OnInitReturnValue::NotUseOnProcess: Don't use OnProcess() (qt event loop)
 *
 * \~
 * \see Start()
 */
CBackend::OnInitReturnValue CBackendTemplateQtEvent::OnInit()
{
    OnInitReturnValue ret = OnInitReturnValue::NotUseOnProcess;
    // TODO: Modify Initialization

    // When ready
    emit sigRunning();

    return ret;
}

int CBackendTemplateQtEvent::OnClean()
{
    int nRet = 0;
    // TODO: Modify clean

    // When finished
    emit sigFinished();
    return nRet;
}
