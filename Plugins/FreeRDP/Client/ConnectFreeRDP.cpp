// Author: Kang Lin <kl222@126.com>
// https://learn.microsoft.com/zh-cn/windows-server/remote/remote-desktop-services/welcome-to-rds
// X.509 Public Key Certificates: https://learn.microsoft.com/zh-cn/windows/win32/seccertenroll/about-x-509-public-key-certificates
// Cryptography: https://learn.microsoft.com/zh-cn/windows/win32/seccrypto/cryptography-portal

#include "ConnectFreeRDP.h"

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

#include "RabbitCommonTools.h"
#include "ConvertKeyCode.h"

#include <memory.h>
#include <QDebug>
#include <QApplication>
#include <QScreen>
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

Q_LOGGING_CATEGORY(FreeRDPConnect, "FreeRDP.Connect")

CConnectFreeRDP::CConnectFreeRDP(CConnecterFreeRDP *pConnecter,
                                 QObject *parent)
    : CConnect(pConnecter, parent),
      m_pContext(nullptr),
      m_pParameter(nullptr),
      m_pConnecter(pConnecter),
      m_ClipBoard(this),
      m_Cursor(this)
{
    Q_ASSERT(pConnecter);
    m_pParameter = dynamic_cast<CParameterFreeRDP*>(pConnecter->GetParameter());
    Q_ASSERT(m_pParameter);
}

CConnectFreeRDP::~CConnectFreeRDP()
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::~CConnectFreeRdp()";
}

/*
 * \return 
 * \li < 0: error
 * \li = 0: Use OnProcess (non-Qt event loop)
 * \li > 0: Don't use OnProcess (qt event loop)
 */
int CConnectFreeRDP::OnInit()
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::OnInit()";
    int nRet = 0;

    ZeroMemory(&m_ClientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	m_ClientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
	m_ClientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
    m_ClientEntryPoints.settings = m_pParameter->m_pSettings;
    m_ClientEntryPoints.GlobalInit = OnGlobalInit;
	m_ClientEntryPoints.GlobalUninit = OnGlobalUninit;
	m_ClientEntryPoints.ContextSize = sizeof(ClientContext);
    m_ClientEntryPoints.ClientNew = OnClientNew;
    m_ClientEntryPoints.ClientFree = OnClientFree;
    m_ClientEntryPoints.ClientStart = [](rdpContext* context)->int {
        CConnectFreeRDP* pThis = ((ClientContext*)context)->pThis;
        if(!pThis) return -1;
        return pThis->OnClientStart(context);
    };
	m_ClientEntryPoints.ClientStop = [](rdpContext* context)->int {
        CConnectFreeRDP* pThis = ((ClientContext*)context)->pThis;
        if(!pThis) return -1;
        return pThis->OnClientStop(context);
    };

    rdpContext* p = freerdp_client_context_new(&m_ClientEntryPoints);
	if(p)
    {
        m_pContext = (ClientContext*)p;
        m_pContext->pThis = this;
    } else {
        qCritical(FreeRDPConnect) << "freerdp_client_context_new fail";
        return -1;
    }

    rdpContext* pRdpContext = (rdpContext*)m_pContext;

    //Load channel
    RedirectionSound();
    RedirectionMicrophone();
    RedirectionDrive();
    RedirectionPrinter();
    RedirectionSerial();

    if(m_pParameter->GetHost().isEmpty())
    {
        QString szErr;
        szErr = tr("The server is empty, please input it");
        qCritical(FreeRDPConnect) << szErr;
        emit sigShowMessage(tr("Error"), szErr, QMessageBox::Critical);
        emit sigError(nRet, szErr.toStdString().c_str());
        return -2;
    }
    nRet = freerdp_client_start(pRdpContext);
    if(nRet)
    {
        qCritical(FreeRDPConnect) << "freerdp_client_start fail";
        return -3;
    }

    return nRet;
}

int CConnectFreeRDP::OnClean()
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::OnClean()";
    int nRet = 0;
    if(m_pContext)
    {
        rdpContext* pRdpContext = (rdpContext*)m_pContext;
        if(!freerdp_disconnect(pRdpContext->instance))
            qCritical(FreeRDPConnect) << "freerdp_disconnect fail";

        if(freerdp_client_stop(pRdpContext))
            qCritical(FreeRDPConnect) << "freerdp_client_stop fail";

        freerdp_client_context_free(pRdpContext);
        m_pContext = nullptr;
    }
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
 *    \li  < 0: error or stop
 * \see slotTimeOut()
 */
int CConnectFreeRDP::OnProcess()
{
    int nRet = 0;
    HANDLE handles[64];
    rdpContext* pRdpContext = (rdpContext*)m_pContext;

    do {
        DWORD nCount = 0;
        nCount = freerdp_get_event_handles(pRdpContext, &handles[nCount],
                                              ARRAYSIZE(handles) - nCount);
        if (nCount == 0)
        {
            qCritical(FreeRDPConnect) << "freerdp_get_event_handles failed";
            nRet = -2;
            break;
        }

        DWORD waitStatus = WaitForMultipleObjects(nCount, handles, FALSE, 500);
        if (waitStatus == WAIT_FAILED)
        {
            qCritical(FreeRDPConnect) << "WaitForMultipleObjects: WAIT_FAILED";
            nRet = -3;
            break;
        }

        if(waitStatus == WAIT_TIMEOUT)
        {
            nRet = 0;
            break;
        }

        if (!freerdp_check_event_handles(pRdpContext))
        {
            nRet = -5;

            UINT32 err = freerdp_get_last_error(pRdpContext);
            QString szErr;
            szErr = "freerdp_check_event_handles[";
            szErr += QString::number(err);
            szErr += "]";
            szErr += freerdp_get_last_error_category(err);
            szErr += "-";
            szErr += freerdp_get_last_error_name(err);
            szErr += ":";
            szErr += freerdp_get_last_error_string(err);
            qCritical(FreeRDPConnect) << szErr;
            emit sigError(err, szErr);

            // Reconnect
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
            qCritical(FreeRDPConnect) << szErr;
            emit sigError(err, szErr);
        }

#if FreeRDP_VERSION_MAJOR >= 3
        if(freerdp_shall_disconnect_context(pRdpContext))
#else
        if(freerdp_shall_disconnect(pRdpContext->instance))
#endif
        {
            qCritical(FreeRDPConnect) << "Abort connect";
            nRet = -7;
        }
    } while(false);

    return nRet;
}

void CConnectFreeRDP::slotClipBoardChanged()
{
    qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotClipBoardChanged()";
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    if(m_pParameter->GetClipboard())
        m_ClipBoard.slotClipBoardChanged();
}

BOOL CConnectFreeRDP::OnGlobalInit()
{
	qDebug(FreeRDPConnect) << "CConnectFreeRdp::OnGlobalInit()";
    //freerdp_register_addin_provider(freerdp_channels_load_static_addin_entry, 0);
	return TRUE;
}

void CConnectFreeRDP::OnGlobalUninit()
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::OnGlobalUninit()";
}

BOOL CConnectFreeRDP::OnClientNew(freerdp *instance, rdpContext *context)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::OnClientNew()";
    instance->PreConnect = cb_pre_connect;
	instance->PostConnect = cb_post_connect;
	instance->PostDisconnect = cb_post_disconnect;

    // Because it is already set in the parameters
#if FreeRDP_VERSION_MAJOR < 3
    instance->Authenticate = cb_authenticate;
    instance->GatewayAuthenticate = cb_GatewayAuthenticate;
#else
    instance->AuthenticateEx = cb_authenticate_ex;
    instance->ChooseSmartcard = cb_choose_smartcard;
#endif
    //instance->VerifyX509Certificate = cb_verify_x509_certificate;
    instance->VerifyCertificateEx = cb_verify_certificate_ex;
	instance->VerifyChangedCertificateEx = cb_verify_changed_certificate_ex;
	instance->PresentGatewayMessage = cb_present_gateway_message;

	instance->LogonErrorInfo = cb_logon_error_info;

    return TRUE;
}

void CConnectFreeRDP::OnClientFree(freerdp *instance, rdpContext *context)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::OnClientFree()";
}

int CConnectFreeRDP::OnClientStart(rdpContext *context)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::OnClientStart()";
    int nRet = 0;
    freerdp* instance = freerdp_client_get_instance(context);
    BOOL status = freerdp_connect(instance);
    if (status) {
        QString szInfo = tr("Connect to ");
        szInfo += m_pParameter->GetHost();
        szInfo += ":";
        szInfo += QString::number(m_pParameter->GetPort());
        qInfo(FreeRDPConnect) << szInfo;
        emit sigInformation(szInfo);
    } else {
        UINT32 nErr = freerdp_get_last_error(instance->context);

        QString szErr;
        szErr = tr("Connect to ");
        szErr += m_pParameter->GetHost();
        szErr += ":";
        szErr += QString::number(m_pParameter->GetPort());
        szErr += tr(" fail.");
        szErr += " [";
        szErr += QString::number(nErr) + " - ";
        szErr += freerdp_get_last_error_name(nErr);
        szErr += "] ";
        /*szErr += "[";
        szErr += freerdp_get_last_error_category(nErr);
        szErr += "] ";*/
        szErr += freerdp_get_last_error_string(nErr);

        switch(nErr) {
        case FREERDP_ERROR_CONNECT_LOGON_FAILURE:
        {
            nRet = -3;
            QString szErr = tr("Logon to ");
            szErr += m_pParameter->GetHost();
            szErr += ":";
            szErr += QString::number(m_pParameter->GetPort());
            szErr += tr(" fail. Please check that the username and password are correct.") + "\n";
            emit sigShowMessage(tr("Error"), szErr, QMessageBox::Critical);
            break;
        }
        case FREERDP_ERROR_CONNECT_WRONG_PASSWORD:
        {
            nRet = -4;
            QString szErr = tr("Logon to ");
            szErr += m_pParameter->GetHost();
            szErr += ":";
            szErr += QString::number(m_pParameter->GetPort());
            szErr += tr(" fail. Please check password are correct.") + "\n";
            emit sigShowMessage(tr("Error"), szErr, QMessageBox::Critical);
            break;
        }
        case FREERDP_ERROR_AUTHENTICATION_FAILED:
        {
            nRet = -5;
            QString szErr = tr("Logon to ");
            szErr += m_pParameter->GetHost();
            szErr += ":";
            szErr += QString::number(m_pParameter->GetPort());
            szErr += tr(" authentication fail. please add a CA certificate to the store.") + "\n";
            emit sigShowMessage(tr("Error"), szErr, QMessageBox::Critical);
            break;
        }
        case FREERDP_ERROR_CONNECT_TRANSPORT_FAILED:
        {
            nRet = -6;
            QString szErr = tr("Logon to ");
            szErr += m_pParameter->GetHost();
            szErr += ":";
            szErr += QString::number(m_pParameter->GetPort());
            szErr += tr(" connect transport layer fail.") + "\n\n";
            szErr += tr("Please:") + "\n";
            szErr += tr("1. Check for any network related issues") + "\n";
            szErr += tr("2. Check you have proper security settings ('NLA' enabled is required for most connections nowadays)") + "\n";
            szErr += tr("3. Check the certificate is proper (and guacd properly checks that)") + "\n";
            emit sigShowMessage(tr("Error"), szErr, QMessageBox::Critical);
            break;
        }
        case FREERDP_ERROR_SECURITY_NEGO_CONNECT_FAILED:
        default:
            nRet = -7;
            emit sigShowMessage(tr("Error"), szErr, QMessageBox::Critical);
        }

        qCritical(FreeRDPConnect) << szErr;
        emit sigError(nRet, szErr.toStdString().c_str());
    }
    return nRet;
}

int CConnectFreeRDP::OnClientStop(rdpContext *context)
{
    int nRet = 0;
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::OnClientStop()";
#if FreeRDP_VERSION_MAJOR >= 3
    nRet = freerdp_client_common_stop(context);
#else
    BOOL bRet = freerdp_abort_connect(context->instance);
    if(!bRet)
    {   qCritical(FreeRDPConnect) << "freerdp_abort_connect fail";
        nRet = -1;
    }
#endif
    return nRet;
}

/**
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
BOOL CConnectFreeRDP::cb_pre_connect(freerdp* instance)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_pre_connect()";
	rdpChannels* channels = nullptr;
	rdpSettings* settings = nullptr;
	rdpContext* context = instance->context;

    if (!instance || !instance->context || !instance->context->settings)
    {
        return FALSE;
    }

    CConnectFreeRDP* pThis = ((ClientContext*)context)->pThis;
    if(!pThis) return FALSE;
    settings = instance->context->settings;
	channels = context->channels;

#if defined (Q_OS_WIN)
    settings->OsMajorType = OSMAJORTYPE_WINDOWS;
    settings->OsMinorType = OSMINORTYPE_WINDOWS_NT;
#endif
#if defined (Q_OS_UNIX)
	settings->OsMajorType = OSMAJORTYPE_UNIX;
	settings->OsMinorType = OSMINORTYPE_NATIVE_XSERVER;
#endif

    // Subscribe channel event
    PubSub_SubscribeChannelConnected(instance->context->pubSub,
                                     OnChannelConnectedEventHandler);
	PubSub_SubscribeChannelDisconnected(instance->context->pubSub,
	                                    OnChannelDisconnectedEventHandler);

	if (!freerdp_client_load_addins(channels, instance->context->settings))
		return FALSE;

    if (pThis->m_pParameter->GetUser().isEmpty()
        && !freerdp_settings_get_bool(settings, FreeRDP_CredentialsFromStdin)
        && !freerdp_settings_get_bool(settings, FreeRDP_SmartcardLogon))
	{
        // Get system login name
        QString szUser = RabbitCommon::CTools::Instance()->GetCurrentUser();
        if(!szUser.isEmpty()){
            pThis->m_pParameter->SetUser(szUser);
        }
        qWarning(FreeRDPConnect) << "No user name set. - Using login name:" << szUser;
	}

    if (freerdp_settings_get_bool(settings, FreeRDP_AuthenticationOnly))
    {
        /* Check +auth-only has a username and password. */
        if (!pThis->m_pParameter->GetPassword().isEmpty())
        {
            qCritical(FreeRDPConnect) << "auth-only, but no password set. Please provide one.";
            return FALSE;
        }
#if FreeRDP_VERSION_MAJOR > 2
        if (!freerdp_settings_set_bool(settings, FreeRDP_DeactivateClientDecoding, TRUE))
            return FALSE;
#endif
        qInfo(FreeRDPConnect) << "Authentication only. Don't connect to X.";
    } else {
		
	}

    // Keyboard layout
    UINT32 rc = freerdp_keyboard_init(
                freerdp_settings_get_uint32(settings, FreeRDP_KeyboardLayout));
    freerdp_settings_set_uint32(settings, FreeRDP_KeyboardLayout, rc);

    // Check desktop size, it is set in parameter
    UINT32 width = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    UINT32 height = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);
    if ((width < 64) || (height < 64) ||
            (width > 4096) || (height > 4096))
    {
        qCritical(FreeRDPConnect) << "invalid dimensions" << width << "*" << height;
        return FALSE;
    } else {
        qInfo(FreeRDPConnect) << "Init desktop size " <<  width << "*" << height;
    }

	return TRUE;
}

const char* CConnectFreeRDP::GetTitle(freerdp* instance)
{
    const char* windowTitle;
    UINT32 port;
    BOOL addPort;
    const char* name = nullptr;

    CConnectFreeRDP* pThis = ((ClientContext*)instance->context)->pThis;
    rdpSettings* settings = instance->context->settings;

    if (!settings)
        return nullptr;
    
    windowTitle = freerdp_settings_get_string(settings, FreeRDP_WindowTitle);
    if (windowTitle)
        return _strdup(windowTitle);

#if FreeRDP_VERSION_MAJOR >= 3
    name = freerdp_settings_get_server_name(settings);
#else
    name = pThis->m_pParameter->GetHost().toStdString().c_str();
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
BOOL CConnectFreeRDP::cb_post_connect(freerdp* instance)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_post_connect()";
	
	rdpContext* context = instance->context;
	rdpSettings* settings = instance->context->settings;
    rdpUpdate* update = instance->context->update;
    CConnectFreeRDP* pThis = ((ClientContext*)instance->context)->pThis;
    
    const char* pWindowTitle = GetTitle(instance);
    if(pWindowTitle)
    {
        WCHAR* windowTitle = NULL;
#if FreeRDP_VERSION_MAJOR >= 3
        windowTitle = ConvertUtf8ToWCharAlloc(settings->WindowTitle, NULL);
#else
        ConvertToUnicode(CP_UTF8, 0, settings->WindowTitle, -1, &windowTitle, 0);
#endif
        if(windowTitle)
        {
            QString title = QString::fromUtf16((const char16_t*)windowTitle);
            if(pThis->m_pParameter->GetServerName().isEmpty())
                emit pThis->sigServerName(title);
        }
    }
    int desktopWidth = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    int desktopHeight = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);

    emit pThis->sigSetDesktopSize(desktopWidth, desktopHeight);

    pThis->m_Image = QImage(desktopWidth,
                            desktopHeight,
                            QImage::Format_RGB32);

    // Init gdi format
    if (!gdi_init_ex(instance, pThis->GetImageFormat(),
                     0, pThis->m_Image.bits(), nullptr))
	{
        qCritical(FreeRDPConnect) << "gdi_init fail";
        return FALSE;
    }

    // Register cursor pointer
    if(pThis->m_Cursor.RegisterPointer(context->graphics))
        return FALSE;

    update->BeginPaint = cb_begin_paint;
    update->EndPaint = cb_end_paint;
    update->DesktopResize = cb_desktop_resize;

	update->PlaySound = cb_play_bell_sound;
//	update->SetKeyboardIndicators = xf_keyboard_set_indicators;
//	update->SetKeyboardImeStatus = xf_keyboard_set_ime_status;
    
//    freerdp_keyboard_init_ex(instance->context->settings->KeyboardLayout,
//	                         instance->context->settings->KeyboardRemappingList);
    
    //TODO: clipboard
//	if (!(xfc->clipboard = xf_clipboard_new(xfc)))
//		return FALSE;

//	if (!(xfc->xfDisp = xf_disp_new(xfc)))
//	{
//		xf_clipboard_free(xfc->clipboard);
//		return FALSE;
//	}

//	EventArgsInit(&e, "xfreerdp");
//	e.width = settings->DesktopWidth;
//	e.height = settings->DesktopHeight;
//	PubSub_OnResizeWindow(context->pubSub, xfc, &e);
    
	return TRUE;
}

void CConnectFreeRDP::cb_post_disconnect(freerdp* instance)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_post_disconnect()";
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

int CConnectFreeRDP::cb_logon_error_info(freerdp* instance, UINT32 data, UINT32 type)
{
	CConnectFreeRDP* pThis = ((ClientContext*)instance->context)->pThis;
	const char* str_data = freerdp_get_logon_error_info_data(data);
	const char* str_type = freerdp_get_logon_error_info_type(type);
    QString szErr = tr("FreeRDP logon info: [");
    szErr += str_type;
    szErr += "] ";
    szErr += str_data;
	qDebug(FreeRDPConnect) << szErr;
	emit pThis->sigInformation(szErr);
    emit pThis->sigError(type, szErr);
	return 1;
}

void CConnectFreeRDP::OnChannelConnectedEventHandler(void *context,
                                                     #if FreeRDP_VERSION_MAJOR >= 3
                                                     const
                                                     #endif
                                                     ChannelConnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    CConnectFreeRDP* pThis = ((ClientContext*)context)->pThis;
    if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0) {
		qInfo(FreeRDPConnect) << "channel" << e->name << "connected";
        pThis->m_ClipBoard.Init((CliprdrClientContext*)e->pInterface,
                                pThis->m_pParameter->GetClipboard());
	} else
        qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
}

void CConnectFreeRDP::OnChannelDisconnectedEventHandler(void *context,
                                                        #if FreeRDP_VERSION_MAJOR >= 3
                                                        const
                                                        #endif
                                                        ChannelDisconnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    CConnectFreeRDP* pThis = ((ClientContext*)context)->pThis;
    
    if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "channel" << e->name << "disconnected";
        pThis->m_ClipBoard.UnInit((CliprdrClientContext*)e->pInterface,
                                  pThis->m_pParameter->GetClipboard());
	} else {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
	}
}

UINT32 CConnectFreeRDP::GetImageFormat(QImage::Format format)
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

UINT32 CConnectFreeRDP::GetImageFormat()
{
    return GetImageFormat(m_Image.format());
}

#if FreeRDP_VERSION_MAJOR >= 3

static CREDUI_INFOW wfUiInfo = { sizeof(CREDUI_INFOW), NULL, L"Enter your credentials",
                                L"Remote Desktop Security", NULL };
BOOL CConnectFreeRDP::cb_authenticate_ex(freerdp* instance,
                               char** username, char** password,
                               char** domain, rdp_auth_reason reason)
{
    qCritical(FreeRDPConnect) << "CConnectFreeRdp::cb_authenticate_ex";
    if(!instance)
        return FALSE;
    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
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
            qCritical(FreeRDPConnect,
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
                qCritical(FreeRDPConnect,
                          "Failed to parse UserName: %s into User: %s Domain: %s",
                          UserName, User, Domain);
                return FALSE;
            }
        }
    }
    
    *username = ConvertWCharNToUtf8Alloc(UserW, ARRAYSIZE(UserW), NULL);
    if (!(*username))
    {
        qCritical(FreeRDPConnect) << "ConvertWCharNToUtf8Alloc failed" << status;
        return FALSE;
    }
    
    if (_wcsnlen(DomainW, ARRAYSIZE(DomainW)) > 0)
        *domain = ConvertWCharNToUtf8Alloc(DomainW, ARRAYSIZE(DomainW), NULL);
    else
        *domain = _strdup("\0");
    
    if (!(*domain))
    {
        free(*username);
        qCritical(FreeRDPConnect) << "strdup failed" << status;
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
BOOL CConnectFreeRDP::cb_choose_smartcard(freerdp* instance,
                         SmartcardCertInfo** cert_list,
                         DWORD count,
                         DWORD* choice, BOOL gateway)
{
    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
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

#endif //#if FreeRDP_VERSION_MAJOR >= 3

BOOL CConnectFreeRDP::cb_authenticate(freerdp* instance, char** username,
                                      char** password, char** domain)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_authenticate";
	if(!instance)
		return FALSE;
    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    if(username || password || domain)
    {
        int nRet = QDialog::Rejected;
        emit pThis->sigBlockShowWidget("CDlgGetUserPasswordFreeRDP", nRet, pThis->m_pParameter);
        if(QDialog::Accepted == nRet)
        {
            QString szPassword = pThis->m_pParameter->GetPassword();
            QString szName = pThis->m_pParameter->GetUser();
            QString szDomain = freerdp_settings_get_string(
                pThis->m_pParameter->m_pSettings, FreeRDP_Domain);;
            if(!szDomain.isEmpty() && domain)
                *domain = _strdup(szDomain.toStdString().c_str());
            if(!szName.isEmpty() && username)
                *username = _strdup(szName.toStdString().c_str());
            //if(!szPassword.isEmpty() && password) // 如果加上会触发 FREERDP_ERROR_AUTHENTICATION_FAILED
                *password = _strdup(szPassword.toStdString().c_str());
        } else
            return FALSE;
    }
    return TRUE;
}

BOOL CConnectFreeRDP::cb_GatewayAuthenticate(freerdp *instance,
                                char **username, char **password, char **domain)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_GatewayAuthenticate";
	if(!instance)
		return FALSE;

    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    if(username || password)
    {
        int nRet = QDialog::Rejected;
        emit pThis->sigBlockShowWidget("CDlgGetUserPasswordFreeRDP", nRet, pThis->m_pParameter);
        if(QDialog::Accepted == nRet)
        {
            QString szPassword = pThis->m_pParameter->GetPassword();
            QString szName = pThis->m_pParameter->GetUser();
            QString szDomain = freerdp_settings_get_string(
                pThis->m_pParameter->m_pSettings, FreeRDP_Domain);;
            if(!szDomain.isEmpty() && domain)
                *domain = _strdup(szDomain.toStdString().c_str());
            if(!szName.isEmpty() && username)
                *username = _strdup(szName.toStdString().c_str());
            //if(!szPassword.isEmpty() && password)
                *password = _strdup(szPassword.toStdString().c_str());
        } else
            return FALSE;
    }
	return TRUE;
}

int CConnectFreeRDP::cb_verify_x509_certificate(freerdp* instance,
                                               const BYTE* data, size_t length,
                                 const char* hostname, UINT16 port, DWORD flags)
{
    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;

    return 0;
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
DWORD CConnectFreeRDP::cb_verify_certificate_ex(freerdp *instance,
                       const char *host, UINT16 port,
                       const char *common_name, const char *subject,
                       const char *issuer, const char *fingerprint, DWORD flags)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_verify_certificate_ex";

    rdpContext* pContext = (rdpContext*)instance->context;
    Q_ASSERT(pContext);
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
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
    message += tr("Fingerprint: ") + fingerprint + "\n";
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

    QMessageBox::StandardButton nRet = QMessageBox::StandardButton::No;
    QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::Ignore | QMessageBox::No;
    bool bCheckBox = false;
    emit pThis->sigBlockShowMessage(title, message, buttons, nRet, bCheckBox,
                                    tr("Don't show again"));
    pThis->m_pParameter->SetShowVerifyDiaglog(!bCheckBox);
    if(pThis->m_pConnecter)
        emit pThis->m_pConnecter->sigUpdateParameters(pThis->m_pConnecter);
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
DWORD CConnectFreeRDP::cb_verify_changed_certificate_ex(freerdp *instance,
                      const char *host, UINT16 port,
                      const char *common_name, const char *subject,
                      const char *issuer, const char *fingerprint,
                      const char *old_subject, const char *old_issuer,
                      const char *old_fingerprint, DWORD flags)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_verify_changed_certificate_ex";
    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
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
    message += tr("Common name: ") + common_name + "\n";
    message += tr("New subject: ") + subject + "\n";
    message += tr("New issuer: ") + issuer + "\n";
    message += tr("New fingerprint: ") + fingerprint + "\n";
    message += tr("Old subject: ") + old_subject + "\n";
    message += tr("Old issuer: ") + old_issuer + "\n";
    message += tr("Old fingerprint: ") + old_fingerprint + "\n";
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
    emit pThis->sigBlockShowMessage(title, message, buttons, nRet, bCheckBox,
                                    tr("Don't show again"));
    pThis->m_pParameter->SetShowVerifyDiaglog(!bCheckBox);
    if(pThis->m_pConnecter)
        emit pThis->m_pConnecter->sigUpdateParameters(pThis->m_pConnecter);

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

BOOL CConnectFreeRDP::cb_present_gateway_message(
        freerdp* instance, UINT32 type, BOOL isDisplayMandatory,
        BOOL isConsentMandatory, size_t length, const WCHAR* message)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_present_gateway_message";

    if (!isDisplayMandatory && !isConsentMandatory)
        return TRUE;

    /* special handling for consent messages (show modal dialog) */
    if (type == GATEWAY_MESSAGE_CONSENT && isConsentMandatory)
    {
        QString msgType = (type == GATEWAY_MESSAGE_CONSENT)
                              ? tr("Consent message") : tr("Service message");
        msgType += "\n";
#if FreeRDP_VERSION_MAJOR >= 3
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
        CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
        QMessageBox::StandardButton nRet = QMessageBox::No;
        bool bCheckBox = false;
        emit pThis->sigBlockShowMessage(tr("Gateway message"), msgType,
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

BOOL CConnectFreeRDP::cb_begin_paint(rdpContext *context)
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

BOOL CConnectFreeRDP::UpdateBuffer(INT32 x, INT32 y, INT32 w, INT32 h)
{
    if(x > m_Image.width() || y > m_Image.height())
        return TRUE;

    QRect rect(x, y, w, h);
    //qDebug(FreeRDPConnect) << "Update:" << rect;
    emit sigUpdateRect(rect, m_Image);
    return FALSE;
}

BOOL CConnectFreeRDP::cb_end_paint(rdpContext *context)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_end_paint";
    ClientContext* pContext = (ClientContext*)context;
    CConnectFreeRDP* pThis = pContext->pThis;
    int ninvalid;
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

    HGDI_WND hwnd = context->gdi->primary->hdc->hwnd;
    ninvalid = hwnd->ninvalid;
	cinvalid = hwnd->cinvalid;
    if (ninvalid < 1)
		return TRUE;

    region16_init(&invalidRegion);

	for (i = 0; i < ninvalid; i++)
	{
		invalidRect.left = cinvalid[i].x;
		invalidRect.top = cinvalid[i].y;
		invalidRect.right = cinvalid[i].x + cinvalid[i].w;
		invalidRect.bottom = cinvalid[i].y + cinvalid[i].h;
		region16_union_rect(&invalidRegion, &invalidRegion, &invalidRect);
	}

	if (!region16_is_empty(&invalidRegion))
	{
		extents = region16_extents(&invalidRegion);
        QRect updateRect;
        updateRect.setX(extents->left);
        updateRect.setY(extents->top);
        updateRect.setRight(extents->right);
        updateRect.setBottom(extents->bottom);
        pThis->UpdateBuffer(updateRect.x(), updateRect.y(), updateRect.width(), updateRect.height());
    }

	region16_uninit(&invalidRegion);

    return TRUE;
}

BOOL CConnectFreeRDP::cb_desktop_resize(rdpContext* context)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRDP::cb_desktop_resize";
    ClientContext* pContext = (ClientContext*)context;
    CConnectFreeRDP* pThis = pContext->pThis;
    rdpSettings* settings;
    if (!context || !context->settings)
		return FALSE;
    settings = context->settings;
    int desktopWidth = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    int desktopHeight = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);

    emit pThis->sigSetDesktopSize(desktopWidth, desktopHeight);

    pThis->m_Image = QImage(desktopWidth,
                            desktopHeight,
                            QImage::Format_RGB32);
    if (!gdi_resize_ex(context->gdi, desktopWidth, desktopHeight, 0,
	                   context->gdi->dstFormat, pThis->m_Image.bits(), NULL))
		return FALSE;
    pThis->UpdateBuffer(0, 0, desktopWidth, desktopHeight);
    return TRUE;
}

BOOL CConnectFreeRDP::cb_play_bell_sound(rdpContext *context, const PLAY_SOUND_UPDATE *play_sound)
{
    ClientContext* pContext = (ClientContext*)context;
    CConnectFreeRDP* pThis = pContext->pThis;
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

// https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/2c1ced34-340a-46cd-be6e-fc8cab7c3b17
bool CConnectFreeRDP::SendMouseEvent(UINT16 flags, QPoint pos)
{
    if(m_pParameter && m_pParameter->GetOnlyView()) return true;
    if(!m_pContext) return false;

#if FreeRDP_VERSION_MAJOR >= 3
    freerdp_client_send_button_event(&m_pContext->Context, FALSE, flags,
                                     pos.x(), pos.y());
#else
    if(!m_pContext->Context.input) return false;
    return freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   pos.x(),
                                   pos.y());
#endif
    return true;
}

void CConnectFreeRDP::slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotWheelEvent" << buttons << pos;
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
   
    UINT16 flags = 0;
    QPoint p = angleDelta;
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
#if FreeRDP_VERSION_MAJOR >= 3
    freerdp_client_send_wheel_event(&m_pContext->Context, flags);
    /*
    freerdp_client_send_button_event(&m_pContext->Context, FALSE, flags,
                                     pos.x(), pos.y());//*/
#else
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   pos.x(),
                                   pos.y());
#endif
}

// https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdpbcgr/2c1ced34-340a-46cd-be6e-fc8cab7c3b17
void CConnectFreeRDP::slotMouseMoveEvent(Qt::MouseButtons buttons, QPoint pos)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotMouseMoveEvent" << buttons << pos;
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    UINT16 flags = PTR_FLAGS_MOVE;
    SendMouseEvent(flags, pos);
}

void CConnectFreeRDP::slotMousePressEvent(Qt::MouseButtons buttons, QPoint pos)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotMousePressEvent" << buttons << pos;
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    UINT16 flags = PTR_FLAGS_DOWN;
    if(buttons & Qt::MouseButton::LeftButton)
    {
        flags |= PTR_FLAGS_BUTTON1;
    } else if(buttons & Qt::MouseButton::RightButton)
    {   
        flags |= PTR_FLAGS_BUTTON2;
    } else if(buttons & Qt::MouseButton::MiddleButton)
    {
        flags |= PTR_FLAGS_BUTTON3;
    }
    SendMouseEvent(flags, pos);    
}

void CConnectFreeRDP::slotMouseReleaseEvent(Qt::MouseButton button, QPoint pos)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotMouseReleaseEvent" << button << pos;
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    UINT16 flags = 0;
    if(button & Qt::MouseButton::LeftButton)
    {
        flags |= PTR_FLAGS_BUTTON1;
    } else if(button & Qt::MouseButton::MiddleButton)
    {
        flags |= PTR_FLAGS_BUTTON3;
    } else if(button & Qt::MouseButton::RightButton)
    {
        flags |= PTR_FLAGS_BUTTON2;
    }
    SendMouseEvent(flags, pos);    
}

void CConnectFreeRDP::slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotKeyPressEvent";
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    // Convert to rdp scan code freerdp/scancode.h
    UINT32 k = CConvertKeyCode::QtToScanCode(key, modifiers);
    if(RDP_SCANCODE_UNKNOWN != key)
#if FreeRDP_VERSION_MAJOR >= 3
        freerdp_input_send_keyboard_event_ex(m_pContext->Context.context.input, true, true, k);
#else
        freerdp_input_send_keyboard_event_ex(m_pContext->Context.input, true, k);
#endif
}

void CConnectFreeRDP::slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotKeyReleaseEvent";
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    UINT32 k = CConvertKeyCode::QtToScanCode(key, modifiers);
    if(RDP_SCANCODE_UNKNOWN != key)
#if FreeRDP_VERSION_MAJOR >= 3
        freerdp_input_send_keyboard_event_ex(m_pContext->Context.context.input, false, false, k);
#else
        freerdp_input_send_keyboard_event_ex(m_pContext->Context.input, false, k);
#endif
}

int CConnectFreeRDP::RedirectionSound()
{
    rdpContext* pRdpContext = (rdpContext*)m_pContext;
    freerdp* instance = freerdp_client_get_instance(pRdpContext);
    rdpSettings* settings = instance->context->settings;
    Q_ASSERT(settings);

    if(m_pParameter->GetRedirectionSound() == CParameterFreeRDP::RedirecionSoundType::Disable)
    {
        /* Disable sound */
		freerdp_settings_set_bool(settings, FreeRDP_AudioPlayback, FALSE);
		freerdp_settings_set_bool(settings, FreeRDP_RemoteConsoleAudio, FALSE);
        return 0;
    } else if(m_pParameter->GetRedirectionSound() == CParameterFreeRDP::RedirecionSoundType::Local)
    {
        freerdp_settings_set_bool(settings, FreeRDP_AudioPlayback, TRUE);
		freerdp_settings_set_bool(settings, FreeRDP_AudioCapture, TRUE);
    } else if(m_pParameter->GetRedirectionSound() == CParameterFreeRDP::RedirecionSoundType::Remote)
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
                                                #if FreeRDP_VERSION_MAJOR < 3
                                                    ptr.p
                                                #else
                                                    ptr.pc
                                                #endif
                                                    );
    if (status)
    {
        status = freerdp_client_add_dynamic_channel(settings, count,
                                            #if FreeRDP_VERSION_MAJOR < 3
                                                    ptr.p
                                            #else
                                                    ptr.pc
                                            #endif
                                                    );
    }
    
    if(!status)
    {
        qCritical(FreeRDPConnect) << "Load rdpsnd fail";
        return -1;
    }
    
    return 0;
}

int CConnectFreeRDP::RedirectionMicrophone()
{
    if(m_pParameter->GetRedirectionSound() == CParameterFreeRDP::RedirecionSoundType::Remote)
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
                                                 #if FreeRDP_VERSION_MAJOR < 3
                                                         ptr.p
                                                 #else
                                                         ptr.pc
                                                 #endif
                                                     );
    
    if(!status)
    {
        qCritical(FreeRDPConnect) << "Load audin fail";
        return -1;
    }
    
    return 0;
}

int CConnectFreeRDP::RedirectionDrive()
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
                                                #if FreeRDP_VERSION_MAJOR < 3
                                                    (char**)
                                                #endif
                                                        argvDrive);
        if(pDrive) free(pDrive);
        if(!status)
        {
            qCritical(FreeRDPConnect) << "Load drive fail";
            return -1;
        }
    }

    return 0;
}

int CConnectFreeRDP::RedirectionPrinter()
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
        qCritical(FreeRDPConnect) << "The printer is empty";
        return -1;
    }
    qDebug(FreeRDPConnect) << printerList;

    // Format: /printer:<device>,<driver>,[default]
    const char* argvPrinter[] = {"printer",  nullptr, nullptr};
    int count = sizeof(argvPrinter) / sizeof(const char*);
    BOOL status = freerdp_client_add_device_channel(settings, count,
                                                #if FreeRDP_VERSION_MAJOR < 3
                                                    (char**)
                                                #endif
                                                    argvPrinter);
    if(!status) {
        qCritical(FreeRDPConnect) << "Load printer fail";
        return -2;
    }

    return 0;
}

int CConnectFreeRDP::RedirectionSerial()
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
        qDebug(FreeRDPConnect) << "systemLocation:" << serial.systemLocation()
                        << "portName:" << serial.portName()
                        << "serialNumber:" << serial.serialNumber();
        char* pSerial = _strdup(serial.systemLocation().toStdString().c_str());
        char* pName = _strdup(serial.portName().toStdString().c_str());
        const char* argvSerial[] = {"serial", pName, pSerial};
        int count = sizeof(argvSerial) / sizeof(const char*);
        BOOL status = freerdp_client_add_device_channel(settings, count,
                                                #if FreeRDP_VERSION_MAJOR < 3
                                                    (char**)
                                                #endif
                                                        argvSerial);
        if(pSerial) free(pSerial);
        if(pName) free(pName);

        if(!status)
        {
            qCritical(FreeRDPConnect) << "Load drive fail";
            return -1;
        }
    }

    return 0;
}
