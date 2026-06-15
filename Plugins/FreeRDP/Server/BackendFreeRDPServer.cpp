// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ParameterFreeRDPServer.h"
#include "BackendFreeRDPServer.h"

static Q_LOGGING_CATEGORY(log, "FreeRDPServer.Backend")
CBackendFreeRDPServer::CBackendFreeRDPServer(
    COperateFreeRDPServer* pOperate, bool bStopSignal)
    : CBackendServer(pOperate, bStopSignal)
    , m_pPara(pOperate->GetParameter())
    , m_pSettings(nullptr)
    , m_pServer(nullptr)
    , m_bServerInit(false)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackendFreeRDPServer::~CBackendFreeRDPServer()
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
CBackend::OnInitReturnValue CBackendFreeRDPServer::OnInit()
{
    shadow_subsystem_set_entry_builtin(NULL);

    m_pServer = shadow_server_new();
    if (!m_pServer)
    {
        qCritical(log) << "Server new failed";
        return OnInitReturnValue::Fail;
    }

    SetParameters();

    int nRet = shadow_server_init(m_pServer);
    if(nRet < 0)
    {
        qCritical(log) << "Server initialization failed:" << nRet;
        return OnInitReturnValue::Fail;
    }
    m_bServerInit = true;

    if ((nRet = shadow_server_start(m_pServer)) < 0)
    {
        qCritical(log) << "Failed to start server:" << nRet;
        return OnInitReturnValue::Fail;
    }

    // Don't use OnProcess (qt event loop)
    // Because freerdp has new thread process loop
    return OnInitReturnValue::NotUseOnProcess;
}

int CBackendFreeRDPServer::OnClean()
{
    int nRet = 0;
    if(m_pServer)
    {
        if(m_bServerInit)
            shadow_server_uninit(m_pServer);
        shadow_server_free(m_pServer);
        m_pServer = nullptr;
    }

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
int CBackendFreeRDPServer::OnProcess()
{
    // TODO: add event dispatch (non-Qt event loop)

    // TODO: When an error occurs. emit sigStop();

    return 0;
}

int CBackendFreeRDPServer::SetParameters()
{
    m_pSettings = m_pServer->settings;
    m_pSettings->NlaSecurity = m_pPara->getNlaSecurity();
    m_pSettings->TlsSecurity = m_pPara->getTlsSecurity();
    m_pSettings->RdpSecurity = m_pPara->getRdpSecurity();
    m_pSettings->UseRdpSecurityLayer = m_pSettings->RdpSecurity;
    m_pSettings->ExtSecurity = m_pPara->getNlaExtSecurity();
    if(!m_pPara->getSamFile().isEmpty())
        freerdp_settings_set_string(m_pSettings, FreeRDP_NtlmSamFile,
                                    m_pPara->getSamFile().toStdString().c_str());

    m_pServer->authentication = m_pPara->getAuthentication();

    m_pServer->port = m_pPara->m_Net.GetPort();
    m_pServer->mayView = m_pPara->getMayView();
    m_pServer->mayInteract = m_pPara->getMayInteract();

    return 0;
}

void CBackendFreeRDPServer::slotDisconnect(const QString &szIp, const quint16 port)
{
    // TODO: Disconnect
}
