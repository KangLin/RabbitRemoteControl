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

#include "RabbitCommonTools.h"
#include "RabbitCommonLog.h"
#include "ConvertKeyCode.h"
#include "DlgGetUserPasswordFreeRDP.h"

#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QInputDialog>

CConnectFreeRDP::CConnectFreeRDP(CConnecterFreeRDP *pConnecter,
                                 QObject *parent)
    : CConnect(pConnecter, parent),
      m_pContext(nullptr),
      m_pParamter(nullptr),
      m_pConnecter(pConnecter),
      m_ClipBoard(this),
      m_Cursor(this)
{
    Q_ASSERT(pConnecter);
    m_pParamter = dynamic_cast<CParameterFreeRDP*>(pConnecter->GetParameter());
    Q_ASSERT(m_pParamter);

    //WLog_SetLogLevel(WLog_GetRoot(), WLOG_DEBUG);
    
    ZeroMemory(&m_ClientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	m_ClientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
	m_ClientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
    m_ClientEntryPoints.settings = m_pParamter->m_pSettings;

    rdpSettings* settings = m_pParamter->m_pSettings;
    
    freerdp_settings_set_string(settings,
                                FreeRDP_ServerHostname,
                                m_pParamter->GetHost().toStdString().c_str());
    settings->ServerPort = m_pParamter->GetPort();
    freerdp_settings_set_string(settings,
                                FreeRDP_Username,
                                m_pParamter->GetUser().toStdString().c_str());
    freerdp_settings_set_string(settings,
                                FreeRDP_Password,
                                m_pParamter->GetPassword().toStdString().c_str());

    RdpClientEntry(&m_ClientEntryPoints);
    
    rdpContext* p = freerdp_client_context_new(&m_ClientEntryPoints);
	if(p)
    {
        m_pContext = (ClientContext*)p;
        m_pContext->pThis = this;
    } else 
        LOG_MODEL_ERROR("FreeRdp", "freerdp_client_context_new fail");
    
}

CConnectFreeRDP::~CConnectFreeRDP()
{
    qDebug() << "CConnectFreeRdp::~CConnectFreeRdp()";
}

int CConnectFreeRDP::RdpClientEntry(RDP_CLIENT_ENTRY_POINTS* pEntryPoints)
{
	pEntryPoints->Version = 1;
	pEntryPoints->Size = sizeof(RDP_CLIENT_ENTRY_POINTS_V1);
	pEntryPoints->GlobalInit = Client_global_init;
	pEntryPoints->GlobalUninit = Client_global_uninit;
	pEntryPoints->ContextSize = sizeof(ClientContext);
	pEntryPoints->ClientNew = Client_new;
	pEntryPoints->ClientFree = Client_free;
	pEntryPoints->ClientStart = Client_start;
	pEntryPoints->ClientStop = Client_stop;
	return 0;
}

/*
 * \return 
 * \li < 0: error
 * \li = 0: Use OnProcess (non-Qt event loop)
 * \li > 0: Don't use OnProcess (qt event loop)
 */
int CConnectFreeRDP::OnInit()
{
    qDebug() << "CConnectFreeRdp::Connect()";
    int nRet = 0;
    freerdp* instance = m_pContext->Context.instance;
    
    rdpSettings* settings = instance->settings;
    Q_ASSERT(settings);
    if(nullptr == settings) return -1;
    
    freerdp_client_start(&m_pContext->Context);
    
    BOOL status = freerdp_connect(instance);
    if (!status)
	{
		if (freerdp_get_last_error(instance->context) == FREERDP_ERROR_AUTHENTICATION_FAILED)
			nRet = -2;
		else if (freerdp_get_last_error(instance->context) ==
		         FREERDP_ERROR_SECURITY_NEGO_CONNECT_FAILED)
			nRet = -3;
		else
			nRet = -4;
        QString szErr = tr("freerdp_connect connect to ");
        szErr += freerdp_settings_get_string(settings, FreeRDP_ServerHostname);
        szErr += ":";
        szErr += QString::number(freerdp_settings_get_uint32(settings, FreeRDP_ServerPort));
        szErr += tr(" fail");
        LOG_MODEL_ERROR("FreeRdp", szErr.toStdString().c_str());
        emit sigError(nRet, szErr.toStdString().c_str());
    } else {
        emit sigConnected();

        QString szInfo = tr("Connect to ");
        szInfo += freerdp_settings_get_string(settings, FreeRDP_ServerHostname);
        szInfo += ":";
        szInfo += QString::number(freerdp_settings_get_uint32(settings, FreeRDP_ServerPort));
        LOG_MODEL_INFO("FreeRdp", szInfo.toStdString().c_str());
        emit sigInformation(szInfo);
    }
    return nRet;
}

int CConnectFreeRDP::OnClean()
{
    int nRet = 0;
    if(m_pContext)
    {
        freerdp_abort_connect(m_pContext->Context.instance);
        rdpContext* pContext = (rdpContext*)m_pContext;
        freerdp_disconnect(pContext->instance);
        freerdp_client_stop(pContext);
        freerdp_client_context_free(pContext);
        m_pContext = nullptr;
    }
    return nRet;
}

/*!
 * \~chinese 插件连接的具体操作处理。因为此插件是非Qt事件，所以在此函数中等待。
 * \~english Specific operation processing of plug-in connection.
 *           Because of it is a non-Qt event loop, so wait in here.
 * \~
 * \return 
 *       \li >= 0: continue, Interval call time (msec)
 *       \li  < 0: error or stop
 * \see slotTimeOut()
 */
int CConnectFreeRDP::OnProcess()
{
    int nRet = 0;
    HANDLE handles[64];
    rdpContext* pContext = (rdpContext*)m_pContext;
    if(freerdp_shall_disconnect(pContext->instance))
        return -1;
    else
    {
        DWORD nCount = 0;
        DWORD tmp = freerdp_get_event_handles(pContext, &handles[nCount],
                                              ARRAYSIZE(handles) - nCount);
        if (tmp == 0)
        {
            LOG_MODEL_ERROR("FreeRdp", "freerdp_get_event_handles failed");
            return -2;
        }
        
        nCount += tmp;
        
        handles[nCount++] = pContext->abortEvent;
                
        DWORD waitStatus = WaitForMultipleObjects(nCount, handles, FALSE, 500);
        if (waitStatus == WAIT_FAILED)
            return -3;
        
        /* Check if a processed event called freerdp_abort_connect() and exit if true */
		if (WaitForSingleObject(pContext->abortEvent, 0) == WAIT_OBJECT_0)
			/* Session disconnected by local user action */
            return -4;
        
        if (!freerdp_check_event_handles(pContext))
        {
            nRet = -5;
            // Reconnect
            if(pContext->settings->AutoReconnectionEnabled)
            {
                if (client_auto_reconnect_ex(pContext->instance, NULL))
                    return m_pParamter->GetReconnectInterval() * 1000;
                else
                {
                    /*
                     * Indicate an unsuccessful connection attempt if reconnect
                     * did not succeed and no other error was specified.
                     */
                    if (freerdp_error_info(pContext->instance) == 0)
                        nRet = -6;
                }
                
                if (freerdp_get_last_error(pContext) == FREERDP_ERROR_SUCCESS)
                    LOG_MODEL_ERROR("FreeRdp", "Failed to check FreeRDP file descriptor");
            }
        }
    }

    return nRet;
}

void CConnectFreeRDP::slotClipBoardChange()
{
    if(m_pParamter->GetClipboard())
        m_ClipBoard.slotClipBoardChange();
}

BOOL CConnectFreeRDP::Client_global_init()
{
	qDebug() << "CConnectFreeRdp::Client_global_init()";

	return TRUE;
}

void CConnectFreeRDP::Client_global_uninit()
{
    qDebug() << "CConnectFreeRdp::Client_global_uninit()";
}

BOOL CConnectFreeRDP::Client_new(freerdp *instance, rdpContext *context)
{
    qDebug() << "CConnectFreeRdp::Client_new()";
    instance->PreConnect = cb_pre_connect;
	instance->PostConnect = cb_post_connect;
	instance->PostDisconnect = cb_post_disconnect;
    
    // Because it is setted in settings.
	instance->Authenticate = cb_authenticate;
	instance->GatewayAuthenticate = cb_GatewayAuthenticate;
    
    instance->VerifyCertificateEx = cb_verify_certificate_ex;
	instance->VerifyChangedCertificateEx = cb_verify_changed_certificate_ex;
	instance->PresentGatewayMessage = cb_present_gateway_message;

	instance->LogonErrorInfo = cb_logon_error_info;
	/*PubSub_SubscribeTerminate(context->pubSub, xf_TerminateEventHandler);
#ifdef WITH_XRENDER
	PubSub_SubscribeZoomingChange(context->pubSub, xf_ZoomingChangeEventHandler);
	PubSub_SubscribePanningChange(context->pubSub, xf_PanningChangeEventHandler);
#endif
    */
    return TRUE;
}

void CConnectFreeRDP::Client_free(freerdp *instance, rdpContext *context)
{
    qDebug() << "CConnectFreeRdp::Client_free()";
}

int CConnectFreeRDP::Client_start(rdpContext *context)
{
    int nRet = 0;
    qDebug() << "CConnectFreeRdp::Client_start()";
    return nRet;
}

int CConnectFreeRDP::Client_stop(rdpContext *context)
{
    int nRet = 0;
    qDebug() << "CConnectFreeRdp::Client_stop()";
    return nRet;
}

/**
 * Callback given to freerdp_connect() to process the pre-connect operations.
 * It will fill the rdp_freerdp structure (instance) with the appropriate options to use for the
 * connection.
 *
 * @param instance - pointer to the rdp_freerdp structure that contains the connection's parameters,
 * and will be filled with the appropriate informations.
 *
 * @return TRUE if successful. FALSE otherwise.
 * Can exit with error code XF_EXIT_PARSE_ARGUMENTS if there is an error in the parameters.
 */
BOOL CConnectFreeRDP::cb_pre_connect(freerdp* instance)
{
    qDebug() << "CConnectFreeRdp::cb_pre_connect()";
	rdpChannels* channels = nullptr;
	rdpSettings* settings = nullptr;
	rdpContext* context = instance->context;
    
    if (!instance || !instance->context || !instance->settings)
    { 
        return FALSE;
    }
    
    settings = instance->settings;
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

	if (!freerdp_client_load_addins(channels, instance->settings))
		return FALSE;

	if (!settings->Username && !settings->CredentialsFromStdin && !settings->SmartcardLogon)
	{
        // Get system login name
        QString szUser = RabbitCommon::CTools::Instance()->GetCurrentUser();
        if(!szUser.isEmpty())
            settings->Username = _strdup(szUser.toStdString().c_str());
        if (!settings->Username)
            return FALSE;
        
        LOG_MODEL_ERROR("FreeRdp", "No user name set. - Using login name: %s", settings->Username);
	}

	if (settings->AuthenticationOnly)
	{
		/* Check +auth-only has a username and password. */
		if (!settings->Password)
		{
			LOG_MODEL_ERROR("FreeRdp", "auth-only, but no password set. Please provide one.");
			return FALSE;
		}

		LOG_MODEL_INFO("FreeRdp", "Authentication only. Don't connect to X.");
	}

    // Keyboard layout
#ifdef WIN32
    freerdp_settings_set_uint32(settings, FreeRDP_KeyboardLayout,
                           reinterpret_cast<int>(GetKeyboardLayout(0)) & 0x0000FFFF);
#else
    freerdp_settings_set_uint32(settings, FreeRDP_KeyboardLayout,
                               freerdp_keyboard_init(settings->KeyboardLayout));
#endif

    // Check desktop size
    UINT32 width = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    UINT32 height = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);
#if defined (Q_OS_WIN)
    /* FIXME: desktopWidth has a limitation that it should be divisible by 4,
	 *        otherwise the screen will crash when connecting to an XP desktop.*/
	freerdp_settings_set_uint32(settings, FreeRDP_DesktopWidth, (width + 3) & (~3));
#endif
    if ((width < 64) || (height < 64) ||
            (width > 4096) || (height > 4096))
    {
        LOG_MODEL_ERROR("FreeRdp", "invalid dimensions %d:%d" 
                        , settings->DesktopWidth
                        , settings->DesktopHeight);
        return FALSE;
    } else {
        LOG_MODEL_INFO("FreeRdp", "Init desktop size %d*%d",
                       settings->DesktopWidth,
                       settings->DesktopHeight);
    }
    
	return TRUE;
}

/**
 * Callback given to freerdp_connect() to perform post-connection operations.
 * It will be called only if the connection was initialized properly, and will continue the
 * initialization based on the newly created connection.
 */
BOOL CConnectFreeRDP::cb_post_connect(freerdp* instance)
{
    LOG_MODEL_DEBUG("CConnecctFreeRDP", "CConnectFreeRdp::cb_post_connect()");
	
	rdpContext* context = instance->context;
	rdpSettings* settings = instance->settings;
    rdpUpdate* update = context->update;
    CConnectFreeRDP* pThis = ((ClientContext*)instance->context)->pThis;

    Q_ASSERT(context);
    Q_ASSERT(settings);
    Q_ASSERT(pThis);
    
    const char* pWindowTitle = freerdp_settings_get_string(settings, FreeRDP_WindowTitle);
    if(pWindowTitle)
        emit pThis->sigServerName(pWindowTitle);;
    
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
        LOG_MODEL_ERROR("FreeRdp", "gdi_init fail");
        return FALSE;
    }

    // Register cursor pointer
    if(pThis->m_Cursor.RegisterPointer(context->graphics))
        return FALSE;
    
//	if (!settings->SoftwareGdi)
//	{
//        //pThis->m_Graphics.Registergraphics(context->graphics);
        
//        //TODO: register update callbacks
//		//xf_gdi_register_update_callbacks(update);
//		brush_cache_register_callbacks(instance->update);
//		glyph_cache_register_callbacks(instance->update);
//		bitmap_cache_register_callbacks(instance->update);
//		offscreen_cache_register_callbacks(instance->update);
//		palette_cache_register_callbacks(instance->update);
//	}

    update->EndPaint = cb_end_paint;

//	if (settings->SoftwareGdi)
//	{
//		update->EndPaint = xf_sw_end_paint;
//		update->DesktopResize = xf_sw_desktop_resize;
//	}
//	else
//	{
//		update->EndPaint = xf_hw_end_paint;
//		update->DesktopResize = xf_hw_desktop_resize;
//	}

//	update->PlaySound = xf_play_sound;
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
    QString szErr = tr("Logon error info [");
    szErr += str_type;
    szErr += "] ";
    szErr += str_data;
	LOG_MODEL_ERROR("FreeRdp", szErr.toStdString().c_str());
	emit pThis->sigInformation(szErr);
    emit pThis->sigError(type, szErr);
	return 1;
}

void CConnectFreeRDP::OnChannelConnectedEventHandler(void *context, ChannelConnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    CConnectFreeRDP* pThis = ((ClientContext*)context)->pThis;
    if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
    {
        LOG_MODEL_INFO("FreeRdp", "Channel %s connected", e->name);
        if(pContext->settings->SoftwareGdi) {
            rdpGdi* gdi = pContext->gdi;
            gdi_graphics_pipeline_init(gdi, (RdpgfxClientContext*)e->pInterface);
        } else
            LOG_MODEL_ERROR("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
        
    }else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0)
    {
		LOG_MODEL_INFO("FreeRdp", "channel %s connected", e->name);
        pThis->m_ClipBoard.Init((CliprdrClientContext*)e->pInterface,
                                pThis->m_pParamter->GetClipboard());
	} else  if (strcmp(e->name, RDPEI_DVC_CHANNEL_NAME) == 0)
    {
        LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
    }
	else if (strcmp(e->name, TSMF_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else  if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, ENCOMSP_SVC_CHANNEL_NAME) == 0)
	{
        LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, DISP_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "channel %s connected", e->name);
	}
	else if (strcmp(e->name, GEOMETRY_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, VIDEO_CONTROL_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, VIDEO_DATA_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
    else
        LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
}

void CConnectFreeRDP::OnChannelDisconnectedEventHandler(void *context, ChannelDisconnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    CConnectFreeRDP* pThis = ((ClientContext*)context)->pThis;
    
    if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
    {
        LOG_MODEL_INFO("FreeRdp", "channel %s disconnected", e->name);
        if(pContext->settings->SoftwareGdi) {
            gdi_graphics_pipeline_uninit(pContext->gdi, (RdpgfxClientContext*)e->pInterface);    
        }
    }
    else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "channel %s disconnected", e->name);
        pThis->m_ClipBoard.UnInit((CliprdrClientContext*)e->pInterface,
                                  pThis->m_pParamter->GetClipboard());
	}
    else if (strcmp(e->name, RDPEI_DVC_CHANNEL_NAME) == 0)
    {
        LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s disconnected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, TSMF_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s disconnected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s disconnected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, ENCOMSP_SVC_CHANNEL_NAME) == 0)
	{
        LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s disconnected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, DISP_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "channel %s disconnected", e->name);
	}
	else if (strcmp(e->name, GEOMETRY_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s disconnected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, VIDEO_CONTROL_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s disconnected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, VIDEO_DATA_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_WARNING("FreeRdp", "Unimplemented: channel %s disconnected but we can’t use it\n", e->name);
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

BOOL CConnectFreeRDP::cb_authenticate(freerdp* instance, char** username,
                                      char** password, char** domain)
{
    qDebug() << "CConnectFreeRdp::cb_authenticate";
	if(!instance)
		return FALSE;
    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    if(username || password)
    {
        int nRet = QDialog::Rejected;
        emit pThis->sigBlockShowWidget("CDlgGetUserPasswordFreeRDP", nRet, pThis->m_pParamter);
        if(QDialog::Accepted == nRet)
        {
            *username = _strdup(pThis->m_pParamter->GetUser().toStdString().c_str());
            *password = _strdup(pThis->m_pParamter->GetPassword().toStdString().c_str());
        }
    }
    return TRUE;
}

BOOL CConnectFreeRDP::cb_GatewayAuthenticate(freerdp *instance,
                                char **username, char **password, char **domain)
{
    qDebug() << "CConnectFreeRdp::cb_GatewayAuthenticate";
	if(!instance)
		return FALSE;

    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    if(username || password)
    {
        int nRet = QDialog::Rejected;
        emit pThis->sigBlockShowWidget("CDlgGetUserPasswordFreeRDP", nRet, pThis->m_pParamter);
        if(QDialog::Accepted == nRet)
        {
            *username = _strdup(pThis->m_pParamter->GetUser().toStdString().c_str());
            *password = _strdup(pThis->m_pParamter->GetPassword().toStdString().c_str());
        }
    }
	return TRUE;
}

DWORD CConnectFreeRDP::cb_verify_certificate_ex(freerdp *instance,
                       const char *host, UINT16 port,
                       const char *common_name, const char *subject,
                       const char *issuer, const char *fingerprint, DWORD flags)
{
    qDebug() << "CConnectFreeRdp::cb_verify_certificate_ex";

    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    if(common_name)
    {
        pThis->m_pParamter->SetServerName(common_name);
        emit pThis->sigServerName(common_name);
    }
    
    if(!pThis->m_pParamter->GetShowVerifyDiaglog()) {
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
    bool bCheckBox = 0;
    emit pThis->sigBlockShowMessage(title, message, buttons, nRet, bCheckBox,
                                    tr("Don't show again"));
    pThis->m_pParamter->SetShowVerifyDiaglog(!bCheckBox);
    if(pThis->m_pConnecter)
        emit pThis->m_pConnecter->sigUpdateParamters(pThis->m_pConnecter);
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
    qDebug() << "CConnectFreeRdp::cb_verify_changed_certificate_ex";
    rdpContext* pContext = (rdpContext*)instance->context;
    CConnectFreeRDP* pThis = ((ClientContext*)pContext)->pThis;
    if(common_name)
        emit pThis->sigServerName(common_name);

    if(!pThis->m_pParamter->GetShowVerifyDiaglog()) {
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

    bool bCheckBox = 0;
    QMessageBox::StandardButton nRet = QMessageBox::StandardButton::No;
    QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::Ignore | QMessageBox::No;
    emit pThis->sigBlockShowMessage(title, message, buttons, nRet, bCheckBox,
                                    tr("Don't show again"));
    pThis->m_pParamter->SetShowVerifyDiaglog(!bCheckBox);
    if(pThis->m_pConnecter)
        emit pThis->m_pConnecter->sigUpdateParamters(pThis->m_pConnecter);

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
    qDebug() << "CConnectFreeRdp::cb_present_gateway_message";
    
    return TRUE;
}

BOOL CConnectFreeRDP::cb_end_paint(rdpContext *context)
{
    //qDebug() << "CConnectFreeRdp::cb_end_paint";
    ClientContext* pContext = (ClientContext*)context;
    CConnectFreeRDP* pThis = pContext->pThis;

    emit pThis->sigUpdateRect(pThis->m_Image.rect(), pThis->m_Image);
    return TRUE;
}

void CConnectFreeRDP::slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta)
{
    if(!m_pContext) return;
    if(m_pParamter && m_pParamter->GetOnlyView()) return;
   
    UINT16 flags = 0;
    if(buttons & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(buttons & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    if(buttons & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    
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
    
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   pos.x(),
                                   pos.y());
}

void CConnectFreeRDP::slotMouseMoveEvent(Qt::MouseButtons buttons, QPoint pos)
{
    if(!m_pContext) return;
    if(m_pParamter && m_pParamter->GetOnlyView()) return;
    UINT16 flags = PTR_FLAGS_MOVE;
    if(buttons & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(buttons & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    if(buttons & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   pos.x(),
                                   pos.y());
}

void CConnectFreeRDP::slotMousePressEvent(Qt::MouseButtons buttons, QPoint pos)
{
    if(!m_pContext) return;
    if(m_pParamter && m_pParamter->GetOnlyView()) return;
    UINT16 flags = PTR_FLAGS_DOWN;
    if(buttons & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(buttons & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    if(buttons & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   pos.x(),
                                   pos.y());
}

void CConnectFreeRDP::slotMouseReleaseEvent(Qt::MouseButton button, QPoint pos)
{
    if(!m_pContext) return;
    if(m_pParamter && m_pParamter->GetOnlyView()) return;
    UINT16 flags = 0;
    if(button & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(button & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    if(button & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    //LOG_MODEL_DEBUG("FreeRdp", "Flags: %d", flags);
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   pos.x(),
                                   pos.y());
}

void CConnectFreeRDP::slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers)
{
    if(!m_pContext) return;
    if(m_pParamter && m_pParamter->GetOnlyView()) return;
    // Convert to rdp scan code freerdp/scancode.h
    UINT32 k = CConvertKeyCode::QtToScanCode(key, modifiers);
    if(RDP_SCANCODE_UNKNOWN != key)
        freerdp_input_send_keyboard_event_ex(m_pContext->Context.input, true, k);
}

void CConnectFreeRDP::slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers)
{
    if(!m_pContext) return;
    if(m_pParamter && m_pParamter->GetOnlyView()) return;
    UINT32 k = CConvertKeyCode::QtToScanCode(key, modifiers);
    if(RDP_SCANCODE_UNKNOWN != key)
        freerdp_input_send_keyboard_event_ex(m_pContext->Context.input, false, k);
}
