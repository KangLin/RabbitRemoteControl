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

/*!
 * \~chinese 具体操作处理
 * \return 
 *       \li >= 0: 继续。再次调用间隔时间，单位毫秒
 *       \li = -1: 停止
 *       \li < -1: 错误代码
 *     
 * \~english Specific operation processing of plug-in
 * \return 
 *       \li >= 0: continue, Interval call time (msec)
 *       \li = -1: stop
 *       \li < -1: error code
 * \~
 * \see Start() slotTimeOut()
 */
int CBackendTemplateDesktop::OnProcess()
{
    // TODO: add event dispatch (non-Qt event loop)
    
    return 0;
}

void CBackendTemplateDesktop::slotClipBoardChanged()
{
    // TODO: add clipboard operate

}
