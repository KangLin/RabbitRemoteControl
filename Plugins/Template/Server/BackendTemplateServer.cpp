// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "BackendTemplateServer.h"
#include "Backend.h"

static Q_LOGGING_CATEGORY(log, "TemplateServer.Backend")
CBackendTemplateServer::CBackendTemplateServer(
    COperateTemplateServer* pOperate, bool bStopSignal)
    : CBackendServer(pOperate, bStopSignal)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackendTemplateServer::~CBackendTemplateServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

/*!
 * \~chinese 唤醒后台线程
 * \~english Wake up Backend thread(background thread)
 */
int CBackendTemplateServer::WakeUp()
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
CBackend::OnStartReturnValue CBackendTemplateServer::OnStart()
{
    OnStartReturnValue ret = OnStartReturnValue::NotUseOnProcess;
    // TODO: Modify Initialization

    return ret;
}

int CBackendTemplateServer::OnStop()
{
    int nRet = 0;
    // TODO: Modify clean

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
int CBackendTemplateServer::OnProcess()
{
    // TODO: add event dispatch (non-Qt event loop)

    // TODO: When an error occurs. emit sigStop();

    return 0;
}

void CBackendTemplateServer::slotDisconnect(const QString &szIp, const quint16 port)
{
    // TODO: Disconnect
}
