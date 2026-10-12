// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "BackendTemplateQtEvent.h"

static Q_LOGGING_CATEGORY(log, "TemplateQtEvent.Backend")
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
 * \~chinese 唤醒后台线程
 * \~english Wake up Backend thread(background thread)
 */
int CBackendTemplateQtEvent::WakeUp()
{
    // TODO: add wake up

    return 0;
}

/*!
 * \~chinese 初始化
 * \return
 * \li OnStartReturnValue::Fail: 错误
 * \li OnStartReturnValue::Success/OnStartReturnValue::UseOnProcess: 使用 OnProcess() (非 Qt 事件循环)
 * \li OnStartReturnValue::NotUseOnProcess: 不使用 OnProcess() (qt 事件循环)
 *
 * \~english Initialization
 * \return CBackend::OnStartReturnValue
 * \li OnStartReturnValue::Fail: error
 * \li OnStartReturnValue::Success/OnStartReturnValue::UseOnProcess: Use OnProcess() (non-Qt event loop)
 * \li OnStartReturnValue::NotUseOnProcess: Don't use OnProcess() (qt event loop)
 *
 * \~
 * \see Start()
 */
CBackend::OnStartReturnValue CBackendTemplateQtEvent::OnStart()
{
    OnStartReturnValue ret = OnStartReturnValue::NotUseOnProcess;
    // TODO: Modify Initialization

    // When ready
    emit sigRunning();

    return ret;
}

int CBackendTemplateQtEvent::OnStop()
{
    int nRet = 0;
    // TODO: Modify clean

    // When finished
    emit sigFinished();
    return nRet;
}
