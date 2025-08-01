// Author: Kang Lin <kl222@126.com>
// https://learn.microsoft.com/zh-cn/windows-server/remote/remote-desktop-services/welcome-to-rds
// X.509 Public Key Certificates: https://learn.microsoft.com/zh-cn/windows/win32/seccertenroll/about-x-509-public-key-certificates
// Cryptography: https://learn.microsoft.com/zh-cn/windows/win32/seccrypto/cryptography-portal

#undef PEN_FLAG_INVERTED
#include "freerdp/client.h"
#include "freerdp/client/channels.h"
#include "freerdp/channels/rdpei.h"
#include "freerdp/channels/rdpdr.h"
#include "freerdp/channels/disp.h"
#include "freerdp/channels/tsmf.h"
#include "freerdp/channels/rdpsnd.h"
#include "freerdp/client/encomsp.h"
#include "freerdp/gdi/gfx.h"
#include "freerdp/settings.h"
#include "freerdp/locale/keyboard.h"
#include "freerdp/channels/rdpgfx.h"
#include "freerdp/channels/cliprdr.h"
#include "freerdp/client/cmdline.h"
#include "freerdp/gdi/video.h"
#include "winpr/winsock.h"

#include "BackendFreeRDP.h"
#include "RabbitCommonTools.h"
#include "ConvertKeyCode.h"

#include <memory.h>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QSslCertificate>
#include <QInputDialog>
#include <QMutexLocker>
#include <QPainter>
#include <QPrinterInfo>
#include <QSerialPort>
#include <QSerialPortInfo>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QSoundEffect>
#else
    #include <QSound>
#endif

#ifdef WINPR_HAVE_POLL_H
#include <poll.h>
#else
#include <time.h>
#if defined(Q_OS_WIN)
#include <WinSock2.h>
#else
#include <sys/select.h>
#endif
#endif

#if FREERDP_VERSION_MAJOR >= 3
#include "ConnectLayerQTcpSocket.h"
#ifdef HAVE_LIBSSH
#include "ConnectLayerSSHTunnel.h"
#endif
#endif

static Q_LOGGING_CATEGORY(log, "FreeRDP.Connect")
static Q_LOGGING_CATEGORY(logKey, "FreeRDP.Connect.Key")
static Q_LOGGING_CATEGORY(logMouse, "FreeRDP.Connect.Mouse")

CBackendFreeRDP::CBackendFreeRDP(COperateFreeRDP *pOperate)
    : CBackendDesktop(pOperate)
    , m_pOperate(pOperate)
    , m_pContext(nullptr)
    , m_pParameter(nullptr)
    , m_ClipBoard(this)
    , m_Cursor(this)
    , m_writeEvent(nullptr)
#if FREERDP_VERSION_MAJOR >= 3
    , m_pConnectLayer(nullptr)
#endif
#ifdef HAVE_LIBSSH
    , m_pThreadSSH(nullptr)
#endif
{
    qDebug(log) << Q_FUNC_INFO;
    m_pParameter = qobject_cast<CParameterFreeRDP*>(pOperate->GetParameter());
    Q_ASSERT(m_pParameter);
}

CBackendFreeRDP::~CBackendFreeRDP()
{
    qDebug(log) << Q_FUNC_INFO;
}

/*
 * \return 
 * \li OnInitReturnValue::Fail: error
 * \li OnInitReturnValue::UseOnProcess: Use OnProcess (non-Qt event loop)
 * \li OnInitReturnValue::NotUseOnProcess: Don't use OnProcess (qt event loop)
 */
CBackendFreeRDP::OnInitReturnValue CBackendFreeRDP::OnInit()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    
    m_writeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if(!m_writeEvent) {
        qCritical(log) << "CreateEvent failed";
        return OnInitReturnValue::Fail;
    }
    ZeroMemory(&m_ClientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	m_ClientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
	m_ClientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
    //m_ClientEntryPoints.settings = m_pParameter->m_pSettings;
    m_ClientEntryPoints.GlobalInit = cbGlobalInit;
	m_ClientEntryPoints.GlobalUninit = cbGlobalUninit;
    m_ClientEntryPoints.ClientNew = cbClientNew;
    m_ClientEntryPoints.ClientFree = cbClientFree;
    m_ClientEntryPoints.ClientStart = cbClientStart;
	m_ClientEntryPoints.ClientStop = cbClientStop;
    m_ClientEntryPoints.ContextSize = sizeof(ClientContext);

    auto pRdpContext = freerdp_client_context_new(&m_ClientEntryPoints);
	if(pRdpContext)
    {
        m_pContext = (ClientContext*)pRdpContext;
        m_pContext->pThis = this;
    } else {
        qCritical(log) << "freerdp_client_context_new fail";
        return OnInitReturnValue::Fail;
    }

    rdpSettings* settings = pRdpContext->settings;
    if(!settings) {
        qCritical(log) << "settings is null";
        return OnInitReturnValue::Fail;
    }

    //*
    // Initial FreeRDP default parameters.
    // See: https://github.com/KangLin/RabbitRemoteControl/issues/27
    // Set default parameters: FreeRDP_SupportGraphicsPipeline and FreeRDP_RemoteFxCodec
    // See:
    //   - [[MS-RDPBCGR]: Remote Desktop Protocol: Basic Connectivity and Graphics Remoting](https://learn.microsoft.com/zh-cn/openspecs/windows_protocols/ms-rdpbcgr/5073f4ed-1e93-45e1-b039-6e30c385867c)
    //   - [[MS-RDPRFX]: Remote Desktop Protocol: RemoteFX Codec Extension](https://learn.microsoft.com/zh-cn/openspecs/windows_protocols/ms-rdprfx/62495a4a-a495-46ea-b459-5cde04c44549)
    //   - [[MS-RDPEGFX]: Remote Desktop Protocol: Graphics Pipeline Extension](https://learn.microsoft.com/zh-cn/openspecs/windows_protocols/ms-rdpegfx/da5c75f9-cd99-450c-98c4-014a496942b0)
    //      https://www.cswamp.com/post/37
    // Set connection parameters. See: FREERDP_API BOOL freerdp_set_connection_type(rdpSettings* settings, UINT32 type);
    char* argv[]= {(char*)QApplication::applicationFilePath().toStdString().c_str()};
    int argc = sizeof(argv) / sizeof(char*);
    nRet = freerdp_client_settings_parse_command_line(settings, argc, argv, TRUE);
    if (nRet)
    {
        nRet = freerdp_client_settings_command_line_status_print(settings, nRet, argc, argv);
        return OnInitReturnValue::Fail;
    } //*/

#if FREERDP_VERSION_MAJOR >= 3
    if (!stream_dump_register_handlers(pRdpContext,
                                       CONNECTION_STATE_MCS_CREATE_REQUEST,
                                       FALSE))
        return OnInitReturnValue::Fail;
#endif
    
    auto &user = m_pParameter->m_Net.m_User;
    if(!user.GetUser().isEmpty())
        freerdp_settings_set_string(
            settings, FreeRDP_Username,
            user.GetUser().toStdString().c_str());
    if(!user.GetPassword().isEmpty())
        freerdp_settings_set_string(
            settings, FreeRDP_Password,
            user.GetPassword().toStdString().c_str());

    freerdp_settings_set_bool(
        settings, FreeRDP_RedirectClipboard, m_pParameter->GetClipboard());
    
#if FREERDP_VERSION_MAJOR >= 3
    bool bOnlyView = m_pParameter->GetOnlyView();
    freerdp_settings_set_bool(
        settings, FreeRDP_SuspendInput, bOnlyView);
#endif
    
    freerdp_settings_set_uint32(settings, FreeRDP_DesktopWidth,
                                m_pParameter->GetDesktopWidth());
    freerdp_settings_set_uint32(settings, FreeRDP_DesktopHeight,
                                m_pParameter->GetDesktopHeight());
    freerdp_settings_set_uint32(settings, FreeRDP_ColorDepth,
                                m_pParameter->GetColorDepth());

    freerdp_settings_set_bool(settings, FreeRDP_UseMultimon,
                              m_pParameter->GetUseMultimon());
    
    if(m_pParameter->GetReconnectInterval()) {
        freerdp_settings_set_bool(
            settings, FreeRDP_AutoReconnectionEnabled, true);
        freerdp_settings_set_uint32(
            settings,
            FreeRDP_AutoReconnectMaxRetries,
            m_pParameter->GetReconnectInterval());
    }
    else
        freerdp_settings_set_bool(
            settings, FreeRDP_AutoReconnectionEnabled, false);

    //*Load channel
    RedirectionSound();
    RedirectionMicrophone();
    RedirectionDriver();
    RedirectionPrinter();
    RedirectionSerial();
    //*/

    // Set proxy
    switch(m_pParameter->m_Proxy.GetUsedType())
    {
    case CParameterProxy::TYPE::System:
        break;
    case CParameterProxy::TYPE::Http:
    case CParameterProxy::TYPE::SockesV5:
    {
        CParameterNet* net = &m_pParameter->m_Proxy.m_SockesV5;
        if (!freerdp_settings_set_uint32(settings, FreeRDP_ProxyType, PROXY_TYPE_SOCKS))
            return OnInitReturnValue::Fail;
        if(CParameterProxy::TYPE::Http == m_pParameter->m_Proxy.GetUsedType()) {
            net = &m_pParameter->m_Proxy.m_Http;
            if (!freerdp_settings_set_uint32(settings, FreeRDP_ProxyType, PROXY_TYPE_HTTP))
                return OnInitReturnValue::Fail;
        }
        auto &user = net->m_User;
        if (!freerdp_settings_set_string(settings, FreeRDP_ProxyHostname,
                                         net->GetHost().toStdString().c_str()))
            return OnInitReturnValue::Fail;
        if (!freerdp_settings_set_uint16(settings, FreeRDP_ProxyPort, net->GetPort()))
            return OnInitReturnValue::Fail;

        if(((user.GetUsedType() == CParameterUser::TYPE::UserPassword)
             && (user.GetPassword().isEmpty() || user.GetUser().isEmpty()))
            || ((user.GetUsedType() == CParameterUser::TYPE::PublicKey)
                && user.GetPassphrase().isEmpty())) {
            int nRet = QDialog::Rejected;
            emit sigBlockShowWidget("CDlgUserPassword", nRet, net);
            if(QDialog::Accepted != nRet)
            {
                return OnInitReturnValue::Fail;
            }
        }
        if (!freerdp_settings_set_string(settings, FreeRDP_ProxyUsername,
                                         user.GetUser().toStdString().c_str()))
            return OnInitReturnValue::Fail;
        if (!freerdp_settings_set_string(settings, FreeRDP_ProxyPassword,
                                         user.GetPassword().toStdString().c_str()))
            return OnInitReturnValue::Fail;
        /*
#if FREERDP_VERSION_MAJOR >= 3
        m_pConnectLayer = new CConnectLayerQTcpSocket(this);
        if(!m_pConnectLayer) return OnInitReturnValue::Fail;
        nRet = m_pConnectLayer->Initialize(pRdpContext);
        if(nRet) return OnInitReturnValue::Fail;
        return OnInitReturnValue::NotUseOnProcess;
#endif
*/
    }
    case CParameterProxy::TYPE::None:
    {
        if(!m_pParameter->GetDomain().isEmpty())
            freerdp_settings_set_string(
                settings, FreeRDP_Domain,
                m_pParameter->GetDomain().toStdString().c_str());
        if(m_pParameter->m_Net.GetHost().isEmpty())
        {
            QString szErr;
            szErr = tr("The server is empty, please input it");
            qCritical(log) << szErr;
            emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
            emit sigError(-1, szErr.toStdString().c_str());
            return OnInitReturnValue::Fail;
        }
        auto &net = m_pParameter->m_Net;
        freerdp_settings_set_string(
            settings, FreeRDP_ServerHostname,
            net.GetHost().toStdString().c_str());
        freerdp_settings_set_uint32(
            settings, FreeRDP_ServerPort,
            net.GetPort());
        
        nRet = freerdp_client_start(pRdpContext);
        if(nRet)
        {
            qCritical(log) << "freerdp_client_start fail";
            return OnInitReturnValue::Fail;
        }
        break;
    }
#ifdef HAVE_LIBSSH
    case CParameterProxy::TYPE::SSHTunnel:
    {
#if FREERDP_VERSION_MAJOR >= 3
        m_pConnectLayer = new ConnectLayerSSHTunnel(this);
        if(!m_pConnectLayer) return OnInitReturnValue::Fail;
        nRet = m_pConnectLayer->Initialize(pRdpContext);
        if(nRet) return OnInitReturnValue::Fail;
        break;
#endif
        return InitSSHTunnelPipe();
    }
#endif // HAVE_LIBSSH

    default:
        qCritical(log) << "Don't support proxy type:" << m_pParameter->m_Proxy.GetUsedType();
        break;
    };

    return OnInitReturnValue::UseOnProcess;
}

int CBackendFreeRDP::OnClean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    if(m_writeEvent)
    {
        CloseHandle(m_writeEvent);
        m_writeEvent = nullptr;
    }
    if(m_pContext)
    {
        rdpContext* pRdpContext = (rdpContext*)m_pContext;
        if(!freerdp_disconnect(pRdpContext->instance))
            qCritical(log) << "freerdp_disconnect fail";

        if(freerdp_client_stop(pRdpContext))
            qCritical(log) << "freerdp_client_stop fail";

        freerdp_client_context_free(pRdpContext);
        m_pContext = nullptr;
    }

#if FREERDP_VERSION_MAJOR >= 3
    if(m_pConnectLayer) {
        m_pConnectLayer->Clean();
        m_pConnectLayer->deleteLater();
        m_pConnectLayer = nullptr;
    }
#endif

#ifdef HAVE_LIBSSH
    CleanSSHTunnelPipe();
#endif

    return nRet;
}

/*!
 * \~chinese 插件连接的具体操作处理。因为此插件是非Qt事件，所以在此函数中等待。
 *
 * \~english Specific operation processing of plug-in connection.
 *           Because of it is a non-Qt event loop, so wait in here.
 *
 * \~
 * \return 
 *    \li >= 0: continue, Interval call time (msec)
 *    \li = -1: stop
 *    \li < -1: error
 * \see CConnect::Connect() CConnect::slotTimeOut()
 */
int CBackendFreeRDP::OnProcess()
{
    return OnProcess(DEFAULT_TIMEOUT);
}

/*!
 * \~chinese 插件连接的具体操作处理。因为此插件是非Qt事件，所以在此函数中等待。
 *
 * \~english Specific operation processing of plug-in connection.
 *           Because of it is a non-Qt event loop, so wait in here.
 *
 * \~
 * \return 
 *    \li >= 0: continue, Interval call time (msec)
 *    \li = -1: stop
 *    \li < -1: error
 * \see CConnect::Connect() CConnect::slotTimeOut()
 */
int CBackendFreeRDP::OnProcess(int nTimeout)
{
    //qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    HANDLE handles[64];
    rdpContext* pRdpContext = (rdpContext*)m_pContext;

    if(nullptr == freerdp_settings_get_string(pRdpContext->settings, FreeRDP_ServerHostname))
    {
        return 50;
    }

    do {
        DWORD nCount = 0;

        nCount += freerdp_get_event_handles(pRdpContext, &handles[nCount],
                                           ARRAYSIZE(handles) - nCount);
        if (nCount == 0)
        {
            qCritical(log) << "freerdp_get_event_handles failed";
            nRet = -2;
            break;
        }

        if(m_writeEvent)
            handles[nCount++] = m_writeEvent;

        DWORD waitStatus = WaitForMultipleObjects(nCount, handles, FALSE, nTimeout);

        if(m_writeEvent)
            ResetEvent(m_writeEvent);

        if (waitStatus == WAIT_FAILED)
        {
            qCritical(log) << "WaitForMultipleObjects: WAIT_FAILED";
            nRet = -2;
            break;
        }

        if(waitStatus == WAIT_TIMEOUT)
        {
            //qDebug(log) << "WaitForMultipleObjects timeout";
            nRet = 0;
            break;
        }

        if (!freerdp_check_event_handles(pRdpContext))
        {
            nRet = -2;

            UINT32 err = freerdp_get_last_error(pRdpContext);
            QString szErr;
            szErr = "freerdp_check_event_handles fail.";
            szErr += " [";
            szErr += QString::number(err);
            szErr += " - ";
            szErr += freerdp_get_last_error_category(err);
            szErr += " - ";
            szErr += freerdp_get_last_error_name(err);
            szErr += "] ";
            szErr += freerdp_get_last_error_string(err);
            qCritical(log) << szErr;
            emit sigError(err, szErr);

            /* Reconnect
            freerdp *instance = pRdpContext->instance;
            if (client_auto_reconnect(instance))
            {
                nRet = 0;
                break;
            }

            err = freerdp_get_last_error(pRdpContext);
            szErr = "client_auto_reconnect[";
            szErr += QString::number(err);
            szErr += "]";
            szErr += freerdp_get_last_error_category(err);
            szErr += "-";
            szErr += freerdp_get_last_error_name(err);
            szErr += ":";
            szErr += freerdp_get_last_error_string(err);
            qCritical(log) << szErr;
            emit sigError(err, szErr);//*/
        }

#if FREERDP_VERSION_MAJOR >= 3
        if(freerdp_shall_disconnect_context(pRdpContext))
#else
        if(freerdp_shall_disconnect(pRdpContext->instance))
#endif
        {
            qCritical(log) << "freerdp_shall_disconnect false";
            nRet = -2;
        }
    } while(false);

    return nRet;
}

void CBackendFreeRDP::slotClipBoardChanged()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    if(m_pParameter->GetClipboard())
        m_ClipBoard.slotClipBoardChanged();
}

BOOL CBackendFreeRDP::cbGlobalInit()
{
	qDebug(log) << Q_FUNC_INFO;
	return TRUE;
}

void CBackendFreeRDP::cbGlobalUninit()
{
    qDebug(log) << Q_FUNC_INFO;
}

BOOL CBackendFreeRDP::cbClientNew(freerdp *instance, rdpContext *context)
{
    qDebug(log) << Q_FUNC_INFO;
    instance->PreConnect = cb_pre_connect;
	instance->PostConnect = cb_post_connect;
	instance->PostDisconnect = cb_post_disconnect;

    // Because it is already set in the parameters
#if FREERDP_VERSION_MAJOR < 3
    instance->Authenticate = cb_authenticate;
    instance->GatewayAuthenticate = cb_GatewayAuthenticate;
#else
    instance->AuthenticateEx = cb_authenticate_ex;
    instance->ChooseSmartcard = cb_choose_smartcard;
#endif
    instance->VerifyX509Certificate = cb_verify_x509_certificate;
    instance->VerifyCertificateEx = cb_verify_certificate_ex;
	instance->VerifyChangedCertificateEx = cb_verify_changed_certificate_ex;
	instance->PresentGatewayMessage = cb_present_gateway_message;

	instance->LogonErrorInfo = cb_logon_error_info;

    return TRUE;
}

void CBackendFreeRDP::cbClientFree(freerdp *instance, rdpContext *context)
{
    qDebug(log) << Q_FUNC_INFO;
}

int CBackendFreeRDP::cbClientStart(rdpContext *context)
{
    qDebug(log) << Q_FUNC_INFO;
    UINT32 nRet = 0;

    if (!context || !context->settings)
        return -1;
    freerdp* instance = freerdp_client_get_instance(context);
    if(!instance)
        return -2;
    CBackendFreeRDP* pThis = ((ClientContext*)context)->pThis;
    auto settings = context->settings;

    QString szDomainHost;
    quint16 nPort;
    szDomainHost = freerdp_settings_get_string(settings, FreeRDP_ServerHostname);
    nPort = freerdp_settings_get_uint32(settings, FreeRDP_ServerPort);
    QString szServer;
    auto &net = pThis->m_pParameter->m_Net;
    szServer = net.GetHost() + ":" + QString::number(net.GetPort());
    auto &proxy = pThis->m_pParameter->m_Proxy;
    switch(proxy.GetUsedType()) {
    case CParameterProxy::TYPE::SockesV5:
    {
        auto &sockesV5 = proxy.m_SockesV5;
        szServer = sockesV5.GetHost() + ":" + QString::number(sockesV5.GetPort())
                   + " -> " + szServer;
        break;
    }
    case CParameterProxy::TYPE::Http:
    {
        auto &http = proxy.m_Http;
        szServer = http.GetHost() + ":" + QString::number(http.GetPort())
                   + " -> " + szServer;
        break;
    }
    case CParameterProxy::TYPE::SSHTunnel:
    {
        auto &sshNet = proxy.m_SSH.m_Net;
#if FREERDP_VERSION_MAJOR < 3
        szServer = szDomainHost + ":" + QString::number(nPort)
                   + " -> " + sshNet.GetHost() + ":" + QString::number(sshNet.GetPort())
                   + " -> " + szServer;
#else
        szServer = sshNet.GetHost() + ":" + QString::number(sshNet.GetPort())
                   + " -> " + szServer;
#endif
        break;
    }
    default:
        break;
    }

    BOOL status = freerdp_connect(instance);
    if (status) {
        QString szInfo = tr("Connected to ") + szServer;
        qInfo(log) << szInfo;
        emit pThis->sigInformation(szInfo);
    } else {
        //DWORD dwErrCode = freerdp_error_info(instance);
        UINT32 nRet = freerdp_get_last_error(context);

        QString szErr;
        szErr = tr("Connect to ") + szServer + tr(" fail.");
        szErr += "\n[";
        szErr += QString::number(nRet) + " - ";
        szErr += freerdp_get_last_error_name(nRet);
        szErr += "] ";
        /*szErr += "[";
        szErr += freerdp_get_last_error_category(nRet);
        szErr += "] ";*/
        szErr += freerdp_get_last_error_string(nRet);
        //szErr += "]";

        switch(nRet) {
        case FREERDP_ERROR_CONNECT_LOGON_FAILURE:
        {
            szErr = tr("Logon to ") + szServer;
            szErr += tr(" fail. Please check that the username and password are correct.") + "\n";
            break;
        }
        case FREERDP_ERROR_CONNECT_WRONG_PASSWORD:
        {
            szErr = tr("Logon to ") + szServer;
            szErr += tr(" fail. Please check password are correct.") + "\n";
            break;
        }
        case FREERDP_ERROR_AUTHENTICATION_FAILED:
        {
            szErr = tr("Logon to ") + szServer;
            szErr += tr(" authentication fail. please add a CA certificate to the store.") + "\n";
            break;
        }
        case FREERDP_ERROR_CONNECT_TRANSPORT_FAILED:
        {
            szErr = tr("Logon to ") + szServer;
            szErr += tr(" connect transport layer fail.") + "\n\n";
            szErr += tr("Please:") + "\n";
            szErr += tr("1. Check for any network related issues") + "\n";
            szErr += tr("2. Check you have proper security settings ('NLA' enabled is required for most connections nowadays)") + "\n";
            szErr += "    " + tr("If you do not know the server security settings, contact your server administrator.") + "\n";
            szErr += tr("3. Check the certificate is proper (and guacd properly checks that)") + "\n";
            break;
        }
        case FREERDP_ERROR_SECURITY_NEGO_CONNECT_FAILED:
            szErr += "\n\n";
            szErr += tr("Please check you have proper security settings.") + "\n";
            szErr += tr("If you do not know the server security settings, contact your server administrator.");
            break;
        case FREERDP_ERROR_CONNECT_CANCELLED:
            szErr = tr("The connect was canceled.") + "\n\n" + szErr;
            break;
        default:
            break;
        }

        emit pThis->sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
        qCritical(log) << szErr;
        emit pThis->sigError(nRet, szErr.toStdString().c_str());
    }

    return nRet;
}

int CBackendFreeRDP::cbClientStop(rdpContext *context)
{
    int nRet = 0;
    qDebug(log) << Q_FUNC_INFO;
#if FREERDP_VERSION_MAJOR >= 3
    nRet = freerdp_client_common_stop(context);
#else
    BOOL bRet = freerdp_abort_connect(context->instance);
    if(!bRet)
    {   qCritical(log) << "freerdp_abort_connect fail";
        nRet = -1;
    }
#endif
    return nRet;
}

/**
 * \~chinese
 * 连接之前调用。用于加载通道，检查和设置所有参数
 *
 * \~english
 * Called before a connection is established.
 * Set all configuration options to support and load channels here.
 * 
 * Callback given to freerdp_connect() to process the pre-connect operations.
 * It will fill the rdp_freerdp structure (instance) with the appropriate options to use for the
 * connection.
 *
 * @param instance - pointer to the rdp_freerdp structure that contains the connection's parameters,
 * and will be filled with the appropriate information.
 *
 * @return TRUE if successful. FALSE otherwise.
 * Can exit with error code XF_EXIT_PARSE_ARGUMENTS if there is an error in the parameters.
 */
BOOL CBackendFreeRDP::cb_pre_connect(freerdp* instance)
{
    qDebug(log) << Q_FUNC_INFO;
	rdpChannels* channels = nullptr;
	rdpSettings* settings = nullptr;
	rdpContext* context = instance->context;

    if (!instance || !instance->context || !instance->context->settings)
        return FALSE;

    CBackendFreeRDP* pThis = ((ClientContext*)context)->pThis;
    if(!pThis) return FALSE;
    settings = instance->context->settings;
	channels = context->channels;
    CParameterFreeRDP* pParameter = pThis->m_pParameter;
    if(!channels || !pParameter)
        return FALSE;

    /* Optional OS identifier sent to server */
#if defined (Q_OS_WIN)
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMajorType, OSMAJORTYPE_WINDOWS))
        return FALSE;
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMinorType, OSMINORTYPE_WINDOWS_NT))
        return FALSE;
#elif defined(Q_OS_ANDROID)
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMajorType, OSMAJORTYPE_ANDROID))
        return FALSE;
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMinorType, OSMINORTYPE_UNSPECIFIED))
        return FALSE;
#elif defined(Q_OS_IOS)
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMajorType, OSMAJORTYPE_IOS))
        return FALSE;
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMinorType, OSMINORTYPE_UNSPECIFIED))
        return FALSE;
#elif defined (Q_OS_UNIX)
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMajorType, OSMAJORTYPE_UNIX))
        return FALSE;
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMinorType, OSMINORTYPE_NATIVE_XSERVER))
        return FALSE;
#else
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMajorType, OSMAJORTYPE_UNSPECIFIED))
        return FALSE;
    if (!freerdp_settings_set_uint32(
            settings, FreeRDP_OsMinorType, OSMINORTYPE_UNSPECIFIED))
        return FALSE;
#endif

    // Subscribe channel event
    PubSub_SubscribeChannelConnected(instance->context->pubSub,
                                     OnChannelConnectedEventHandler);
	PubSub_SubscribeChannelDisconnected(instance->context->pubSub,
	                                    OnChannelDisconnectedEventHandler);

#if FREERDP_VERSION_MAJOR < 3
	if (!freerdp_client_load_addins(channels, instance->context->settings))
		return FALSE;
#else
    #if defined(Q_OS_LINUX) || (defined(Q_OS_WIN) && defined(WITH_WINDOWS_CERT_STORE))
        if (!freerdp_settings_set_bool(settings, FreeRDP_CertificateCallbackPreferPEM, TRUE))
            return FALSE;
    #endif
#endif
    
    if(!freerdp_settings_set_bool(
            settings, FreeRDP_NegotiateSecurityLayer,
            pParameter->GetNegotiateSecurityLayer()))
        return FALSE;
    CParameterFreeRDP::Security security = pParameter->GetSecurity();
    // [5.3 Standard RDP Security](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/8e8b2cca-c1fa-456c-8ecb-a82fc60b2322)
    if(!freerdp_settings_set_bool(
            settings, FreeRDP_RdpSecurity,
            CParameterFreeRDP::Security::RDP & security))
        return FALSE;
    if (!freerdp_settings_set_bool(
            settings, FreeRDP_UseRdpSecurityLayer,
            CParameterFreeRDP::Security::RDP & security))
        return FALSE;
    // [5.4 Enhanced RDP Security](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/592a0337-dc91-4de3-a901-e1829665291d)
    if(!freerdp_settings_set_bool(
            settings, FreeRDP_TlsSecurity,
            CParameterFreeRDP::Security::TLS & security))
        return FALSE;
    if(!freerdp_settings_set_bool(
            settings, FreeRDP_NlaSecurity,
            CParameterFreeRDP::Security::NLA & security))
        return FALSE;
    if(!freerdp_settings_set_bool(
            settings, FreeRDP_ExtSecurity,
            CParameterFreeRDP::Security::NLA_Ext & security))
        return FALSE;
#if FREERDP_VERSION_MAJOR >= 3
    if(!freerdp_settings_set_bool(
            settings, FreeRDP_AadSecurity,
            CParameterFreeRDP::Security::RDSAAD & security))
        return FALSE;
    if(!freerdp_settings_set_bool(
            settings, FreeRDP_RdstlsSecurity,
            CParameterFreeRDP::Security::RDSTLS & security))
        return FALSE;
    freerdp_settings_set_uint16(settings, FreeRDP_TLSMinVersion,
                                pParameter->GetTlsVersion());
#endif

    // Check authentication parameters
    if (freerdp_settings_get_bool(settings, FreeRDP_AuthenticationOnly))
    {
        /* Check +auth-only has a username and password. */
        auto &user = pParameter->m_Net.m_User;
        if(!freerdp_settings_get_string(settings, FreeRDP_Username)) {
            if(user.GetUser().isEmpty()) {
                if(user.GetUser().isEmpty()) {
                    // Will be call instance->Authenticate = cb_authenticate
                    qWarning(log) << "Auth-only, but no user name set. Will be call instance->Authenticate.";
                    }
            } else
                freerdp_settings_set_string(
                    settings, FreeRDP_Username,
                    user.GetUser().toStdString().c_str());
        }
        if (!freerdp_settings_get_string(settings, FreeRDP_Password)) {
            if (user.GetPassword().isEmpty()) {
                // Will be call instance->Authenticate = cb_authenticate
                qWarning(log) << "auth-only, but no password set. Will be call instance->Authenticate";
            } else
                freerdp_settings_set_string(
                    settings, FreeRDP_Password,
                    user.GetPassword().toStdString().c_str());
        }
#if FREERDP_VERSION_MAJOR >= 3
        if (!freerdp_settings_set_bool(settings, FreeRDP_DeactivateClientDecoding, TRUE))
            return FALSE;
#endif
    } else if(freerdp_settings_get_bool(settings, FreeRDP_CredentialsFromStdin)){
        // Because the pragram is GUI. so don't process it.
    } else if(freerdp_settings_get_bool(settings, FreeRDP_SmartcardLogon)) {
        // TODO: add FreeRDP_SmartcardLogon !
    }

    /* TODO: Check Keyboard layout
    UINT32 rc = freerdp_keyboard_init(
                freerdp_settings_get_uint32(settings, FreeRDP_KeyboardLayout));
    freerdp_settings_set_uint32(settings, FreeRDP_KeyboardLayout, rc);
    //*/
    
    // Check desktop size, it is set in parameter
    UINT32 width = pParameter->GetDesktopWidth();
    UINT32 height = pParameter->GetDesktopHeight();
    if ((width < 64) || (height < 64) ||
            (width > 4096) || (height > 4096))
    {
        QString szErr = tr("Invalid dimensions:")
                        + QString::number(width)
                        + "*" + QString::number(height);
        qCritical(log) << szErr;
        //emit pThis->sigShowMessageBox(tr("Error"), szErr);
        return FALSE;
    } else {
        qInfo(log) << "Init desktop size " <<  width << "*" << height;
    }

    qDebug(log)
        << "width:" << freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth)
        << "height:" << freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight)
        << "ColorDepth:" << freerdp_settings_get_uint32(settings, FreeRDP_ColorDepth);

    // Initial FreeRDP graph codec
    // See: https://github.com/KangLin/RabbitRemoteControl/issues/27
    // Set default parameters FreeRDP_SupportGraphicsPipeline and FreeRDP_RemoteFxCodec in auto detec connect type
    // See:
    //   - [[MS-RDPBCGR]: Remote Desktop Protocol: Basic Connectivity and Graphics Remoting](https://learn.microsoft.com/zh-cn/openspecs/windows_protocols/ms-rdpbcgr/5073f4ed-1e93-45e1-b039-6e30c385867c)
    //   - [[MS-RDPRFX]: Remote Desktop Protocol: RemoteFX Codec Extension](https://learn.microsoft.com/zh-cn/openspecs/windows_protocols/ms-rdprfx/62495a4a-a495-46ea-b459-5cde04c44549)
    //   - [[MS-RDPEGFX]: Remote Desktop Protocol: Graphics Pipeline Extension](https://learn.microsoft.com/zh-cn/openspecs/windows_protocols/ms-rdpegfx/da5c75f9-cd99-450c-98c4-014a496942b0)
    //      https://www.cswamp.com/post/37
    //   - [Remote Desktop Protocol (RDP) 10 AVC/H.264 improvements in Windows 10 and Windows Server 2016 Technical Preview](https://techcommunity.microsoft.com/blog/microsoft-security-blog/remote-desktop-protocol-rdp-10-avch-264-improvements-in-windows-10-and-windows-s/249588)
    if(!freerdp_set_connection_type(settings, pParameter->GetConnectType()))
        return FALSE;
    freerdp_settings_set_uint32(
        settings, FreeRDP_PerformanceFlags, pParameter->GetPerformanceFlags());
    freerdp_performance_flags_split(settings);

    freerdp_settings_set_bool(settings, FreeRDP_UnicodeInput, pParameter->GetEnableInputMethod());
    return TRUE;
}

const char* CBackendFreeRDP::GetTitle(freerdp* instance)
{
    const char* windowTitle;
    UINT32 port;
    BOOL addPort;
    const char* name = nullptr;

    CBackendFreeRDP* pThis = ((ClientContext*)instance->context)->pThis;
    rdpSettings* settings = instance->context->settings;

    if (!settings)
        return nullptr;

    windowTitle = freerdp_settings_get_string(settings, FreeRDP_WindowTitle);
    if (windowTitle)
        return windowTitle;

#if FREERDP_VERSION_MAJOR >= 3
    name = freerdp_settings_get_server_name(settings);
#else
    name = pThis->m_pParameter->m_Net.GetHost().toStdString().c_str();
#endif
    port = freerdp_settings_get_uint32(settings, FreeRDP_ServerPort);

    addPort = (port != 3389);

    char buffer[MAX_PATH + 64] = { 0 };

    if (!addPort)
        sprintf_s(buffer, sizeof(buffer), "%s", name);
    else
        sprintf_s(buffer, sizeof(buffer), "%s:%" PRIu32, name, port);

    freerdp_settings_set_string(settings, FreeRDP_WindowTitle, buffer);
    return freerdp_settings_get_string(settings, FreeRDP_WindowTitle);
}

/**
 * Callback given to freerdp_connect() to perform post-connection operations.
 * It will be called only if the connection was initialized properly, and will continue the
 * initialization based on the newly created connection.
 */
BOOL CBackendFreeRDP::cb_post_connect(freerdp* instance)
{
    qDebug(log) << Q_FUNC_INFO;
	
	rdpContext* context = instance->context;
	rdpSettings* settings = instance->context->settings;
    rdpUpdate* update = instance->context->update;
    CBackendFreeRDP* pThis = ((ClientContext*)instance->context)->pThis;
    
    const char* pWindowTitle = GetTitle(instance);
    if(pWindowTitle)
    {
        WCHAR* windowTitle = NULL;
#if FREERDP_VERSION_MAJOR >= 3
        windowTitle = ConvertUtf8ToWCharAlloc(pWindowTitle, NULL);
#else
        ConvertToUnicode(CP_UTF8, 0, pWindowTitle, -1, &windowTitle, 0);
#endif
        if(windowTitle)
        {
            QString title = QString::fromUtf16((const char16_t*)windowTitle);
            delete windowTitle;
            if(pThis->m_pParameter->GetServerName().isEmpty())
                emit pThis->sigServerName(title);
        }
    }

    int desktopWidth = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    int desktopHeight = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);
    emit pThis->sigSetDesktopSize(desktopWidth, desktopHeight);

    if (!gdi_init(instance, PIXEL_FORMAT_BGRA32))
        return FALSE;

    if(!pThis->CreateImage(instance->context))
        return FALSE;

    Q_ASSERT(instance->context->cache);

    // Register cursor pointer
    if(pThis->m_Cursor.RegisterPointer(context->graphics))
        return FALSE;

    update->BeginPaint = cb_begin_paint;
    update->EndPaint = cb_end_paint;
    update->DesktopResize = cb_desktop_resize;

	update->PlaySound = cb_play_bell_sound;

	update->SetKeyboardIndicators = cb_keyboard_set_indicators;
	update->SetKeyboardImeStatus = cb_keyboard_set_ime_status;
    
    emit pThis->sigRunning();
	return TRUE;
}

void CBackendFreeRDP::cb_post_disconnect(freerdp* instance)
{
    qDebug(log) << Q_FUNC_INFO;
	rdpContext* context = nullptr;

	if (!instance || !instance->context)
		return;

	context = instance->context;
	
	PubSub_UnsubscribeChannelConnected(instance->context->pubSub,
	                                   OnChannelConnectedEventHandler);
	PubSub_UnsubscribeChannelDisconnected(instance->context->pubSub,
	                                      OnChannelDisconnectedEventHandler);
	gdi_free(instance);
}

int CBackendFreeRDP::cb_logon_error_info(freerdp* instance, UINT32 data, UINT32 type)
{
	CBackendFreeRDP* pThis = ((ClientContext*)instance->context)->pThis;
	const char* str_data = freerdp_get_logon_error_info_data(data);
	const char* str_type = freerdp_get_logon_error_info_type(type);
    QString szErr = tr("FreeRDP logon info: [");
    szErr += str_type;
    szErr += "] ";
    szErr += str_data;
	qDebug(log) << szErr;
	emit pThis->sigInformation(szErr);
    emit pThis->sigError(type, szErr);
	return 1;
}

void CBackendFreeRDP::OnChannelConnectedEventHandler(void *context,
                                                     #if FREERDP_VERSION_MAJOR >= 3
                                                     const
                                                     #endif
                                                     ChannelConnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    CBackendFreeRDP* pThis = ((ClientContext*)context)->pThis;
    if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0) {
        qDebug(log) << "channel" << e->name << "connected";
        pThis->m_ClipBoard.Init((CliprdrClientContext*)e->pInterface,
                                pThis->m_pParameter->GetClipboard());
    }
#if FREERDP_VERSION_MAJOR >= 3
    else
        freerdp_client_OnChannelConnectedEventHandler(pContext, e);
#else
    else if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
    {
        if (freerdp_settings_get_bool(pContext->settings, FreeRDP_SoftwareGdi)) {
            rdpGdi* gdi = pContext->gdi;
            // See: https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpegfx/da5c75f9-cd99-450c-98c4-014a496942b0
            gdi_graphics_pipeline_init(gdi, (RdpgfxClientContext*) e->pInterface);
        }
        else
            qDebug(log, "Unimplemented: channel %s connected but libfreerdp is in HardwareGdi mode\n", e->name);
    }
    else if (strcmp(e->name, GEOMETRY_DVC_CHANNEL_NAME) == 0)
    {
        gdi_video_geometry_init(pContext->gdi, (GeometryClientContext*)e->pInterface);
    }
    else if (strcmp(e->name, VIDEO_DATA_DVC_CHANNEL_NAME) == 0)
    {
        gdi_video_data_init(pContext->gdi, (VideoClientContext*)e->pInterface);
    } else
        qDebug(log) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
#endif
}

void CBackendFreeRDP::OnChannelDisconnectedEventHandler(void *context,
                                                        #if FREERDP_VERSION_MAJOR >= 3
                                                        const
                                                        #endif
                                                        ChannelDisconnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    CBackendFreeRDP* pThis = ((ClientContext*)context)->pThis;
    
    if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0) {
		qDebug(log) << "channel" << e->name << "disconnected";
        pThis->m_ClipBoard.UnInit((CliprdrClientContext*)e->pInterface,
                                  pThis->m_pParameter->GetClipboard());
	}
#if FREERDP_VERSION_MAJOR >= 3
    else
        freerdp_client_OnChannelDisconnectedEventHandler(pContext, e);
#else
    else if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
    {
        if (freerdp_settings_get_bool(pContext->settings, FreeRDP_SoftwareGdi)) {
            rdpGdi* gdi = pContext->gdi;
            gdi_graphics_pipeline_uninit(gdi, (RdpgfxClientContext*) e->pInterface);
        }
        else
            qDebug(log, "Unimplemented: channel %s connected but libfreerdp is in HardwareGdi mode\n", e->name);
    }
    else
        qDebug(log) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
#endif

}

UINT32 CBackendFreeRDP::GetImageFormat(QImage::Format format)
{
    switch (format) {
#if (QT_VERSION >= QT_VERSION_CHECK(5,2,0))
    case QImage::Format_RGBA8888:
        return PIXEL_FORMAT_RGBA32;
    case QImage::Format_RGBX8888:
        return PIXEL_FORMAT_RGBX32;
#endif
    case QImage::Format_RGB16:
        return PIXEL_FORMAT_RGB16;
    case QImage::Format_ARGB32:
        return PIXEL_FORMAT_BGRA32;
    case QImage::Format_RGB32:
        return PIXEL_FORMAT_BGRA32;
    default:
        break;
    }
    return 0;
}

UINT32 CBackendFreeRDP::GetImageFormat()
{
    return GetImageFormat(m_Image.format());
}

BOOL CBackendFreeRDP::CreateImage(rdpContext *context)
{
    Q_ASSERT(context);
    ClientContext* pContext = (ClientContext*)context;
    CBackendFreeRDP* pThis = pContext->pThis;
    rdpGdi* gdi = context->gdi;
    Q_ASSERT(pThis && gdi);
    pThis->m_Image = QImage(gdi->primary_buffer,
                            static_cast<int>(gdi->width),
                            static_cast<int>(gdi->height),
                            QImage::Format_ARGB32);
    return TRUE;
}

#if FREERDP_VERSION_MAJOR >= 3
#ifdef Q_OS_WINDOWS
static CREDUI_INFOW wfUiInfo = { sizeof(CREDUI_INFOW), NULL, L"Enter your credentials",
                                L"Remote Desktop Security", NULL };
#endif

BOOL CBackendFreeRDP::cb_authenticate_ex(freerdp* instance,
                               char** username, char** password,
                               char** domain, rdp_auth_reason reason)
{
    qDebug(log) << Q_FUNC_INFO << "reason:" << reason;
    if(!instance)
        return FALSE;

    if(!username || !password || !domain) return FALSE;

    rdpContext* pContext = (rdpContext*)instance->context;
#ifdef Q_OS_WINDOWS
    BOOL fSave;
    DWORD status;
    DWORD dwFlags;
    WCHAR UserNameW[CREDUI_MAX_USERNAME_LENGTH + 1] = { 0 };
    WCHAR UserW[CREDUI_MAX_USERNAME_LENGTH + 1] = { 0 };
    WCHAR DomainW[CREDUI_MAX_DOMAIN_TARGET_LENGTH + 1] = { 0 };
    WCHAR PasswordW[CREDUI_MAX_PASSWORD_LENGTH + 1] = { 0 };
    
    WINPR_ASSERT(instance);
    WINPR_ASSERT(instance->context);
    WINPR_ASSERT(instance->context->settings);
    
    WINPR_ASSERT(username);
    WINPR_ASSERT(domain);
    WINPR_ASSERT(password);
    
    const WCHAR auth[] = L"Target credentials requested";
    const WCHAR authPin[] = L"PIN requested";
    const WCHAR gwAuth[] = L"Gateway credentials requested";
    const WCHAR* titleW = auth;
    
    fSave = FALSE;
    dwFlags = CREDUI_FLAGS_DO_NOT_PERSIST | CREDUI_FLAGS_EXCLUDE_CERTIFICATES |
              CREDUI_FLAGS_USERNAME_TARGET_CREDENTIALS;
    switch (reason)
    {
    case AUTH_NLA:
        break;
    case AUTH_TLS:
    case AUTH_RDP:
        if ((*username) && (*password))
            return TRUE;
        break;
    case AUTH_SMARTCARD_PIN:
        dwFlags &= ~CREDUI_FLAGS_USERNAME_TARGET_CREDENTIALS;
        dwFlags |= CREDUI_FLAGS_PASSWORD_ONLY_OK | CREDUI_FLAGS_KEEP_USERNAME;
        titleW = authPin;
        if (*password)
            return TRUE;
        if (!(*username))
            *username = _strdup("PIN");
        break;
    case GW_AUTH_HTTP:
    case GW_AUTH_RDG:
    case GW_AUTH_RPC:
        titleW = gwAuth;
        break;
    default:
        return FALSE;
    }
    
    if (*username)
    {
        ConvertUtf8ToWChar(*username, UserNameW, ARRAYSIZE(UserNameW));
        ConvertUtf8ToWChar(*username, UserW, ARRAYSIZE(UserW));
    }
    
    if (*password)
        ConvertUtf8ToWChar(*password, PasswordW, ARRAYSIZE(PasswordW));
    
    if (*domain)
        ConvertUtf8ToWChar(*domain, DomainW, ARRAYSIZE(DomainW));
    
    if (_wcsnlen(PasswordW, ARRAYSIZE(PasswordW)) == 0)
    {
        status = CredUIPromptForCredentialsW(&wfUiInfo, titleW, NULL, 0, UserNameW,
                                             ARRAYSIZE(UserNameW), PasswordW,
                                             ARRAYSIZE(PasswordW), &fSave, dwFlags);
        if (status != NO_ERROR)
        {
            qCritical(log,
                      "CredUIPromptForCredentials unexpected status: 0x%08lX",
                      status);
            return FALSE;
        }
        
        if ((dwFlags & CREDUI_FLAGS_KEEP_USERNAME) == 0)
        {
            status = CredUIParseUserNameW(UserNameW, UserW, ARRAYSIZE(UserW), DomainW,
                                          ARRAYSIZE(DomainW));
            if (status != NO_ERROR)
            {
                CHAR User[CREDUI_MAX_USERNAME_LENGTH + 1] = { 0 };
                CHAR UserName[CREDUI_MAX_USERNAME_LENGTH + 1] = { 0 };
                CHAR Domain[CREDUI_MAX_DOMAIN_TARGET_LENGTH + 1] = { 0 };
                
                ConvertWCharNToUtf8(UserNameW, ARRAYSIZE(UserNameW), UserName, ARRAYSIZE(UserName));
                ConvertWCharNToUtf8(UserW, ARRAYSIZE(UserW), User, ARRAYSIZE(User));
                ConvertWCharNToUtf8(DomainW, ARRAYSIZE(DomainW), Domain, ARRAYSIZE(Domain));
                qCritical(log,
                          "Failed to parse UserName: %s into User: %s Domain: %s",
                          UserName, User, Domain);
                return FALSE;
            }
        }
    }
    
    *username = ConvertWCharNToUtf8Alloc(UserW, ARRAYSIZE(UserW), NULL);
    if (!(*username))
    {
        qCritical(log) << "ConvertWCharNToUtf8Alloc failed" << status;
        return FALSE;
    }
    
    if (_wcsnlen(DomainW, ARRAYSIZE(DomainW)) > 0)
        *domain = ConvertWCharNToUtf8Alloc(DomainW, ARRAYSIZE(DomainW), NULL);
    else
        *domain = _strdup("\0");
    
    if (!(*domain))
    {
        free(*username);
        qCritical(log) << "strdup failed" << status;
        return FALSE;
    }
    
    *password = ConvertWCharNToUtf8Alloc(PasswordW, ARRAYSIZE(PasswordW), NULL);
    if (!(*password))
    {
        free(*username);
        free(*domain);
        return FALSE;
    }
    return TRUE;
#else
    return cb_authenticate(instance, username, password, domain);
#endif //#ifdef Q_OS_WINDOWS
}

//TODO: to be continue!!!
BOOL CBackendFreeRDP::cb_choose_smartcard(freerdp* instance,
                         SmartcardCertInfo** cert_list,
                         DWORD count,
                         DWORD* choice, BOOL gateway)
{
    rdpContext* pContext = (rdpContext*)instance->context;
    CBackendFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    QString msg("Multiple smartcards are available for use:\n");
    for (DWORD i = 0; i < count; i++)
    {
        const SmartcardCertInfo* cert = cert_list[i];
        char* reader = ConvertWCharToUtf8Alloc(cert->reader, NULL);
        char* container_name = ConvertWCharToUtf8Alloc(cert->containerName, NULL);

        msg += QString::number(i) + " ";
        msg += QString(container_name) + "\n\t";
        msg += "Reader: " + QString(reader) + "\n\t";
        msg += "User: " + QString(cert->userHint) + + "@" + QString(cert->domainHint) + "\n\t";
        msg += "Subject: " + QString(cert->subject) + "\n\t";
        msg += "Issuer: " + QString(cert->issuer) + "\n\t";
        msg += "UPN: " + QString(cert->upn) + "\n";
        
        free(reader);
        free(container_name);
    }
    
    msg += "\nChoose a smartcard to use for ";
    if(gateway)
        msg += "gateway authentication";
    else
        msg += "logon";
    
    msg += "(0 - " + QString::number(count - 1) + ")";
    
    QString num;
    emit pThis->sigBlockInputDialog(tr("Choose"), tr("Please choose smartcard"),
                                    msg, num);
    if(!num.isEmpty())
    {
        bool ok = false;
        int n = num.toInt(&ok);
        if(ok)
        {
            *choice = n;
            return TRUE;
        }
    }
    return FALSE;
}

#ifdef WITH_WINDOWS_CERT_STORE
/* https://stackoverflow.com/questions/1231178/load-an-pem-encoded-x-509-certificate-into-windows-cryptoapi/3803333#3803333
 */
/* https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/security/cryptoapi/peertrust/cpp/peertrust.cpp
 */
/* https://stackoverflow.com/questions/7340504/whats-the-correct-way-to-verify-an-ssl-certificate-in-win32
 */

static void wf_report_error(char* wszMessage, DWORD dwErrCode)
{
    LPSTR pwszMsgBuf = NULL;
    
    if (NULL != wszMessage && 0 != *wszMessage)
    {
        WLog_ERR(TAG, "%s", wszMessage);
    }
    
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL,                                      // Location of message
                   //  definition ignored
                   dwErrCode,                                 // Message identifier for
                   //  the requested message
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Language identifier for
                   //  the requested message
                   (LPSTR)&pwszMsgBuf,                        // Buffer that receives
                   //  the formatted message
                   0,                                         // Size of output buffer
                   //  not needed as allocate
                   //  buffer flag is set
                   NULL                                       // Array of insert values
                   );
    
    if (NULL != pwszMsgBuf)
    {
        WLog_ERR(TAG, "Error: 0x%08x (%d) %s", dwErrCode, dwErrCode, pwszMsgBuf);
        LocalFree(pwszMsgBuf);
    }
    else
    {
        WLog_ERR(TAG, "Error: 0x%08x (%d)", dwErrCode, dwErrCode);
    }
}

static DWORD wf_is_x509_certificate_trusted(const char* common_name, const char* subject,
                                            const char* issuer, const char* fingerprint)
{
    HRESULT hr = CRYPT_E_NOT_FOUND;
    
    DWORD dwChainFlags = CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT;
    PCCERT_CONTEXT pCert = NULL;
    HCERTCHAINENGINE hChainEngine = NULL;
    PCCERT_CHAIN_CONTEXT pChainContext = NULL;
    
    CERT_ENHKEY_USAGE EnhkeyUsage = { 0 };
    CERT_USAGE_MATCH CertUsage = { 0 };
    CERT_CHAIN_PARA ChainPara = { 0 };
    CERT_CHAIN_POLICY_PARA ChainPolicy = { 0 };
    CERT_CHAIN_POLICY_STATUS PolicyStatus = { 0 };
    CERT_CHAIN_ENGINE_CONFIG EngineConfig = { 0 };
    
    DWORD derPubKeyLen = WINPR_ASSERTING_INT_CAST(uint32_t, strlen(fingerprint));
    char* derPubKey = calloc(derPubKeyLen, sizeof(char));
    if (NULL == derPubKey)
    {
        WLog_ERR(TAG, "Could not allocate derPubKey");
        goto CleanUp;
    }
    
    /*
	 * Convert from PEM format to DER format - removes header and footer and decodes from base64
	 */
    if (!CryptStringToBinaryA(fingerprint, 0, CRYPT_STRING_BASE64HEADER, derPubKey, &derPubKeyLen,
                              NULL, NULL))
    {
        WLog_ERR(TAG, "CryptStringToBinary failed. Err: %d", GetLastError());
        goto CleanUp;
    }
    
    //---------------------------------------------------------
    // Initialize data structures for chain building.
    
    EnhkeyUsage.cUsageIdentifier = 0;
    EnhkeyUsage.rgpszUsageIdentifier = NULL;
    
    CertUsage.dwType = USAGE_MATCH_TYPE_AND;
    CertUsage.Usage = EnhkeyUsage;
    
    ChainPara.cbSize = sizeof(ChainPara);
    ChainPara.RequestedUsage = CertUsage;
    
    ChainPolicy.cbSize = sizeof(ChainPolicy);
    
    PolicyStatus.cbSize = sizeof(PolicyStatus);
    
    EngineConfig.cbSize = sizeof(EngineConfig);
    EngineConfig.dwUrlRetrievalTimeout = 0;
    
    pCert = CertCreateCertificateContext(X509_ASN_ENCODING, derPubKey, derPubKeyLen);
    if (NULL == pCert)
    {
        WLog_ERR(TAG, "FAILED: Certificate could not be parsed.");
        goto CleanUp;
    }
    
    dwChainFlags |= CERT_CHAIN_ENABLE_PEER_TRUST;
    
    // When this flag is set, end entity certificates in the
    // Trusted People store are trusted without doing any chain building
    // This optimizes the chain building process.
    
    //---------------------------------------------------------
    // Create chain engine.
    
    if (!CertCreateCertificateChainEngine(&EngineConfig, &hChainEngine))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto CleanUp;
    }
    
    //-------------------------------------------------------------------
    // Build a chain using CertGetCertificateChain
    
    if (!CertGetCertificateChain(hChainEngine, // use the default chain engine
                                 pCert,        // pointer to the end certificate
                                 NULL,         // use the default time
                                 NULL,         // search no additional stores
                                 &ChainPara,   // use AND logic and enhanced key usage
                                 //  as indicated in the ChainPara
                                 //  data structure
                                 dwChainFlags,
                                 NULL,            // currently reserved
                                 &pChainContext)) // return a pointer to the chain created
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto CleanUp;
    }
    
    //---------------------------------------------------------------
    // Verify that the chain complies with policy
    
    if (!CertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_BASE, // use the base policy
                                          pChainContext,          // pointer to the chain
                                          &ChainPolicy,
                                          &PolicyStatus)) // return a pointer to the policy status
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto CleanUp;
    }
    
    if (PolicyStatus.dwError != S_OK)
    {
        wf_report_error("CertVerifyCertificateChainPolicy: Chain Status", PolicyStatus.dwError);
        hr = PolicyStatus.dwError;
        // Instruction: If the PolicyStatus.dwError is CRYPT_E_NO_REVOCATION_CHECK or
        // CRYPT_E_REVOCATION_OFFLINE, it indicates errors in obtaining
        //				revocation information. These can be ignored since the retrieval of
        // revocation information depends on network availability
        
        if (PolicyStatus.dwError == CRYPT_E_NO_REVOCATION_CHECK ||
            PolicyStatus.dwError == CRYPT_E_REVOCATION_OFFLINE)
        {
            hr = S_OK;
        }
        
        goto CleanUp;
    }
    
    WLog_INFO(TAG, "CertVerifyCertificateChainPolicy succeeded for %s (%s) issued by %s",
              common_name, subject, issuer);
    
    hr = S_OK;
CleanUp:
    
    if (FAILED(hr))
    {
        WLog_INFO(TAG, "CertVerifyCertificateChainPolicy failed for %s (%s) issued by %s",
                  common_name, subject, issuer);
        wf_report_error(NULL, hr);
    }
    
    free(derPubKey);
    
    if (NULL != pChainContext)
    {
        CertFreeCertificateChain(pChainContext);
    }
    
    if (NULL != hChainEngine)
    {
        CertFreeCertificateChainEngine(hChainEngine);
    }
    
    if (NULL != pCert)
    {
        CertFreeCertificateContext(pCert);
    }
    
    return (DWORD)hr;
}
#endif

#endif //#if FREERDP_VERSION_MAJOR >= 3

BOOL CBackendFreeRDP::cb_authenticate(freerdp* instance, char** username,
                                      char** password, char** domain)
{
    qDebug(log) << Q_FUNC_INFO;
	if(!instance)
		return FALSE;
    rdpContext* pContext = (rdpContext*)instance->context;
    CBackendFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    if(!username || !password || !domain) return FALSE;
    if(*username && *password ) return TRUE;

    int nRet = QDialog::Rejected;
    emit pThis->sigBlockShowWidget("CDlgGetUserPasswordFreeRDP",
                                   nRet, pThis->m_pParameter);
    if(QDialog::Accepted == nRet)
    {
        QString szPassword = pThis->m_pParameter->m_Net.m_User.GetPassword();
        QString szName = pThis->m_pParameter->m_Net.m_User.GetUser();
        QString szDomain = pThis->m_pParameter->GetDomain();
        if(!szDomain.isEmpty() && domain)
            *domain = _strdup(szDomain.toStdString().c_str());
        if(!szName.isEmpty() && username)
            *username = _strdup(szName.toStdString().c_str());
        if(!szPassword.isEmpty() && password)
            *password = _strdup(szPassword.toStdString().c_str());
    } else
        return FALSE;

    return TRUE;
}

BOOL CBackendFreeRDP::cb_GatewayAuthenticate(freerdp *instance,
                                char **username, char **password, char **domain)
{
    qDebug(log) << Q_FUNC_INFO;
	if(!instance)
		return FALSE;

    rdpContext* pContext = (rdpContext*)instance->context;
    CBackendFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    if(!username || !password || !domain) return FALSE;
    if(*username && *password ) return TRUE;
    
    int nRet = QDialog::Rejected;
    emit pThis->sigBlockShowWidget("CDlgGetUserPasswordFreeRDP", nRet, pThis->m_pParameter);
    if(QDialog::Accepted == nRet)
    {
        QString szPassword = pThis->m_pParameter->m_Net.m_User.GetPassword();
        QString szName = pThis->m_pParameter->m_Net.m_User.GetUser();
        QString szDomain = pThis->m_pParameter->GetDomain();
        if(!szDomain.isEmpty() && domain)
            *domain = _strdup(szDomain.toStdString().c_str());
        if(!szName.isEmpty() && username)
            *username = _strdup(szName.toStdString().c_str());
        if(!szPassword.isEmpty() && password)
            *password = _strdup(szPassword.toStdString().c_str());
    } else
        return FALSE;

	return TRUE;
}

int CBackendFreeRDP::cb_verify_x509_certificate(freerdp* instance,
                                               const BYTE* data, size_t length,
                                 const char* hostname, UINT16 port, DWORD flags)
{
    qDebug(log) << Q_FUNC_INFO;
    rdpContext* pContext = (rdpContext*)instance->context;
    QSslCertificate cert(QByteArray((const char*)data, length));
#if FREERDP_VERSION_MAJOR >= 3
    /* Newer versions of FreeRDP allow exposing the whole PEM by setting
	 * FreeRDP_CertificateCallbackPreferPEM to TRUE
	 */
    if (flags & VERIFY_CERT_FLAG_FP_IS_PEM) {
        return cb_verify_certificate_ex(
            instance, hostname, port,
            cert.issuerDisplayName().toStdString().c_str(),
            cert.subjectDisplayName().toStdString().c_str(),
            cert.issuerDisplayName().toStdString().c_str(),
            (const char*)data,
            flags);
    } else
#endif
    return cb_verify_certificate_ex(
        instance, hostname, port,
        cert.issuerDisplayName().toStdString().c_str(),
        cert.subjectDisplayName().toStdString().c_str(),
        cert.issuerDisplayName().toStdString().c_str(),
        cert.serialNumber().toStdString().c_str(),
        flags);
}

static QString pem_cert_fingerprint(const char* pem, DWORD flags)
{
    QString szFingerPrint;
#if FREERDP_VERSION_MAJOR >= 3
    /* Newer versions of FreeRDP allow exposing the whole PEM by setting
	 * FreeRDP_CertificateCallbackPreferPEM to TRUE
	 */
    if (flags & VERIFY_CERT_FLAG_FP_IS_PEM)
    {        
        rdpCertificate* cert = freerdp_certificate_new_from_pem(pem);
        if (!cert)
            return NULL;

        char* fp = freerdp_certificate_get_fingerprint(cert);
        char* start = freerdp_certificate_get_validity(cert, TRUE);
        char* end = freerdp_certificate_get_validity(cert, FALSE);
        freerdp_certificate_free(cert);

        szFingerPrint = QObject::tr("Valid from: ") + QString(start) + "\n";
        szFingerPrint += QObject::tr("Valid to: ") + QString(end) + "\n";
        szFingerPrint += QObject::tr("Fingerprint: ") + QString(fp) + "\n";

        free(fp);
        free(start);
        free(end);
    } else
#endif
        szFingerPrint = QObject::tr("Fingerprint: ") + QString(pem) + "\n";
    return szFingerPrint;
}

/** Callback set in the rdp_freerdp structure, and used to make a certificate validation
 *  when the connection requires it.
 *  This function will actually be called by tls_verify_certificate().
 *  @see rdp_client_connect() and tls_connect()
 *  @param instance     pointer to the rdp_freerdp structure that contains the connection settings
 *  @param host         The host currently connecting to
 *  @param port         The port currently connecting to
 *  @param common_name  The common name of the certificate, should match host or an alias of it
 *  @param subject      The subject of the certificate
 *  @param issuer       The certificate issuer name
 *  @param fingerprint  The fingerprint of the certificate
 *  @param flags        See VERIFY_CERT_FLAG_* for possible values.
 *
 *  @return 1 if the certificate is trusted, 2 if temporary trusted, 0 otherwise.
 */
DWORD CBackendFreeRDP::cb_verify_certificate_ex(freerdp *instance,
                       const char *host, UINT16 port,
                       const char *common_name, const char *subject,
                       const char *issuer, const char *fingerprint, DWORD flags)
{
    qDebug(log) << Q_FUNC_INFO;

    rdpContext* pContext = (rdpContext*)instance->context;
    Q_ASSERT(pContext);
    CBackendFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    Q_ASSERT(pThis);
    if(common_name)
    {
        //pThis->m_pParameter->SetServerName(common_name);
        emit pThis->sigServerName(common_name);
    }

    if(!pThis->m_pParameter->GetShowVerifyDiaglog()) {
        /* return 1 to accept and store a certificate, 2 to accept
         * a certificate only for this session, 0 otherwise */
        return 2;
    }

#if FREERDP_VERSION_MAJOR >= 3
#if defined(Q_OS_WIN) && defined(WITH_WINDOWS_CERT_STORE)
    if (flags & VERIFY_CERT_FLAG_FP_IS_PEM && !(flags & VERIFY_CERT_FLAG_MISMATCH))
    {
        if (wf_is_x509_certificate_trusted(common_name, subject, issuer, fingerprint) == S_OK)
        {
            return 2;
        }
    }
#endif
#endif

    QString szType = tr("RDP-Server");
    if (flags & VERIFY_CERT_FLAG_GATEWAY)
        szType = tr("RDP-Gateway");
    if (flags & VERIFY_CERT_FLAG_REDIRECT)
        szType = tr("RDP-Redirect");

    QString title(tr("Verify certificate"));
    QString message;

    message += szType + tr(": %1:%2").arg(host, QString::number(port)) + "\n";
    message += tr("Common name: ") + common_name + "\n";
    message += tr("Subject: ") + subject + "\n";
    message += tr("Issuer: ") + issuer + "\n";
    message += pem_cert_fingerprint(fingerprint, flags);
    message += "\n";
    if(VERIFY_CERT_FLAG_CHANGED & flags) {
        message += tr("The above X.509 certificate is changed.\n"
                  "It is possible that the server has changed its certificate, "
                  "or Maybe it was attacked."
                  "Please look at the OpenSSL documentation on "
                  "how to add a private CA to the store.");
    } else {
        message += tr("The above X.509 certificate could not be verified.\n"
                  "Possibly because you do not have the CA certificate "
                  "in your certificate store, or the certificate has expired.\n"
                  "Please look at the OpenSSL documentation on "
                  "how to add a private CA to the store.");
    }
    message += "\n";
    message += "\n";
    message += tr("Yes - trusted") + "\n";
    message += tr("Ignore - temporary trusted") + "\n";
    message += tr("No - no trusted") + "\n";

    QMessageBox::StandardButton nRet = QMessageBox::StandardButton::No;
    QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::Ignore | QMessageBox::No;
    bool bCheckBox = false;
    emit pThis->sigBlockShowMessageBox(title, message, buttons, nRet, bCheckBox,
                                    tr("Don't show again"));
    pThis->m_pParameter->SetShowVerifyDiaglog(!bCheckBox);
    emit pThis->m_pParameter->sigChanged();
    /* return 1 to accept and store a certificate, 2 to accept
	 * a certificate only for this session, 0 otherwise */
    switch(nRet)
    {
    case QMessageBox::StandardButton::Yes:
        return 1;
    case QMessageBox::StandardButton::Ignore:
        return 2;
    default:
        return 0;
    }
    return 2;
}

/** Callback set in the rdp_freerdp structure, and used to make a certificate validation
 *  when a stored certificate does not match the remote counterpart.
 *  This function will actually be called by tls_verify_certificate().
 *  @see rdp_client_connect() and tls_connect()
 *  @param instance        pointer to the rdp_freerdp structure that contains the connection
 * settings
 *  @param host            The host currently connecting to
 *  @param port            The port currently connecting to
 *  @param common_name     The common name of the certificate, should match host or an alias of it
 *  @param subject         The subject of the certificate
 *  @param issuer          The certificate issuer name
 *  @param fingerprint     The fingerprint of the certificate
 *  @param old_subject     The subject of the previous certificate
 *  @param old_issuer      The previous certificate issuer name
 *  @param old_fingerprint The fingerprint of the previous certificate
 *  @param flags           See VERIFY_CERT_FLAG_* for possible values.
 *
 *  @return 1 if the certificate is trusted, 2 if temporary trusted, 0 otherwise.
 */
DWORD CBackendFreeRDP::cb_verify_changed_certificate_ex(freerdp *instance,
                      const char *host, UINT16 port,
                      const char *common_name, const char *subject,
                      const char *issuer, const char *fingerprint,
                      const char *old_subject, const char *old_issuer,
                      const char *old_fingerprint, DWORD flags)
{
    qDebug(log) << Q_FUNC_INFO;
    rdpContext* pContext = (rdpContext*)instance->context;
    CBackendFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    if(common_name)
        emit pThis->sigServerName(common_name);

    if(!pThis->m_pParameter->GetShowVerifyDiaglog()) {
        /* return 1 to accept and store a certificate, 2 to accept
         * a certificate only for this session, 0 otherwise */
        return 2;
    }

    QString szType = tr("RDP-Server");
    if (flags & VERIFY_CERT_FLAG_GATEWAY)
        szType = tr("RDP-Gateway");
    if (flags & VERIFY_CERT_FLAG_REDIRECT)
        szType = tr("RDP-Redirect");

    QString title(tr("Verify changed certificate"));
    QString message;
    message += szType + tr(": %1:%2").arg(host, QString::number(port)) + "\n";
    message += tr("New Certificate details:") + "\n";
    message += "  " + tr("name: ") + common_name + "\n";
    message += "  " + tr("subject: ") + subject + "\n";
    message += "  " + tr("issuer: ") + issuer + "\n";
    message += "  " + pem_cert_fingerprint(fingerprint, flags) + "\n";
    message += tr("Old Certificate details:") + "\n";
    message += "  " + tr("subject: ") + old_subject + "\n";
    message += "  " + tr("issuer: ") + old_issuer + "\n";
    message += "  " + pem_cert_fingerprint(old_fingerprint, flags) + "\n";
    message += "\n";
    message += tr("The above X.509 certificate could not be verified, "
                  "possibly because you do not have the CA certificate "
                  "in your certificate store, or the certificate has expired. "
                  "Please look at the OpenSSL documentation on "
                  "how to add a private CA to the store.");
    message += "\n";
    message += "\n";
    message += tr("Yes - trusted") + "\n";
    message += tr("Ignore - temporary trusted") + "\n";
    message += tr("No - no trusted") + "\n";

    bool bCheckBox = false;
    QMessageBox::StandardButton nRet = QMessageBox::StandardButton::No;
    QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::Ignore | QMessageBox::No;
    emit pThis->sigBlockShowMessageBox(title, message, buttons, nRet, bCheckBox,
                                    tr("Don't show again"));
    pThis->m_pParameter->SetShowVerifyDiaglog(!bCheckBox);
    emit pThis->m_pParameter->sigChanged();

    /* return 1 to accept and store a certificate, 2 to accept
         * a certificate only for this session, 0 otherwise */
    switch(nRet)
    {
    case QMessageBox::StandardButton::Yes:
        return 1;
    case QMessageBox::StandardButton::Ignore:
        return 2;
    default:
        return 0;
    }

    /* return 1 to accept and store a certificate, 2 to accept
     * a certificate only for this session, 0 otherwise */
    return 2;
}

BOOL CBackendFreeRDP::cb_present_gateway_message(
        freerdp* instance, UINT32 type, BOOL isDisplayMandatory,
        BOOL isConsentMandatory, size_t length, const WCHAR* message)
{
    qDebug(log) << Q_FUNC_INFO;

    if (!isDisplayMandatory && !isConsentMandatory)
        return TRUE;

    /* special handling for consent messages (show modal dialog) */
    if (type == GATEWAY_MESSAGE_CONSENT && isConsentMandatory)
    {
        QString msgType = (type == GATEWAY_MESSAGE_CONSENT)
                              ? tr("Consent message") : tr("Service message");
        msgType += "\n";
#if FREERDP_VERSION_MAJOR >= 3
        char* pMsg = ConvertWCharToUtf8Alloc(message, NULL);
        if(pMsg) {
            msgType += pMsg;
            free(pMsg);
        }
#else
        msgType += QString::fromStdWString((wchar_t*)message);
#endif
        msgType += "\n";
        msgType += tr("I understand and agree to the terms of this policy (Y/N)");

        rdpContext* pContext = (rdpContext*)instance->context;
        CBackendFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
        QMessageBox::StandardButton nRet = QMessageBox::No;
        bool bCheckBox = false;
        emit pThis->sigBlockShowMessageBox(tr("Gateway message"), msgType,
                                        QMessageBox::Yes|QMessageBox::No,
                                        nRet, bCheckBox);
        switch (nRet) {
        case QMessageBox::Yes:
            return TRUE;
            break;
        default:
            return FALSE;
        }
    }
    else
        return client_cli_present_gateway_message(
            instance, type, isDisplayMandatory,
            isConsentMandatory, length, message);

    return TRUE;
}

BOOL CBackendFreeRDP::cb_begin_paint(rdpContext *context)
{
    HGDI_DC hdc;

    if (!context || !context->gdi || !context->gdi->primary
            || !context->gdi->primary->hdc)
		return FALSE;

    hdc = context->gdi->primary->hdc;

	if (!hdc || !hdc->hwnd || !hdc->hwnd->invalid)
		return FALSE;

	hdc->hwnd->invalid->null = TRUE;
	hdc->hwnd->ninvalid = 0;
	return TRUE;
}

BOOL CBackendFreeRDP::UpdateBuffer(INT32 x, INT32 y, INT32 w, INT32 h)
{
    if(x > m_Image.width() || y > m_Image.height()) {
        qCritical(log) << "The width and height out of range."
                       << "Image width:" << m_Image.width()
                       << "Image height:" << m_Image.height()
                       << "w:" << w << "h:" << h;
        return FALSE;
    }

    QRect rect(x, y, w, h);
    QImage img = m_Image.copy(rect);
    //qDebug(log) << "Image:" << rect << img.rect() << img;
    emit sigUpdateRect(rect, img);
    return TRUE;
}

BOOL CBackendFreeRDP::cb_end_paint(rdpContext *context)
{
    //qDebug(log) << Q_FUNC_INFO;
    ClientContext* pContext = (ClientContext*)context;
    CBackendFreeRDP* pThis = pContext->pThis;
    INT32 ninvalid;
    HGDI_RGN cinvalid;
    REGION16 invalidRegion;
    RECTANGLE_16 invalidRect;
    const RECTANGLE_16* extents;
    HGDI_DC hdc;
    int i = 0;

    if (!context || !context->gdi || !context->gdi->primary
            || !context->gdi->primary->hdc)
		return FALSE;

    hdc = context->gdi->primary->hdc;
    
    if (!hdc || !hdc->hwnd || !hdc->hwnd->invalid)
		return FALSE;

    rdpGdi* gdi = context->gdi;
    if (gdi->suppressOutput)
        return TRUE;

    HGDI_WND hwnd = context->gdi->primary->hdc->hwnd;
    ninvalid = hwnd->ninvalid; //无效区数量
	cinvalid = hwnd->cinvalid; //无效区数组
    if (ninvalid < 1)
		return TRUE;

    region16_init(&invalidRegion);

	for (i = 0; i < ninvalid; i++)
	{
        if(cinvalid[i].null)
        {
            qWarning(log) << "is null region" << cinvalid[i].x << cinvalid[i].y
                          << cinvalid[i].w << cinvalid[i].h;
            continue;
        }
		invalidRect.left = cinvalid[i].x;
		invalidRect.top = cinvalid[i].y;
		invalidRect.right = cinvalid[i].x + cinvalid[i].w;
		invalidRect.bottom = cinvalid[i].y + cinvalid[i].h;
		region16_union_rect(&invalidRegion, &invalidRegion, &invalidRect);
	}

	if (!region16_is_empty(&invalidRegion))
	{
		extents = region16_extents(&invalidRegion);
        //qDebug(log) << extents->left << extents->top << extents->right << extents->bottom;
        pThis->UpdateBuffer(extents->left,
                            extents->top,
                            extents->right - extents->left,
                            extents->bottom - extents->top);
    }

	region16_uninit(&invalidRegion);

    return TRUE;
}

BOOL CBackendFreeRDP::cb_desktop_resize(rdpContext* context)
{
    qDebug(log) << Q_FUNC_INFO;
    ClientContext* pContext = (ClientContext*)context;
    CBackendFreeRDP* pThis = pContext->pThis;
    rdpSettings* settings;
    if (!context || !context->settings)
		return FALSE;
    settings = context->settings;
    int desktopWidth = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    int desktopHeight = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);

    if(!gdi_resize(context->gdi, desktopWidth, desktopHeight))
        return FALSE;
    if(!pThis->CreateImage(context))
        return FALSE;

    emit pThis->sigSetDesktopSize(desktopWidth, desktopHeight);
    pThis->UpdateBuffer(0, 0, desktopWidth, desktopHeight);
    return TRUE;
}

BOOL CBackendFreeRDP::cb_play_bell_sound(rdpContext *context, const PLAY_SOUND_UPDATE *play_sound)
{
    qDebug(log) << Q_FUNC_INFO;
    ClientContext* pContext = (ClientContext*)context;
    CBackendFreeRDP* pThis = pContext->pThis;
	WINPR_UNUSED(play_sound);
    QApplication::beep();
    return TRUE;

    QString szFile;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QSoundEffect effect;
    effect.setSource(QUrl::fromLocalFile(szFile));
//    effect.setLoopCount(1);
//    effect.setVolume(1);
    effect.play();
#else
    QSound::play(szFile);
#endif
	return TRUE;
}

/* This function is called to update the keyboard indicator LED */
BOOL CBackendFreeRDP::cb_keyboard_set_indicators(rdpContext *context, UINT16 led_flags)
{
    qDebug(log) << Q_FUNC_INFO;
    ClientContext* pContext = (ClientContext*)context;
    CBackendFreeRDP* pThis = pContext->pThis;
    
    int state = CFrmViewer::LED_STATE::Unknown;

    if (led_flags & KBD_SYNC_NUM_LOCK)
        state |= CFrmViewer::LED_STATE::NumLock;
    if (led_flags & KBD_SYNC_CAPS_LOCK)
        state |= CFrmViewer::LED_STATE::CapsLock;
    if (led_flags & KBD_SYNC_SCROLL_LOCK)
        state |= CFrmViewer::LED_STATE::ScrollLock;
    
    emit pThis->sigUpdateLedState(state);

    return TRUE;
}

/* This function is called to set the IME state */
BOOL CBackendFreeRDP::cb_keyboard_set_ime_status(
    rdpContext* context, UINT16 imeId, UINT32 imeState, UINT32 imeConvMode)
{
    if (!context)
        return FALSE;
    
    qWarning(log,
              "KeyboardSetImeStatus(unitId=%04" PRIx16 ", imeState=%08" PRIx32
              ", imeConvMode=%08" PRIx32 ") ignored",
              imeId, imeState, imeConvMode);
    return TRUE;
}

int CBackendFreeRDP::WakeUp()
{
    //qDebug(log) << Q_FUNC_INFO;
    if(m_writeEvent)
        SetEvent(m_writeEvent);
    return 0;
}

// https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/2c1ced34-340a-46cd-be6e-fc8cab7c3b17
bool CBackendFreeRDP::SendMouseEvent(UINT16 flags, QPoint pos, bool isExtended)
{
    if(m_pParameter && m_pParameter->GetOnlyView()) return true;
    if(!m_pContext) return false;

#if FREERDP_VERSION_MAJOR >= 3
    if(isExtended)
        freerdp_client_send_extended_button_event(
            &m_pContext->Context, FALSE, flags, pos.x(), pos.y());
    else
        freerdp_client_send_button_event(
            &m_pContext->Context, FALSE, flags, pos.x(), pos.y());
#else
    if(!m_pContext->Context.input) return false;
    return freerdp_input_send_mouse_event(
        m_pContext->Context.input, flags, pos.x(), pos.y());
#endif
    return true;
}

void CBackendFreeRDP::wheelEvent(QWheelEvent *event)
{
    qDebug(logMouse) << Q_FUNC_INFO << event;
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
   
    UINT16 flags = 0;
    QPointF pos;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    pos = event->position();
#else
    pos = event->pos();
#endif
    QPoint p = event->angleDelta();
    if(p.y() > 0)
    {
        flags |= PTR_FLAGS_WHEEL | p.y();
    }
    if(p.y() < 0)
    {
        flags |= PTR_FLAGS_WHEEL | PTR_FLAGS_WHEEL_NEGATIVE | -p.y();
    }
    
    if(p.x() < 0)
    {
        flags |= PTR_FLAGS_HWHEEL | PTR_FLAGS_WHEEL_NEGATIVE | -p.x();  
    }
    if(p.x() > 0)
    {
        flags |= PTR_FLAGS_HWHEEL | p.x();
    }
#if FREERDP_VERSION_MAJOR >= 3
    freerdp_client_send_wheel_event(&m_pContext->Context, flags);
    /*
    freerdp_client_send_button_event(
        &m_pContext->Context, FALSE, flags, pos.x(), pos.y());//*/
#else
    freerdp_input_send_mouse_event(
        m_pContext->Context.input, flags, pos.x(), pos.y());
#endif
}

// https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/2c1ced34-340a-46cd-be6e-fc8cab7c3b17
void CBackendFreeRDP::mouseMoveEvent(QMouseEvent *event)
{
    qDebug(logMouse) << Q_FUNC_INFO << event << event->buttons() << event->button();
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    UINT16 flags = PTR_FLAGS_MOVE;
    SendMouseEvent(flags, event->pos(), false);
}

void CBackendFreeRDP::mousePressEvent(QMouseEvent *event)
{
    qDebug(logMouse) << Q_FUNC_INFO << event << event->buttons() << event->button();
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;

    UINT16 flags = 0;
    bool isExtended = false;
    Qt::MouseButton button = event->button();
    if (button & Qt::MouseButton::LeftButton)
    {
        flags = PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON1;
    }
    else if (button & Qt::MouseButton::RightButton)
    {
        flags = PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON2;
    }
    else if (button & Qt::MouseButton::MiddleButton)
    {
        flags = PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON3;
    }
    else if (button & Qt::MouseButton::ForwardButton)
    {
        flags = PTR_XFLAGS_DOWN | PTR_XFLAGS_BUTTON2;
        isExtended = true;
    }
    else if (button & Qt::MouseButton::BackButton)
    {
        flags = PTR_XFLAGS_DOWN | PTR_XFLAGS_BUTTON1;
        isExtended = true;
    }
    if (flags != 0)
    {
        SendMouseEvent(flags, event->pos(), isExtended);
    }
}

void CBackendFreeRDP::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug(logMouse) << Q_FUNC_INFO << event << event->buttons() << event->button();
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;

    UINT16 flags = 0;
    bool isExtended = false;
    Qt::MouseButton button = event->button();
    if (button & Qt::MouseButton::LeftButton)
    {
        flags = PTR_FLAGS_BUTTON1;
    }
    else if (button & Qt::MouseButton::MiddleButton)
    {
        flags = PTR_FLAGS_BUTTON3;
    }
    else if (button & Qt::MouseButton::RightButton)
    {
        flags = PTR_FLAGS_BUTTON2;
    }
    else if (button & Qt::MouseButton::ForwardButton)
    {
        flags = PTR_XFLAGS_BUTTON2;
        isExtended = true;
    }
    else if (button & Qt::MouseButton::BackButton)
    {
        flags = PTR_XFLAGS_BUTTON1;
        isExtended = true;
    }
    if (flags != 0)
    {
        SendMouseEvent(flags, event->pos(), isExtended);
    }
}

void CBackendFreeRDP::keyPressEvent(QKeyEvent *event)
{
    qDebug(logKey) << Q_FUNC_INFO << event;
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    // Convert to rdp scan code freerdp/scancode.h
    UINT32 k = CConvertKeyCode::QtToScanCode(event->key(), event->modifiers());
    if(RDP_SCANCODE_UNKNOWN != k)
#if FREERDP_VERSION_MAJOR >= 3
        freerdp_input_send_keyboard_event_ex(
            m_pContext->Context.context.input, true, true, k);
#else
        freerdp_input_send_keyboard_event_ex(
            m_pContext->Context.input, true, k);
#endif
}

void CBackendFreeRDP::keyReleaseEvent(QKeyEvent *event)
{
    qDebug(logKey) << Q_FUNC_INFO << event;
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    UINT32 k = CConvertKeyCode::QtToScanCode(event->key(), event->modifiers());
    if(RDP_SCANCODE_UNKNOWN != k)
#if FREERDP_VERSION_MAJOR >= 3
        freerdp_input_send_keyboard_event_ex(
            m_pContext->Context.context.input, false, false, k);
#else
        freerdp_input_send_keyboard_event_ex(
            m_pContext->Context.input, false, k);
#endif
}

void CBackendFreeRDP::InputMethodEvent(QInputMethodEvent *event)
{
    qDebug(logKey) << Q_FUNC_INFO << event;
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    QString szText = event->commitString();
    if(szText.isEmpty())
        return;
    for(int i = 0; i < szText.length(); i++) {
        QChar c = szText.at(i);
#if FREERDP_VERSION_MAJOR >= 3
        freerdp_input_send_unicode_keyboard_event(m_pContext->Context.context.input, 0, (UINT16)c.unicode());
#else
        freerdp_input_send_unicode_keyboard_event(m_pContext->Context.input, 0, (UINT16)c.unicode());
#endif
    }
}

int CBackendFreeRDP::RedirectionSound()
{
    rdpContext* pRdpContext = (rdpContext*)m_pContext;
    freerdp* instance = freerdp_client_get_instance(pRdpContext);
    rdpSettings* settings = instance->context->settings;
    Q_ASSERT(settings);

    if(m_pParameter->GetRedirectionSound()
        == CParameterFreeRDP::RedirecionSoundType::Disable)
    {
        /* Disable sound */
		freerdp_settings_set_bool(settings, FreeRDP_AudioPlayback, FALSE);
		freerdp_settings_set_bool(settings, FreeRDP_RemoteConsoleAudio, FALSE);
        return 0;
    } else if(m_pParameter->GetRedirectionSound()
               == CParameterFreeRDP::RedirecionSoundType::Local)
    {
        freerdp_settings_set_bool(settings, FreeRDP_AudioPlayback, TRUE);
		freerdp_settings_set_bool(settings, FreeRDP_AudioCapture, TRUE);
    } else if(m_pParameter->GetRedirectionSound()
               == CParameterFreeRDP::RedirecionSoundType::Remote)
    {
        freerdp_settings_set_bool(settings, FreeRDP_RemoteConsoleAudio, TRUE);
        return 0;
    }
    
    // Sound:
    // rdpsnd channel parameters format see: rdpsnd_process_addin_args in FreeRDP/channels/rdpsnd/client/rdpsnd_main.c
    // rdpsnd devices see: rdpsnd_process_connect in FreeRDP/channels/rdpsnd/client/rdpsnd_main.c
    // Format ag: /rdpsnd:sys:oss,dev:1,format:1
    //
    size_t count = 0;
    union
    {
        char** p;
        const char** pc;
    } ptr;
    ptr.p = CommandLineParseCommaSeparatedValuesEx("rdpsnd",
              m_pParameter->GetRedirectionSoundParameters().toStdString().c_str(),
                                                   &count);
    BOOL status = freerdp_client_add_static_channel(settings, count,
                                                #if FREERDP_VERSION_MAJOR < 3
                                                    ptr.p
                                                #else
                                                    ptr.pc
                                                #endif
                                                    );
    if (status)
    {
        status = freerdp_client_add_dynamic_channel(settings, count,
                                            #if FREERDP_VERSION_MAJOR < 3
                                                    ptr.p
                                            #else
                                                    ptr.pc
                                            #endif
                                                    );
    }
    
    if(!status)
    {
        qCritical(log) << "Load rdpsnd fail";
        return -1;
    }
    
    return 0;
}

int CBackendFreeRDP::RedirectionMicrophone()
{
    if(m_pParameter->GetRedirectionSound()
        == CParameterFreeRDP::RedirecionSoundType::Remote)
        return 0;
    if(!m_pParameter->GetRedirectionMicrophone())
        return 0;

    rdpContext* pRdpContext = (rdpContext*)m_pContext;
    freerdp* instance = freerdp_client_get_instance(pRdpContext);

    rdpSettings* settings = instance->context->settings;
    Q_ASSERT(settings);

    freerdp_settings_set_bool(settings, FreeRDP_AudioCapture, TRUE);

    // Microphone:
    // Audin channel parameters format see: audin_process_addin_args in FreeRDP/channels/audin/client/audin_main.c
    // Audin channel devices see: audin_DVCPluginEntry in FreeRDP/channels/audin/client/audin_main.c
    size_t count = 0;
    union
    {
        char** p;
        const char** pc;
    } ptr;
    ptr.p = CommandLineParseCommaSeparatedValuesEx("audin",
         m_pParameter->GetRedirectionMicrophoneParameters().toStdString().c_str(),
                                                   &count);
    BOOL status = freerdp_client_add_dynamic_channel(settings, count,
                                                 #if FREERDP_VERSION_MAJOR < 3
                                                         ptr.p
                                                 #else
                                                         ptr.pc
                                                 #endif
                                                     );
    
    if(!status)
    {
        qCritical(log) << "Load audin fail";
        return -1;
    }
    
    return 0;
}

int CBackendFreeRDP::RedirectionDriver()
{
    QStringList lstDrives = m_pParameter->GetRedirectionDrives();
    if(lstDrives.isEmpty())
        return 0;

    rdpContext* pRdpContext = (rdpContext*)m_pContext;
    freerdp* instance = freerdp_client_get_instance(pRdpContext);
    rdpSettings* settings = instance->context->settings;
    Q_ASSERT(settings);

    foreach (auto drive, lstDrives) {
        // Format: /drive:name,path
        char* pDrive = _strdup(drive.toStdString().c_str());
        const char* argvDrive[] = {"drive", pDrive};
        int count = sizeof(argvDrive) / sizeof(const char*);
        BOOL status = freerdp_client_add_device_channel(settings, count,
                                                #if FREERDP_VERSION_MAJOR < 3
                                                    (char**)
                                                #endif
                                                        argvDrive);
        if(pDrive) free(pDrive);
        if(!status)
        {
            qCritical(log) << "Load drive fail";
            return -1;
        }
    }

    return 0;
}

int CBackendFreeRDP::RedirectionPrinter()
{
    if(!m_pParameter->GetRedirectionPrinter())
        return 0;

    rdpContext* pRdpContext = (rdpContext*)m_pContext;
    freerdp* instance = freerdp_client_get_instance(pRdpContext);
    rdpSettings* settings = instance->context->settings;
    Q_ASSERT(settings);
    // 获取系统的打印机列表，并在combobox中显示
    QStringList printerList = QPrinterInfo::availablePrinterNames();
    if(printerList.isEmpty())
    {
        qCritical(log) << "The printer is empty";
        return -1;
    }
    qDebug(log) << printerList;

    // Format: /printer:<device>,<driver>,[default]
    const char* argvPrinter[] = {"printer",  nullptr, nullptr};
    int count = sizeof(argvPrinter) / sizeof(const char*);
    BOOL status = freerdp_client_add_device_channel(settings, count,
                                                #if FREERDP_VERSION_MAJOR < 3
                                                    (char**)
                                                #endif
                                                    argvPrinter);
    if(!status) {
        qCritical(log) << "Load printer fail";
        return -2;
    }

    return 0;
}

int CBackendFreeRDP::RedirectionSerial()
{
    //TODO: FreeRDP don't support
    return 0;
    rdpContext* pRdpContext = (rdpContext*)m_pContext;
    freerdp* instance = freerdp_client_get_instance(pRdpContext);
    rdpSettings* settings = instance->context->settings;
    Q_ASSERT(settings);

    QList<QSerialPortInfo> lstSerial = QSerialPortInfo::availablePorts();

    int nNum = 1;
    foreach (auto serial, lstSerial) {
        // Format: /serial:<name>,<device>,[SerCx2|SerCx|Serial],[permissive]
        //     ag: /serial:COM1,/dev/ttyS0
        qDebug(log) << "systemLocation:" << serial.systemLocation()
                        << "portName:" << serial.portName()
                        << "serialNumber:" << serial.serialNumber();
        char* pSerial = _strdup(serial.systemLocation().toStdString().c_str());
        char* pName = _strdup(serial.portName().toStdString().c_str());
        const char* argvSerial[] = {"serial", pName, pSerial};
        int count = sizeof(argvSerial) / sizeof(const char*);
        BOOL status = freerdp_client_add_device_channel(settings, count,
                                                    #if FREERDP_VERSION_MAJOR < 3
                                                        (char**)
                                                    #endif
                                                        argvSerial);
        if(pSerial) free(pSerial);
        if(pName) free(pName);

        if(!status)
        {
            qCritical(log) << "Load drive fail";
            return -1;
        }
    }

    return 0;
}

void CBackendFreeRDP::slotConnectProxyServer(QString szHost, quint16 nPort)
{
    qDebug(log) << "Connect proxy server:" << szHost + ":" + QString::number(nPort);
    rdpContext* pContext = (rdpContext*)m_pContext;
    rdpSettings* settings = pContext->settings;
    if(!settings) {
        qCritical(log) << "settings is null";
    }

    freerdp_settings_set_string(
        settings, FreeRDP_ServerHostname,
        szHost.toStdString().c_str());
    freerdp_settings_set_uint32(
        settings, FreeRDP_ServerPort,
        nPort);

    int nRet = freerdp_client_start(pContext);
    if(nRet)
    {
        qCritical(log) << "freerdp_client_start fail";
    }
    qDebug(log) << "Connect proxy server:" << szHost + ":" + QString::number(nPort) << "end";
}

#if HAVE_LIBSSH
CBackendFreeRDP::OnInitReturnValue CBackendFreeRDP::InitSSHTunnelPipe()
{
    // Start ssh thread
    if(!m_pThreadSSH)
        m_pThreadSSH = new CSSHTunnelThread(
            &m_pParameter->m_Proxy.m_SSH, &m_pParameter->m_Net, this);
    if(!m_pThreadSSH)
        return OnInitReturnValue::Fail;
    bool check = connect(m_pThreadSSH, SIGNAL(sigServer(QString, quint16)),
                         this, SLOT(slotConnectProxyServer(QString, quint16)));
    Q_ASSERT(check);
    check = connect(m_pThreadSSH, SIGNAL(sigError(int,QString)),
                    this, SIGNAL(sigError(int,QString)));
    Q_ASSERT(check);
    check = connect(m_pThreadSSH, SIGNAL(sigStop()),
                    this, SIGNAL(sigStop()));
    Q_ASSERT(check);
    m_pThreadSSH->start();
    return OnInitReturnValue::UseOnProcess;
}

int CBackendFreeRDP::CleanSSHTunnelPipe()
{
    if(m_pThreadSSH)
    {
        m_pThreadSSH->Exit();
        m_pThreadSSH = nullptr;
    }
    return 0;
}
#endif // HAVE_LIBSSH
