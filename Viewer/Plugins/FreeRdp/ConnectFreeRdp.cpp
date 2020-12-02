#include "ConnectFreeRdp.h"

#include "log4cplus/loggingmacros.h"
#include "freerdp/client/cmdline.h"
#include "freerdp/channels/rdpei.h"
#include "freerdp/channels/rdpdr.h"
#include "freerdp/channels/rdpgfx.h"
#include "freerdp/channels/rdpsnd.h"
#include "freerdp/channels/cliprdr.h"
#include "freerdp/gdi/gfx.h"
#include "freerdp/client/encomsp.h"
#include "freerdp/channels/disp.h"
#include "freerdp/channels/tsmf.h"

#include <QDebug>

static log4cplus::Logger g_logger;

CConnectFreeRdp::CConnectFreeRdp(CFrmViewer *pView, QObject *parent)
    : CConnect(pView, parent),
      m_pContext(nullptr),
      m_pSettings(nullptr)
{
    g_logger = log4cplus::Logger::getInstance("FreeRdp");
    
    ZeroMemory(&m_ClientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	m_ClientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
	m_ClientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
    
    RdpClientEntry(&m_ClientEntryPoints);
    
    rdpContext* p = freerdp_client_context_new(&m_ClientEntryPoints);
	if(p)
    {
        m_pContext = (ClientContext*)p;
        
        m_pContext->pConnect = this;
        m_pSettings = m_pContext->Context.settings;
    } else 
        LOG4CPLUS_ERROR(g_logger, "freerdp_client_context_new fail");
    
    //TODO:
    this->SetServerName("172.17.0.1");
}

CConnectFreeRdp::~CConnectFreeRdp()
{
    qDebug() << "CConnectFreeRdp::~CConnectFreeRdp()";
    if(m_pContext)
    {
        freerdp_client_context_free((rdpContext*)m_pContext);
        m_pContext = nullptr;
    }
}

int CConnectFreeRdp::RdpClientEntry(RDP_CLIENT_ENTRY_POINTS* pEntryPoints)
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

int CConnectFreeRdp::Connect()
{
    qDebug() << "CConnectFreeRdp::Connect()";
    int nRet = 0;
    freerdp* instance = m_pContext->Context.instance;
    Q_ASSERT(m_pSettings);
    if(nullptr == m_pSettings) return -1;

    if(!m_szHost.isEmpty())
        m_pSettings->ServerHostname = _strdup(m_szHost.toStdString().c_str());
    if(m_nPort)
        m_pSettings->ServerPort = static_cast<UINT32>(m_nPort);
    if(!m_szUser.isEmpty())
        m_pSettings->Username = _strdup(m_szUser.toStdString().c_str());
    if(!m_szPassword.isEmpty())
        m_pSettings->Password = _strdup(m_szPassword.toStdString().c_str());
    
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
        
        LOG4CPLUS_ERROR(g_logger, "freerdp_connect fail:" << nRet);
	}
    
    return nRet;
}

int CConnectFreeRdp::Disconnect()
{
    int nRet = 0;
    freerdp_abort_connect(m_pContext->Context.instance);
    return nRet;
}

int CConnectFreeRdp::Process()
{
    int nRet = 0;
    HANDLE handles[64];
    rdpContext* pContext = (rdpContext*)m_pContext;
    if(!freerdp_shall_disconnect(pContext->instance))
    {
        wMessage msg;
        wMessageQueue* queue = freerdp_get_message_queue(pContext->instance, FREERDP_INPUT_MESSAGE_QUEUE);;
        DWORD nCount = 0;
        //handles[nCount++] = MessageQueue_Event(queue);
        DWORD tmp = freerdp_get_event_handles(pContext, &handles[nCount],
                                              ARRAYSIZE(handles) - nCount);
        if (tmp == 0)
        {
            LOG4CPLUS_ERROR(g_logger, "freerdp_get_event_handles failed");
            return -1;
        }
        
        nCount += tmp;
        
        handles[nCount++] = pContext->abortEvent;
                
        DWORD waitStatus = WaitForMultipleObjects(nCount, handles, FALSE, INFINITE);
        if (waitStatus == WAIT_FAILED)
            return -2;
        
        if (WaitForSingleObject(handles[0], 0) == WAIT_OBJECT_0)
        {
            if (MessageQueue_Peek(queue, &msg, FALSE))
            {
                if (msg.id == WMQ_QUIT)
                    return 1;
            }
        }
        
        /* Check if a processed event called freerdp_abort_connect() and exit if true */
		if (WaitForSingleObject(pContext->abortEvent, 0) == WAIT_OBJECT_0)
			/* Session disconnected by local user action */
            return 1;
        
        if (!freerdp_check_event_handles(pContext))
        {
            nRet = -4;
            //TODO: add reconnect
//            if (client_auto_reconnect_ex(pContext, NULL))
//                continue;
//            else
//            {
//                /*
//                         * Indicate an unsuccessful connection attempt if reconnect
//                         * did not succeed and no other error was specified.
//                         */
//                if (freerdp_error_info(pContext->instance) == 0)
//                    exit_code = XF_EXIT_CONN_FAILED;
//            }
            
            if (freerdp_get_last_error(pContext) == FREERDP_ERROR_SUCCESS)
                LOG4CPLUS_ERROR(g_logger, "Failed to check FreeRDP file descriptor");
            
        }
    }

    return nRet;
}

int CConnectFreeRdp::Clean()
{
    if(m_pContext)
    {
        rdpContext* pContext = (rdpContext*)m_pContext;
        freerdp_disconnect(pContext->instance);
        freerdp_client_stop(pContext);
        freerdp_client_context_free(pContext);
    }
    return 0;
}

void CConnectFreeRdp::slotClipBoardChange()
{
}

BOOL CConnectFreeRdp::Client_global_init()
{
	qDebug() << "CConnectFreeRdp::Client_global_init()";

	return TRUE;
}

void CConnectFreeRdp::Client_global_uninit()
{
    qDebug() << "CConnectFreeRdp::Client_global_uninit()";
}

BOOL CConnectFreeRdp::Client_new(freerdp *instance, rdpContext *context)
{
    qDebug() << "CConnectFreeRdp::Client_new()";
    instance->PreConnect = cb_pre_connect;
	instance->PostConnect = cb_post_connect;
	instance->PostDisconnect = cb_post_disconnect;
    
	instance->Authenticate = client_cli_authenticate;
	instance->GatewayAuthenticate = client_cli_gw_authenticate;
    
    // TODO: Is console?
    //if(isConsole) {
	instance->VerifyCertificateEx = client_cli_verify_certificate_ex;
	instance->VerifyChangedCertificateEx = client_cli_verify_changed_certificate_ex;
	instance->PresentGatewayMessage = client_cli_present_gateway_message;
    //} else {
    //}
    
	instance->LogonErrorInfo = cb_logon_error_info;
	/*PubSub_SubscribeTerminate(context->pubSub, xf_TerminateEventHandler);
#ifdef WITH_XRENDER
	PubSub_SubscribeZoomingChange(context->pubSub, xf_ZoomingChangeEventHandler);
	PubSub_SubscribePanningChange(context->pubSub, xf_PanningChangeEventHandler);
#endif
    */
    return TRUE;
}

void CConnectFreeRdp::Client_free(freerdp *instance, rdpContext *context)
{
    qDebug() << "CConnectFreeRdp::Client_free()";
}

int CConnectFreeRdp::Client_start(rdpContext *context)
{
    int nRet = 0;
    qDebug() << "CConnectFreeRdp::Client_start()";
    return nRet;
}

int CConnectFreeRdp::Client_stop(rdpContext *context)
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
BOOL CConnectFreeRdp::cb_pre_connect(freerdp* instance)
{
    qDebug() << "CConnectFreeRdp::cb_pre_connect()";
	rdpChannels* channels = nullptr;
	rdpSettings* settings = nullptr;
	rdpContext* context = instance->context;
	CConnectFreeRdp* pThis = (CConnectFreeRdp*)((ClientContext*)instance->context)->pConnect;
	UINT32 desktopWidth = 0;
	UINT32 desktopHeight = 0;
    
    if (!instance || !instance->context || !instance->settings)
            return FALSE;
    
	settings = instance->settings;
	channels = context->channels;
#ifdef WIN32
    settings->OsMajorType = OSMAJORTYPE_WINDOWS;
    settings->OsMinorType = OSMINORTYPE_WINDOWS_NT;
#endif
#if defined(UNIX) || defined(LINUX)
	settings->OsMajorType = OSMAJORTYPE_UNIX;
	settings->OsMinorType = OSMINORTYPE_NATIVE_XSERVER;
#endif
        
    // Subscribe channel event
	PubSub_SubscribeChannelConnected(instance->context->pubSub, OnChannelConnectedEventHandler);
	PubSub_SubscribeChannelDisconnected(instance->context->pubSub,
	                                    OnChannelDisconnectedEventHandler);

	if (!freerdp_client_load_addins(channels, instance->settings))
		return FALSE;

	if (!settings->Username && !settings->CredentialsFromStdin && !settings->SmartcardLogon)
	{
		int rc;
		char login_name[MAX_PATH] = { 0 };

        // TODO: Get system login name
        /*
#ifdef HAVE_GETLOGIN_R
		rc = getlogin_r(login_name, sizeof(login_name));
#else
		strncpy(login_name, getlogin(), sizeof(login_name));
		rc = 0;
#endif
		if (rc == 0)
		{
			settings->Username = _strdup(login_name);

			if (!settings->Username)
				return FALSE;

			WLog_INFO(TAG, "No user name set. - Using login name: %s", settings->Username);
		}*/
	}

	if (settings->AuthenticationOnly)
	{
		/* Check +auth-only has a username and password. */
		if (!settings->Password)
		{
			LOG4CPLUS_ERROR(g_logger, "auth-only, but no password set. Please provide one.");
			return FALSE;
		}

		LOG4CPLUS_INFO(g_logger, "Authentication only. Don't connect to X.");
	}

    //TODO: Keyboard layout
    
    // Windows(Monitor) size, beacuse the view is don't initial, so that don't set desktop, use default value.
//    desktopWidth = pThis->m_pView->width();
//    desktopHeight = pThis->m_pView->height();

//    if(settings->DesktopWidth != desktopWidth)
//        freerdp_settings_set_uint32(settings, FreeRDP_DesktopWidth, desktopWidth);
//    if(settings->DesktopHeight != desktopHeight)
//        freerdp_settings_set_uint32(settings, FreeRDP_DesktopHeight, desktopHeight);
//    if ((settings->DesktopWidth < 64) || (settings->DesktopHeight < 64) ||
//            (settings->DesktopWidth > 4096) || (settings->DesktopHeight > 4096))
//    {
//        LOG4CPLUS_ERROR(g_logger, "invalid dimensions" 
//                        << settings->DesktopWidth
//                        << settings->DesktopHeight);
//        return FALSE;
//    }
    
	return TRUE;
}

/**
 * Callback given to freerdp_connect() to perform post-connection operations.
 * It will be called only if the connection was initialized properly, and will continue the
 * initialization based on the newly created connection.
 */
BOOL CConnectFreeRdp::cb_post_connect(freerdp* instance)
{
    qDebug() << "CConnectFreeRdp::cb_post_connect()";
	rdpUpdate* update;
	rdpContext* context;
	rdpSettings* settings;
	ResizeWindowEventArgs e;
    CConnectFreeRdp* pThis = (CConnectFreeRdp*)instance->context;

	context = instance->context;
	settings = instance->settings;
	update = context->update;

    //TODO:Set desktop width and height
    
    // Init gdi format
	if (!gdi_init(instance, pThis->GetImageFormat()))
		return FALSE;

    // Register cursor pointer
//	if (!xf_register_pointer(context->graphics))
//		return FALSE;

    // pointer_cache_register_callbacks(instance->update);
    
	if (!settings->SoftwareGdi)
	{
//		if (!xf_register_graphics(context->graphics))
//		{
//			WLog_ERR(TAG, "failed to register graphics");
//			return FALSE;
//		}

//		xf_gdi_register_update_callbacks(update);
		brush_cache_register_callbacks(instance->update);
		glyph_cache_register_callbacks(instance->update);
		bitmap_cache_register_callbacks(instance->update);
		offscreen_cache_register_callbacks(instance->update);
		palette_cache_register_callbacks(instance->update);
	}

#ifdef WITH_XRENDER
	xfc->scaledWidth = settings->DesktopWidth;
	xfc->scaledHeight = settings->DesktopHeight;
	xfc->offset_x = 0;
	xfc->offset_y = 0;
#endif

//	if (!xfc->xrenderAvailable)
//	{
//		if (settings->SmartSizing)
//		{
//			WLog_ERR(TAG, "XRender not available: disabling smart-sizing");
//			settings->SmartSizing = FALSE;
//		}

//		if (settings->MultiTouchGestures)
//		{
//			WLog_ERR(TAG, "XRender not available: disabling local multi-touch gestures");
//			settings->MultiTouchGestures = FALSE;
//		}
//	}

//	if (settings->RemoteApplicationMode)
//		xfc->remote_app = TRUE;

//	if (!xf_create_window(xfc))
//	{
//		WLog_ERR(TAG, "xf_create_window failed");
//		return FALSE;
//	}

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

void CConnectFreeRdp::cb_post_disconnect(freerdp* instance)
{
	rdpContext* context;

	if (!instance || !instance->context)
		return;

	context = instance->context;
	
	PubSub_UnsubscribeChannelConnected(instance->context->pubSub,
	                                   OnChannelConnectedEventHandler);
	PubSub_UnsubscribeChannelDisconnected(instance->context->pubSub,
	                                      OnChannelDisconnectedEventHandler);
	gdi_free(instance);

//	if (xfc->clipboard)
//	{
//		xf_clipboard_free(xfc->clipboard);
//		xfc->clipboard = NULL;
//	}

//	if (xfc->xfDisp)
//	{
//		xf_disp_free(xfc->xfDisp);
//		xfc->xfDisp = NULL;
//	}

//	if ((xfc->window != NULL) && (xfc->drawable == xfc->window->handle))
//		xfc->drawable = 0;
//	else
//		xf_DestroyDummyWindow(xfc, xfc->drawable);

//	xf_window_free(xfc);
//	xf_keyboard_free(xfc);
}

int CConnectFreeRdp::cb_logon_error_info(freerdp* instance, UINT32 data, UINT32 type)
{
	//xfContext* xfc = (xfContext*)instance->context;
	const char* str_data = freerdp_get_logon_error_info_data(data);
	const char* str_type = freerdp_get_logon_error_info_type(type);
	LOG4CPLUS_ERROR(g_logger, "Logon Error Info " << str_data << "[" << str_type << "]");
	//xf_rail_disable_remoteapp_mode(xfc);
	return 1;
}

void CConnectFreeRdp::OnChannelConnectedEventHandler(void *context, ChannelConnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    if (strcmp(e->name, RDPEI_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, TSMF_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
	{
        if(pContext->settings->SoftwareGdi) {
            
        }
	}
	else if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0)
	{
		
	}
	else if (strcmp(e->name, ENCOMSP_SVC_CHANNEL_NAME) == 0)
	{
        LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, DISP_DVC_CHANNEL_NAME) == 0)
	{
		
	}
	else if (strcmp(e->name, GEOMETRY_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, VIDEO_CONTROL_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, VIDEO_DATA_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
}

void CConnectFreeRdp::OnChannelDisconnectedEventHandler(void *context, ChannelDisconnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    if (strcmp(e->name, RDPEI_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, TSMF_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
	{
        if(pContext->settings->SoftwareGdi) {
            
        }
	}
	else if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0)
	{
		
	}
	else if (strcmp(e->name, ENCOMSP_SVC_CHANNEL_NAME) == 0)
	{
        LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, DISP_DVC_CHANNEL_NAME) == 0)
	{
		
	}
	else if (strcmp(e->name, GEOMETRY_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, VIDEO_CONTROL_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
	else if (strcmp(e->name, VIDEO_DATA_DVC_CHANNEL_NAME) == 0)
	{
		LOG4CPLUS_INFO(g_logger, "Unimplemented: channel %s connected but we can’t use it\n" << e->name);
	}
}

UINT32 CConnectFreeRdp::GetImageFormat()
{
	if (m_Image.isNull())
		return 0;

    switch (m_Image.format()) {
    case QImage::Format_RGB32:
        return PIXEL_FORMAT_RGBA32;
    case QImage::Format_RGB888:
        return PIXEL_FORMAT_RGB24;
    case QImage::Format_RGB16:
        return PIXEL_FORMAT_RGB16;
    case QImage::Format_Mono:
        return PIXEL_FORMAT_MONO;
    
    }
    return PIXEL_FORMAT_RGBA32;
}
