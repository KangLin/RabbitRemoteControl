//! @author: Kang Lin(kl222@126.com)

#include "ConnectFreeRdp.h"

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
#include "freerdp/locale/keyboard.h"

#include "RabbitCommonTools.h"
#include "RabbitCommonLog.h"
#include "ConvertKeyCode.h"

#include <QDebug>
#include <QApplication>
#include <QScreen>

CConnectFreeRdp::CConnectFreeRdp(rdpSettings *settings,
                                 CFrmViewer *pView,
                                 QObject *parent)
    : CConnect(pView, parent),
      m_pContext(nullptr)
{
    ZeroMemory(&m_ClientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	m_ClientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
	m_ClientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
    m_ClientEntryPoints.settings = settings;
    
    SetParamter(settings);
    RdpClientEntry(&m_ClientEntryPoints);
    
    rdpContext* p = freerdp_client_context_new(&m_ClientEntryPoints);
	if(p)
    {
        m_pContext = (ClientContext*)p;
        
        m_pContext->pThis = this;
    } else 
        LOG_MODEL_ERROR("FreeRdp", "freerdp_client_context_new fail");
    
}

CConnectFreeRdp::~CConnectFreeRdp()
{
    qDebug() << "CConnectFreeRdp::~CConnectFreeRdp()";
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
        
        LOG_MODEL_ERROR("FreeRdp", "freerdp_connect connect to %s:%d fail: %d",
                        settings->ServerHostname,
                        settings->ServerPort,
                        nRet);
    } else {
        LOG_MODEL_INFO("FreeRdp", "Connect to %s:%d",
                       settings->ServerHostname,
                       settings->ServerPort);
        
        emit sigConnected();
        emit sigSetDesktopName(m_szServerName);
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
        DWORD nCount = 0;
        DWORD tmp = freerdp_get_event_handles(pContext, &handles[nCount],
                                              ARRAYSIZE(handles) - nCount);
        if (tmp == 0)
        {
            LOG_MODEL_ERROR("FreeRdp", "freerdp_get_event_handles failed");
            return -1;
        }
        
        nCount += tmp;
        
        handles[nCount++] = pContext->abortEvent;
                
        DWORD waitStatus = WaitForMultipleObjects(nCount, handles, FALSE, INFINITE);
        if (waitStatus == WAIT_FAILED)
            return -2;
        
        
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
                LOG_MODEL_ERROR("FreeRdp", "Failed to check FreeRDP file descriptor");
            
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
        m_pContext = nullptr;
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
    
    // Because settings is set. 
	//instance->Authenticate = cb_authenticate;
	//instance->GatewayAuthenticate = cb_GatewayAuthenticate;
    
    instance->VerifyCertificateEx = cb_verify_certificate_ex;
	instance->VerifyChangedCertificateEx = cb_verify_changed_certificate_ex;
	instance->PresentGatewayMessage = cb_present_gateway_message;

	/* There are is set authenticate callback, the follow is set authenticate in console.
    instance->Authenticate = client_cli_authenticate;
	instance->GatewayAuthenticate = client_cli_gw_authenticate;
	instance->VerifyCertificateEx = client_cli_verify_certificate_ex;
	instance->VerifyChangedCertificateEx = client_cli_verify_changed_certificate_ex;
	instance->PresentGatewayMessage = client_cli_present_gateway_message;
	//*/

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
	CConnectFreeRdp* pThis = (CConnectFreeRdp*)((ClientContext*)instance->context)->pThis;
	UINT32 desktopWidth = 0;
	UINT32 desktopHeight = 0;
    
    if (!instance || !instance->context || !instance->settings)
        return FALSE;
    
	settings = instance->settings;
	channels = context->channels;
#if defined (Q_OS_WIN)
    settings->OsMajorType = OSMAJORTYPE_WINDOWS;
    settings->OsMinorType = OSMINORTYPE_WINDOWS_NT;
#endif
#if defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
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
                                 (int)GetKeyboardLayout(0) & 0x0000FFFF);
#else
    freerdp_settings_set_uint32(settings, FreeRDP_KeyboardLayout,
                                freerdp_keyboard_init(settings->KeyboardLayout));
#endif
    
    // Windows(Monitor) size, beacuse the view is don't initial, so that don't set desktop, use default value.
    //TODO: Move to setting dialog
    desktopWidth = qApp->primaryScreen()->availableVirtualSize().width();
    desktopHeight =  qApp->primaryScreen()->availableVirtualSize().height();
    
    if(settings->DesktopWidth != desktopWidth)
        freerdp_settings_set_uint32(settings, FreeRDP_DesktopWidth, desktopWidth);
    if(settings->DesktopHeight != desktopHeight)
        freerdp_settings_set_uint32(settings, FreeRDP_DesktopHeight, desktopHeight);
    if ((settings->DesktopWidth < 64) || (settings->DesktopHeight < 64) ||
            (settings->DesktopWidth > 4096) || (settings->DesktopHeight > 4096))
    {
        LOG_MODEL_ERROR("FreeRdp", "invalid dimensions %d:%d" 
                        , settings->DesktopWidth
                        , settings->DesktopHeight);
        return FALSE;
    }//*/
    
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
    CConnectFreeRdp* pThis = ((ClientContext*)instance->context)->pThis;

	context = instance->context;
	settings = instance->settings;
	update = context->update;

    Q_ASSERT(context);
    Q_ASSERT(settings);
    
    pThis->m_Image = QImage(settings->DesktopWidth,
                            settings->DesktopHeight,
                            QImage::Format_RGB32);
    
    // Init gdi format
	//if (!gdi_init(instance, pThis->GetImageFormat()))
    if (!gdi_init_ex(instance, pThis->GetImageFormat(),
                     0, pThis->m_Image.bits(), nullptr))
	{
        LOG_MODEL_ERROR("FreeRdp", "gdi_init fail");
        return FALSE;
    }

    // TODO: Register cursor pointer
//	if (!xf_register_pointer(context->graphics))
//		return FALSE;

    // pointer_cache_register_callbacks(instance->update);
    
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
	LOG_MODEL_ERROR("FreeRdp", "Logon Error Info %s [%s]", str_data, str_type);
	//xf_rail_disable_remoteapp_mode(xfc);
	return 1;
}

void CConnectFreeRdp::OnChannelConnectedEventHandler(void *context, ChannelConnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    CConnectFreeRdp* pThis = ((ClientContext*)context)->pThis;
    if (strcmp(e->name, RDPEI_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, TSMF_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
	{
        LOG_MODEL_INFO("FreeRdp", "Channel %s connected", e->name);
        if(pContext->settings->SoftwareGdi) {
            rdpGdi* gdi = pContext->gdi;
            gdi_graphics_pipeline_init(gdi, (RdpgfxClientContext*)e->pInterface);
        } else
            LOG_MODEL_ERROR("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
            
	}
	else if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "channel %s connected", e->name);
        pThis->m_ClipBoard.Init((CliprdrClientContext*)e->pInterface);
	}
	else if (strcmp(e->name, ENCOMSP_SVC_CHANNEL_NAME) == 0)
	{
        LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, DISP_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "channel %s connected", e->name);
	}
	else if (strcmp(e->name, GEOMETRY_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, VIDEO_CONTROL_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, VIDEO_DATA_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
    else
        LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
}

void CConnectFreeRdp::OnChannelDisconnectedEventHandler(void *context, ChannelDisconnectedEventArgs *e)
{
    rdpContext* pContext = (rdpContext*)context;
    CConnectFreeRdp* pThis = ((ClientContext*)context)->pThis;
    
    if (strcmp(e->name, RDPEI_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, TSMF_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, RDPGFX_DVC_CHANNEL_NAME) == 0)
	{
        LOG_MODEL_INFO("FreeRdp", "channel %s connected", e->name);
        if(pContext->settings->SoftwareGdi) {
            gdi_graphics_pipeline_uninit(pContext->gdi, (RdpgfxClientContext*)e->pInterface);    
        }
	}
	else if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "channel %s connected", e->name);
        pThis->m_ClipBoard.UnInit((CliprdrClientContext*)e->pInterface);
	}
	else if (strcmp(e->name, ENCOMSP_SVC_CHANNEL_NAME) == 0)
	{
        LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, DISP_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "channel %s connected", e->name);
	}
	else if (strcmp(e->name, GEOMETRY_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, VIDEO_CONTROL_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
	else if (strcmp(e->name, VIDEO_DATA_DVC_CHANNEL_NAME) == 0)
	{
		LOG_MODEL_INFO("FreeRdp", "Unimplemented: channel %s connected but we can’t use it\n", e->name);
	}
}

UINT32 CConnectFreeRdp::GetImageFormat()
{
    switch (m_Image.format()) {
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

BOOL CConnectFreeRdp::cb_authenticate(freerdp* instance, char** username, char** password, char** domain)
{
    qDebug() << "CConnectFreeRdp::cb_authenticate";
	if(!instance)
		return FALSE;

    if(username)
        *username = _strdup(freerdp_settings_get_string(instance->settings, FreeRDP_Username));
    if(password)
	    *password = _strdup(freerdp_settings_get_string(instance->settings, FreeRDP_Password));
    if(domain)
        *domain = _strdup(freerdp_settings_get_string(instance->settings, FreeRDP_Domain));
	
	return TRUE;
}

BOOL CConnectFreeRdp::cb_GatewayAuthenticate(freerdp *instance, char **username, char **password, char **domain)
{
    qDebug() << "CConnectFreeRdp::cb_GatewayAuthenticate";
	if(!instance)
		return FALSE;

    if(username)
        *username = _strdup(freerdp_settings_get_string(instance->settings, FreeRDP_Username));
    if(password)
	    *password = _strdup(freerdp_settings_get_string(instance->settings, FreeRDP_Password));
    if(domain)
        *domain = _strdup(freerdp_settings_get_string(instance->settings, FreeRDP_Domain));

	return TRUE;
}

DWORD CConnectFreeRdp::cb_verify_certificate_ex(freerdp *instance, const char *host, UINT16 port, const char *common_name, const char *subject, const char *issuer, const char *fingerprint, DWORD flags)
{
    qDebug() << "CConnectFreeRdp::cb_verify_certificate_ex";
    
    /* return 1 to accept and store a certificate, 2 to accept
	 * a certificate only for this session, 0 otherwise */
    return 1;
}

DWORD CConnectFreeRdp::cb_verify_changed_certificate_ex(freerdp *instance, const char *host, UINT16 port, const char *common_name, const char *subject, const char *issuer, const char *new_fingerprint, const char *old_subject, const char *old_issuer, const char *old_fingerprint, DWORD flags)
{
    qDebug() << "CConnectFreeRdp::cb_verify_changed_certificate_ex";
    
    /* return 1 to accept and store a certificate, 2 to accept
	 * a certificate only for this session, 0 otherwise */
    return 1;
}

BOOL CConnectFreeRdp::cb_present_gateway_message(freerdp* instance, UINT32 type, BOOL isDisplayMandatory,
                                                  BOOL isConsentMandatory, size_t length, const WCHAR* message)
{
    qDebug() << "CConnectFreeRdp::cb_present_gateway_message";
    
    return TRUE;
}

BOOL CConnectFreeRdp::cb_end_paint(rdpContext *context)
{
    //qDebug() << "CConnectFreeRdp::cb_end_paint";
    ClientContext* pContext = (ClientContext*)context;
    CConnectFreeRdp* pThis = pContext->pThis;

    emit pThis->sigUpdateRect(pThis->m_Image.rect(), pThis->m_Image);
    return TRUE;
}

void CConnectFreeRdp::slotWheelEvent(QWheelEvent* e)
{
    UINT16 flags = 0;
   
    if(e->buttons() & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(e->buttons() & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    if(e->buttons() & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    
    QPoint p = e->angleDelta();
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
                                   e->x(),
                                   e->y());
}

void CConnectFreeRdp::slotMouseMoveEvent(QMouseEvent* e)
{
    UINT16 flags = PTR_FLAGS_MOVE;
    if(e->button() & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(e->button() & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    if(e->button() & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   e->x(),
                                   e->y());
}

void CConnectFreeRdp::slotMousePressEvent(QMouseEvent* e)
{
    UINT16 flags = PTR_FLAGS_DOWN;
    if(e->button() & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(e->button() & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    if(e->button() & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   e->x(),
                                   e->y());
}

void CConnectFreeRdp::slotMouseReleaseEvent(QMouseEvent* e)
{
    UINT16 flags = 0;
    if(e->button() & Qt::MouseButton::LeftButton)
        flags |= PTR_FLAGS_BUTTON1;
    if(e->button() & Qt::MouseButton::MiddleButton)
        flags |= PTR_FLAGS_BUTTON3;
    if(e->button() & Qt::MouseButton::RightButton)
        flags |= PTR_FLAGS_BUTTON2;
    //LOG_MODEL_DEBUG("FreeRdp", "Flags: %d", flags);
    freerdp_input_send_mouse_event(m_pContext->Context.input,
                                   flags,
                                   e->x(),
                                   e->y());
}

void CConnectFreeRdp::slotKeyPressEvent(QKeyEvent* e)
{
    
    // Convert to rdp scan code freerdp/scancode.h
#if defined (Q_OS_WIN)
    freerdp_input_send_keyboard_event_ex(m_pContext->Context.input,
                                         true, e->nativeScanCode());
#else
    freerdp_input_send_keyboard_event_ex(m_pContext->Context.input,
                                         true,
                                         CConvertKeyCode::QtToScanCode(e->key()));
#endif
}

void CConnectFreeRdp::slotKeyReleaseEvent(QKeyEvent* e)
{
    //TODO: Key 
    qDebug() << "Key:" << e->key() << e->nativeScanCode() << e->nativeVirtualKey() << e->nativeModifiers();
#if defined (Q_OS_WIN)
    freerdp_input_send_keyboard_event_ex(m_pContext->Context.input,
                                         false, e->nativeScanCode());
#else
    freerdp_input_send_keyboard_event_ex(m_pContext->Context.input,
                                         false,
                                         CConvertKeyCode::QtToScanCode(e->key()));
#endif
}

int CConnectFreeRdp::SetParamter(void *pPara)
{
    Q_ASSERT(pPara);
    if(!pPara) return -1;
    
    rdpSettings* settings = (rdpSettings*)pPara;
    
    SetUser(settings->Username,
            settings->Password);
    SetServerName(QString(settings->ServerHostname)
                  + ":" + QString::number(settings->ServerPort));
    
    return 0;
}
