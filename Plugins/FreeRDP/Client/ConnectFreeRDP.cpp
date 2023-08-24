// Author: Kang Lin <kl222@126.com>

#include "ConnectFreeRDP.h"

#include "freerdp/client/cmdline.h"
#include "freerdp/client/encomsp.h"
#include "freerdp/gdi/gfx.h"
#include "freerdp/settings.h"
#include "freerdp/locale/keyboard.h"
#include "freerdp/channels/rdpgfx.h"
#include "freerdp/channels/cliprdr.h"

#undef PEN_FLAG_INVERTED
#include "freerdp/channels/rdpei.h"
#include "freerdp/channels/rdpdr.h"
#include "freerdp/channels/disp.h"
#include "freerdp/channels/tsmf.h"
#include "freerdp/channels/rdpsnd.h"
#include "freerdp/client/channels.h"

#include "RabbitCommonTools.h"
#include "ConvertKeyCode.h"
#include "DlgGetUserPasswordFreeRDP.h"

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

    rdpSettings* settings = m_pParameter->m_pSettings;
    
    freerdp_settings_set_string(settings,
                                FreeRDP_ServerHostname,
                                m_pParameter->GetHost().toStdString().c_str());
    settings->ServerPort = m_pParameter->GetPort();
    freerdp_settings_set_string(settings,
                                FreeRDP_Username,
                                m_pParameter->GetUser().toStdString().c_str());
    freerdp_settings_set_string(settings,
                                FreeRDP_Password,
                                m_pParameter->GetPassword().toStdString().c_str());

    ZeroMemory(&m_ClientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	m_ClientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
	m_ClientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
    m_ClientEntryPoints.settings = m_pParameter->m_pSettings;
	m_ClientEntryPoints.GlobalInit = cb_global_init;
	m_ClientEntryPoints.GlobalUninit = cb_global_uninit;
	m_ClientEntryPoints.ContextSize = sizeof(ClientContext);
	m_ClientEntryPoints.ClientNew = cb_client_new;
	m_ClientEntryPoints.ClientFree = cb_client_free;
	m_ClientEntryPoints.ClientStart = cb_client_start;
	m_ClientEntryPoints.ClientStop = cb_client_stop;
    
    rdpContext* p = freerdp_client_context_new(&m_ClientEntryPoints);
	if(p)
    {
        m_pContext = (ClientContext*)p;
        m_pContext->pThis = this;
    } else {
        qCritical(FreeRDPConnect) << "freerdp_client_context_new fail";
        throw("freerdp_client_context_new fail");
    }
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
    BOOL status = false;
    rdpContext* pRdpContext = (rdpContext*)m_pContext;
    freerdp* instance = pRdpContext->instance;
    rdpSettings* settings = instance->context->settings;
    Q_ASSERT(settings);
    if(nullptr == settings) return -1;

    //Load channel
    RedirectionSound();
    RedirectionMicrophone();
    RedirectionDrive();
    RedirectionPrinter();
    RedirectionSerial();

    freerdp_client_start(pRdpContext);
    status = freerdp_connect(instance);
    if (!status)
	{
        QMessageBox::StandardButton nRetButton = QMessageBox::Ok;
        bool checkBox = false;
        QString szErr;
        UINT32 nErr = freerdp_get_last_error(instance->context);
        switch(nErr) {
        case FREERDP_ERROR_CONNECT_LOGON_FAILURE:
        {
            nRet = -2;
            szErr = tr("Logon to ");
            szErr += freerdp_settings_get_string(settings, FreeRDP_ServerHostname);
            szErr += ":";
            szErr += QString::number(freerdp_settings_get_uint32(settings, FreeRDP_ServerPort));
            szErr += tr(" fail. Please check that the username and password are correct.");
            break;
        }
        case FREERDP_ERROR_AUTHENTICATION_FAILED:
        case FREERDP_ERROR_SECURITY_NEGO_CONNECT_FAILED:
        default:
            nRet = -3;
            szErr = tr("Connect to ");
            szErr += freerdp_settings_get_string(settings, FreeRDP_ServerHostname);
            szErr += ":";
            szErr += QString::number(freerdp_settings_get_uint32(settings, FreeRDP_ServerPort));
            szErr += tr(" fail:");
            szErr += "[";
            szErr += QString::number(nErr) + " - ";
            szErr += freerdp_get_last_error_name(nErr);
            szErr += "] ";
            /*szErr += "[";
        szErr += freerdp_get_last_error_category(nErr);
        szErr += "] ";*/
            szErr += freerdp_get_last_error_string(nErr);
        }
        qCritical(FreeRDPConnect) << szErr;
        emit sigError(nRet, szErr.toStdString().c_str());
    } else {
        emit sigConnected();

        QString szInfo = tr("Connect to ");
        szInfo += freerdp_settings_get_string(settings, FreeRDP_ServerHostname);
        szInfo += ":";
        szInfo += QString::number(freerdp_settings_get_uint32(settings, FreeRDP_ServerPort));
        qInfo(FreeRDPConnect) << szInfo;
        emit sigInformation(szInfo);
    }
    return nRet;
}

int CConnectFreeRDP::OnClean()
{
    int nRet = 0;
    if(m_pContext)
    {
        rdpContext* pRdpContext = (rdpContext*)m_pContext;
        freerdp_disconnect(pRdpContext->instance);
        freerdp_client_stop(pRdpContext);
#if FreeRDP_VERSION_MAJOR >= 3
        freerdp_abort_connect_context(pRdpContext);
#else
        freerdp_abort_connect(pRdpContext->instance);
#endif
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
#if FreeRDP_VERSION_MAJOR >= 3
    if(freerdp_shall_disconnect_context(pRdpContext))
#else
    if(freerdp_shall_disconnect(pRdpContext->instance))
#endif
        return -1;
    else
    {
        DWORD nCount = 0;
        DWORD tmp = freerdp_get_event_handles(pRdpContext, &handles[nCount],
                                              ARRAYSIZE(handles) - nCount);
        if (tmp == 0)
        {
            qCritical(FreeRDPConnect) << "freerdp_get_event_handles failed";
            return -2;
        }
        
        nCount += tmp;
#if FreeRDP_VERSION_MAJOR < 3
        handles[nCount++] = pRdpContext->abortEvent;
#endif
        DWORD waitStatus = WaitForMultipleObjects(nCount, handles, FALSE, 500);
        if (waitStatus == WAIT_FAILED)
            return -3;
#if FreeRDP_VERSION_MAJOR < 3
        /* Check if a processed event called freerdp_abort_connect() and exit if true */
		if (WaitForSingleObject(pRdpContext->abortEvent, 0) == WAIT_OBJECT_0)
			/* Session disconnected by local user action */
            return -4;
#endif
        if (!freerdp_check_event_handles(pRdpContext))
        {
            nRet = -5;
            UINT32 err = freerdp_get_last_error(pRdpContext);
            qCritical(FreeRDPConnect) << "freerdp_check_event_handles[" << err << "]"
                                      << freerdp_get_last_error_category(err) << "-"
                                      << freerdp_get_last_error_name(err) << ":"
                                      << freerdp_get_last_error_string(err);
            // Reconnect
            bool bReconnect = freerdp_settings_get_bool(pRdpContext->settings,
                                               FreeRDP_AutoReconnectionEnabled);
            qDebug(FreeRDPConnect) << "Reconnect:" << bReconnect;
            if(bReconnect)
            {
                qDebug(FreeRDPConnect) << "Reconnect" << m_pParameter->GetReconnectInterval() * 1000;
                if (client_auto_reconnect_ex(pRdpContext->instance, NULL))
                    return m_pParameter->GetReconnectInterval() * 1000;
                else
                {
                    /*
                     * Indicate an unsuccessful connection attempt if reconnect
                     * did not succeed and no other error was specified.
                     */
                    if (freerdp_error_info(pRdpContext->instance) == 0)
                        nRet = -6;
                }

                UINT32 err = freerdp_get_last_error(pRdpContext);
                if (FREERDP_ERROR_SUCCESS != err)
                    qCritical(FreeRDPConnect) << "Auto reconnection[" << err << "]"
                                          << freerdp_get_last_error_category(err)
                                          << freerdp_get_last_error_name(err)
                                          << freerdp_get_last_error_string(err);
            }
        }
    }

    return nRet;
}

void CConnectFreeRDP::slotClipBoardChanged()
{
    qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotClipBoardChanged()";
    if(m_pParameter->GetClipboard())
        m_ClipBoard.slotClipBoardChanged();
}

BOOL CConnectFreeRDP::cb_global_init()
{
	qDebug(FreeRDPConnect) << "CConnectFreeRdp::Client_global_init()";
    freerdp_register_addin_provider(freerdp_channels_load_static_addin_entry, 0);
	return TRUE;
}

void CConnectFreeRDP::cb_global_uninit()
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::Client_global_uninit()";
}

BOOL CConnectFreeRDP::cb_client_new(freerdp *instance, rdpContext *context)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::Client_new()";
    instance->PreConnect = cb_pre_connect;
	instance->PostConnect = cb_post_connect;
	instance->PostDisconnect = cb_post_disconnect;

    // Because it is already set in the parameters
#if FreeRDP_VERSION_MAJOR < 3
    instance->Authenticate = cb_authenticate;
    instance->GatewayAuthenticate = cb_GatewayAuthenticate;
#else
    instance->AuthenticateEx = cb_authenticate_ex;
#endif
    instance->VerifyCertificateEx = cb_verify_certificate_ex;
	instance->VerifyChangedCertificateEx = cb_verify_changed_certificate_ex;
	instance->PresentGatewayMessage = cb_present_gateway_message;

	instance->LogonErrorInfo = cb_logon_error_info;

    // Because it is processed in OnProcess. so comment it.
    //PubSub_SubscribeTerminate(context->pubSub, OnTerminateEventHandler);

    return TRUE;
}

void CConnectFreeRDP::cb_client_free(freerdp *instance, rdpContext *context)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::Client_free()";
    PubSub_UnsubscribeTerminate(context->pubSub, OnTerminateEventHandler);
}

int CConnectFreeRDP::cb_client_start(rdpContext *context)
{
    int nRet = 0;
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::Client_start()";
    return nRet;
}

int CConnectFreeRDP::cb_client_stop(rdpContext *context)
{
    int nRet = 0;
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::Client_stop()";
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
        && !settings->CredentialsFromStdin
        && !settings->SmartcardLogon)
	{
        // Get system login name
        QString szUser = RabbitCommon::CTools::Instance()->GetCurrentUser();
        if(!szUser.isEmpty()){
            pThis->m_pParameter->SetUser(szUser);
            freerdp_settings_set_string(settings,
                                        FreeRDP_Username,
                          pThis->m_pParameter->GetUser().toStdString().c_str());
        }
        qWarning(FreeRDPConnect) << "No user name set. - Using login name:" << szUser;
	}

    if (freerdp_settings_get_bool(settings, FreeRDP_AuthenticationOnly))
    {
        /* Check +auth-only has a username and password. */
        if (!freerdp_settings_get_string(settings, FreeRDP_Password))
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

void CConnectFreeRDP::OnTerminateEventHandler(void *context,
                                              #if FreeRDP_VERSION_MAJOR >= 3
                                              const
                                              #endif
                                              TerminateEventArgs *e)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRDP::TerminateEventHandler:" << e->code;
    WINPR_UNUSED(e);
    rdpContext* pContext = (rdpContext*)context;
    CConnectFreeRDP* pThis = ((ClientContext*)context)->pThis;
    emit pThis->sigDisconnected();
}

void CConnectFreeRDP::OnChannelConnectedEventHandler(void *context,
                                                     #if FreeRDP_VERSION_MAJOR >= 3
                                                     const
                                                     #endif
                                                     ChannelConnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    CConnectFreeRDP* pThis = ((ClientContext*)context)->pThis;
    if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
    {
        qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
    } else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0) {
		qInfo(FreeRDPConnect) << "channel" << e->name << "connected";
        pThis->m_ClipBoard.Init((CliprdrClientContext*)e->pInterface,
                                pThis->m_pParameter->GetClipboard());
	} else if (strcmp(e->name, RDPEI_DVC_CHANNEL_NAME) == 0) {
        qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
    } else if (strcmp(e->name, TSMF_DVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
	} else  if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
	} else if (strcmp(e->name, ENCOMSP_SVC_CHANNEL_NAME) == 0) {
        qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
	} else if (strcmp(e->name, DISP_DVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
	} else if (strcmp(e->name, GEOMETRY_DVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
	} else if (strcmp(e->name, VIDEO_CONTROL_DVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
	} else if (strcmp(e->name, VIDEO_DATA_DVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "connected but we can’t use it";
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
    
    if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
    {
        qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
    } else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "channel" << e->name << "disconnected";
        pThis->m_ClipBoard.UnInit((CliprdrClientContext*)e->pInterface,
                                  pThis->m_pParameter->GetClipboard());
	} else if (strcmp(e->name, RDPEI_DVC_CHANNEL_NAME) == 0) {
        qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
	} else if (strcmp(e->name, TSMF_DVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
	} else if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
	} else if (strcmp(e->name, ENCOMSP_SVC_CHANNEL_NAME) == 0) {
        qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
	} else if (strcmp(e->name, DISP_DVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
	} else if (strcmp(e->name, GEOMETRY_DVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
	} else if (strcmp(e->name, VIDEO_CONTROL_DVC_CHANNEL_NAME) == 0) {
		qDebug(FreeRDPConnect) << "Unimplemented: channel" << e->name << "disconnected but we can’t use it";
	} else if (strcmp(e->name, VIDEO_DATA_DVC_CHANNEL_NAME) == 0) {
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

#if FreeRDP_VERSION_MAJOR > 3
//TODO: implement it!!!
static BOOL CConnectFreeRDP::cb_authenticate_ex(freerdp* instance,
                               char** username, char** password,
                               char** domain, rdp_auth_reason reason)
{
    qCritical(FreeRDPConnect) << "CConnectFreeRdp::cb_authenticate_ex is not implement";
    Q_ASSERT(false);
    return TRUE;
}
#endif

BOOL CConnectFreeRDP::cb_authenticate(freerdp* instance, char** username,
                                      char** password, char** domain)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_authenticate";
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
            *username = _strdup(pThis->m_pParameter->GetUser().toStdString().c_str());
            *password = _strdup(pThis->m_pParameter->GetPassword().toStdString().c_str());
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
            *username = _strdup(pThis->m_pParameter->GetUser().toStdString().c_str());
            *password = _strdup(pThis->m_pParameter->GetPassword().toStdString().c_str());
        } else
            return FALSE;
    }
	return TRUE;
}

DWORD CConnectFreeRDP::cb_verify_certificate_ex(freerdp *instance,
                       const char *host, UINT16 port,
                       const char *common_name, const char *subject,
                       const char *issuer, const char *fingerprint, DWORD flags)
{
    qDebug(FreeRDPConnect) << "CConnectFreeRdp::cb_verify_certificate_ex";

    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
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

    QString title(tr("Verify certificate"));
    QString message;
    message += tr("Host: %1; Port: %2").arg(host, QString::number(port)) + "\n";
    message += tr("Common name: ") + common_name + "\n";
    message += tr("Subject: ") + subject + "\n";
    message += tr("Issuer: ") + issuer + "\n";
    message += tr("Fingerprint: ") + fingerprint;
    
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

    QString title(tr("Verify changed certificate"));
    QString message;
    message += tr("Host: %1; Port: %2").arg(host, QString::number(port)) + "\n";
    message += tr("Common name: ") + common_name + "\n";
    message += tr("New subject: ") + subject + "\n";
    message += tr("New issuer: ") + issuer + "\n";
    message += tr("New fingerprint: ") + fingerprint + "\n";
    message += tr("Old subject: ") + old_subject + "\n";
    message += tr("Old issuer: ") + old_issuer + "\n";
    message += tr("Old fingerprint: ") + old_fingerprint;

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
    
    QString msgType = (type == GATEWAY_MESSAGE_CONSENT) ? tr("Consent message") : tr("Service message");
    msgType += "\n";
    
    LPSTR msg = nullptr;
    if (ConvertFromUnicode(CP_UTF8, 0, message, (int)(length / 2), &msg, 0, NULL, NULL) < 1)
    {
        qCritical(FreeRDPConnect) << "Failed to convert message!";
        return FALSE;
    }
    msgType += msg;
    msgType += "\n";
    free(msg);
    
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

    //QRect rect(x, y, w, h);
    //qDebug(FreeRDPConnect) << "Update:" << rect;
    //emit sigUpdateRect(rect, m_Image.copy(rect));
    emit sigUpdateRect(m_Image.rect(), m_Image);
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
    return FALSE;
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

void CConnectFreeRDP::slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotWheelEvent";
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
   
    UINT16 flags = 0;
    /*
    if(buttons & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(buttons & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    if(buttons & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    //*/
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

void CConnectFreeRDP::slotMouseMoveEvent(Qt::MouseButtons buttons, QPoint pos)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotMouseMoveEvent";
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    UINT16 flags = PTR_FLAGS_MOVE;
    /*
    if(buttons & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(buttons & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    if(buttons & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;//*/
#if FreeRDP_VERSION_MAJOR >= 3
    freerdp_client_send_button_event(&m_pContext->Context, FALSE, flags,
                                     pos.x(), pos.y());
#else
        freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   pos.x(),
                                   pos.y());
#endif
}

void CConnectFreeRDP::slotMousePressEvent(Qt::MouseButtons buttons, QPoint pos)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotMousePressEvent";
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    UINT16 flags = PTR_FLAGS_DOWN;
    if(buttons & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(buttons & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    if(buttons & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
#if FreeRDP_VERSION_MAJOR >= 3
    freerdp_client_send_button_event(&m_pContext->Context, FALSE, flags,
                                     pos.x(), pos.y());
#else
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   pos.x(),
                                   pos.y());
#endif
}

void CConnectFreeRDP::slotMouseReleaseEvent(Qt::MouseButton button, QPoint pos)
{
    //qDebug(FreeRDPConnect) << "CConnectFreeRDP::slotMouseReleaseEvent";
    if(!m_pContext) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    UINT16 flags = 0;
    if(button & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(button & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    if(button & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    //qDebug(FreeRDPConnect) << "Flags:" << flags;
#if FreeRDP_VERSION_MAJOR >= 3
    freerdp_client_send_button_event(&m_pContext->Context, FALSE, flags,
                                     pos.x(), pos.y());
#else
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   pos.x(),
                                   pos.y());
#endif
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
        freerdp_input_send_keyboard_event_ex(m_pContext->Context.context.input, true, false, k);
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
    freerdp* instance = pRdpContext->instance;
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
    freerdp* instance = pRdpContext->instance;

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
    freerdp* instance = pRdpContext->instance;
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
    freerdp* instance = pRdpContext->instance;
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
    freerdp* instance = pRdpContext->instance;
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
