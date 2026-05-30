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
