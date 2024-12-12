#include <QLoggingCategory>

#include "Rail.h"
#include "ConnectFreeRDP.h"

static Q_LOGGING_CATEGORY(log, "FreeRDP.Rail")
static Q_LOGGING_CATEGORY(TAG, "FreeRDP.Rail.Tag")

CRail::CRail(CConnectFreeRDP* pConnect, QObject *parent)
    : QObject{parent}
    , m_pConnect(pConnect)
{
    qDebug(log) << "CRail::CRail()";
}

CRail::~CRail()
{
    qDebug(log) << "CRail::~CRail()";
}

int CRail::Init(RailClientContext *pContext)
{
    qDebug(log) << "CRail::Init()";
    pContext->custom = this;
    pContext->ServerExecuteResult = cbServerExecuteResult;
    pContext->ServerSystemParam = cbServerSystemParam;
    pContext->ServerHandshake = cbServerHandshake;
    pContext->ServerHandshakeEx = cbServerHandshakeEx;
    pContext->ServerLocalMoveSize = cbServerLocalMoveSize;
    pContext->ServerMinMaxInfo = cbServerMinMaxInfo;
    pContext->ServerLanguageBarInfo = cbServerLanguageBarInfo;
    pContext->ServerGetAppIdResponse = cbServerGetAppIdResponse;

    Q_ASSERT(m_pConnect && m_pConnect->m_pContext);
    rdpContext* p = (rdpContext*)m_pConnect->m_pContext;
    RegisterUpdateCallbacks(p->update);
    return 0;
}

int CRail::Clean(RailClientContext *pContext)
{
    qDebug(log) << "CRail::Clean()";
    return 0;
}

UINT CRail::cbServerExecuteResult(
    RailClientContext *context,
    const RAIL_EXEC_RESULT_ORDER *execResult)
{
    qDebug(log) << Q_FUNC_INFO;
    return CHANNEL_RC_OK;
}

UINT CRail::cbServerSystemParam(
    RailClientContext *context,
    const RAIL_SYSPARAM_ORDER *sysparam)
{
    qDebug(log) << Q_FUNC_INFO;
    return CHANNEL_RC_OK;
}

UINT CRail::ServerStartCmd(RailClientContext* context)
{
    UINT status;
    RAIL_EXEC_ORDER exec = { 0 };
    RAIL_SYSPARAM_ORDER sysparam = { 0 };
    RAIL_CLIENT_STATUS_ORDER clientStatus = { 0 };

    rdpContext* pContext = (rdpContext*)m_pConnect->m_pContext;
    rdpSettings* settings = pContext->settings;

    clientStatus.flags = TS_RAIL_CLIENTSTATUS_ALLOWLOCALMOVESIZE;

    if (settings->AutoReconnectionEnabled)
        clientStatus.flags |= TS_RAIL_CLIENTSTATUS_AUTORECONNECT;

    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_ZORDER_SYNC;
    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_WINDOW_RESIZE_MARGIN_SUPPORTED;
    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_APPBAR_REMOTING_SUPPORTED;
    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_POWER_DISPLAY_REQUEST_SUPPORTED;
    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_BIDIRECTIONAL_CLOAK_SUPPORTED;
    status = context->ClientInformation(context, &clientStatus);

    if (status != CHANNEL_RC_OK)
        return status;

    if (settings->RemoteAppLanguageBarSupported)
    {
        RAIL_LANGBAR_INFO_ORDER langBarInfo;
        langBarInfo.languageBarStatus = 0x00000008; /* TF_SFT_HIDDEN */
        status = context->ClientLanguageBarInfo(context, &langBarInfo);

        /* We want the language bar, but the server might not support it. */
        switch (status)
        {
        case CHANNEL_RC_OK:
        case ERROR_BAD_CONFIGURATION:
            break;
        default:
            return status;
        }
    }

    sysparam.params = 0;
    sysparam.params |= SPI_MASK_SET_HIGH_CONTRAST;
    sysparam.highContrast.colorScheme.string = NULL;
    sysparam.highContrast.colorScheme.length = 0;
    sysparam.highContrast.flags = 0x7E;
    sysparam.params |= SPI_MASK_SET_MOUSE_BUTTON_SWAP;
    sysparam.mouseButtonSwap = FALSE;
    sysparam.params |= SPI_MASK_SET_KEYBOARD_PREF;
    sysparam.keyboardPref = FALSE;
    sysparam.params |= SPI_MASK_SET_DRAG_FULL_WINDOWS;
    sysparam.dragFullWindows = FALSE;
    sysparam.params |= SPI_MASK_SET_KEYBOARD_CUES;
    sysparam.keyboardCues = FALSE;
    sysparam.params |= SPI_MASK_SET_WORK_AREA;
    sysparam.workArea.left = 0;
    sysparam.workArea.top = 0;
    sysparam.workArea.right = settings->DesktopWidth;
    sysparam.workArea.bottom = settings->DesktopHeight;
    sysparam.dragFullWindows = FALSE;
    status = context->ClientSystemParam(context, &sysparam);

    if (status != CHANNEL_RC_OK)
        return status;

    exec.RemoteApplicationProgram = settings->RemoteApplicationProgram;
    exec.RemoteApplicationWorkingDir = settings->ShellWorkingDirectory;
    exec.RemoteApplicationArguments = settings->RemoteApplicationCmdLine;
    return context->ClientExecute(context, &exec);
}

UINT CRail::cbServerHandshake(
    RailClientContext *context,
    const RAIL_HANDSHAKE_ORDER *handshake)
{
    qDebug(log) << Q_FUNC_INFO;
#if FreeRDP_VERSION_MAJOR >= 3
    return client_rail_server_start_cmd(context);
#else
    CRail* pRail = (CRail*)context->custom;
    return pRail->ServerStartCmd(context);
#endif

}

UINT CRail::cbServerHandshakeEx(
    RailClientContext *context,
    const RAIL_HANDSHAKE_EX_ORDER *handshakeEx)
{
    qDebug(log) << Q_FUNC_INFO;
#if FreeRDP_VERSION_MAJOR >= 3
    return client_rail_server_start_cmd(context);
#else
    CRail* pRail = (CRail*)context->custom;
    return pRail->ServerStartCmd(context);
#endif
}

UINT CRail::cbServerLocalMoveSize(
    RailClientContext *context,
    const RAIL_LOCALMOVESIZE_ORDER *localMoveSize)
{
    qDebug(log) << Q_FUNC_INFO;
    return CHANNEL_RC_OK;
}

UINT CRail::cbServerMinMaxInfo(
    RailClientContext *context,
    const RAIL_MINMAXINFO_ORDER *minMaxInfo)
{
    qDebug(log) << Q_FUNC_INFO;
    return CHANNEL_RC_OK;
}

UINT CRail::cbServerLanguageBarInfo(
    RailClientContext *context,
    const RAIL_LANGBAR_INFO_ORDER *langBarInfo)
{
    qDebug(log) << Q_FUNC_INFO;
    return CHANNEL_RC_OK;
}

UINT CRail::cbServerGetAppIdResponse(
    RailClientContext *context,
    const RAIL_GET_APPID_RESP_ORDER *getAppIdResp)
{
    qDebug(log) << Q_FUNC_INFO;
    return CHANNEL_RC_OK;
}

void CRail::RegisterUpdateCallbacks(rdpUpdate* update)
{
    rdpWindowUpdate* window = update->window;
    window->WindowCreate = cbWindowCommon;
    window->WindowUpdate = cbWindowCommon;
    window->WindowDelete = cbWindowDelete;
    window->WindowIcon = cbWindowIcon;
    window->WindowCachedIcon = cbWindowCachedIcon;
    window->NotifyIconCreate = cbNotifyIconCreate;
    window->NotifyIconUpdate = cbNotifyIconUpdate;
    window->NotifyIconDelete = cbNotifyIconDelete;
    window->MonitoredDesktop = cbMonitoredDesktop;
    window->NonMonitoredDesktop = cbNonMonitoredDesktop;
}

typedef struct
{
    UINT32 style;
    const char* name;
    BOOL multi;
} WINDOW_STYLE;

static const WINDOW_STYLE WINDOW_STYLES[] = {
    { WS_BORDER, "WS_BORDER", FALSE },
    { WS_CAPTION, "WS_CAPTION", FALSE },
    { WS_CHILD, "WS_CHILD", FALSE },
    { WS_CLIPCHILDREN, "WS_CLIPCHILDREN", FALSE },
    { WS_CLIPSIBLINGS, "WS_CLIPSIBLINGS", FALSE },
    { WS_DISABLED, "WS_DISABLED", FALSE },
    { WS_DLGFRAME, "WS_DLGFRAME", FALSE },
    { WS_GROUP, "WS_GROUP", FALSE },
    { WS_HSCROLL, "WS_HSCROLL", FALSE },
    { WS_ICONIC, "WS_ICONIC", FALSE },
    { WS_MAXIMIZE, "WS_MAXIMIZE", FALSE },
    { WS_MAXIMIZEBOX, "WS_MAXIMIZEBOX", FALSE },
    { WS_MINIMIZE, "WS_MINIMIZE", FALSE },
    { WS_MINIMIZEBOX, "WS_MINIMIZEBOX", FALSE },
    { WS_OVERLAPPED, "WS_OVERLAPPED", FALSE },
    { WS_OVERLAPPEDWINDOW, "WS_OVERLAPPEDWINDOW", TRUE },
    { WS_POPUP, "WS_POPUP", FALSE },
    { WS_POPUPWINDOW, "WS_POPUPWINDOW", TRUE },
    { WS_SIZEBOX, "WS_SIZEBOX", FALSE },
    { WS_SYSMENU, "WS_SYSMENU", FALSE },
    { WS_TABSTOP, "WS_TABSTOP", FALSE },
    { WS_THICKFRAME, "WS_THICKFRAME", FALSE },
    { WS_VISIBLE, "WS_VISIBLE", FALSE } };

static const WINDOW_STYLE EXTENDED_WINDOW_STYLES[] = {
    { WS_EX_ACCEPTFILES, "WS_EX_ACCEPTFILES", FALSE },
    { WS_EX_APPWINDOW, "WS_EX_APPWINDOW", FALSE },
    { WS_EX_CLIENTEDGE, "WS_EX_CLIENTEDGE", FALSE },
    { WS_EX_COMPOSITED, "WS_EX_COMPOSITED", FALSE },
    { WS_EX_CONTEXTHELP, "WS_EX_CONTEXTHELP", FALSE },
    { WS_EX_CONTROLPARENT, "WS_EX_CONTROLPARENT", FALSE },
    { WS_EX_DLGMODALFRAME, "WS_EX_DLGMODALFRAME", FALSE },
    { WS_EX_LAYERED, "WS_EX_LAYERED", FALSE },
    { WS_EX_LAYOUTRTL, "WS_EX_LAYOUTRTL", FALSE },
    { WS_EX_LEFT, "WS_EX_LEFT", FALSE },
    { WS_EX_LEFTSCROLLBAR, "WS_EX_LEFTSCROLLBAR", FALSE },
    { WS_EX_LTRREADING, "WS_EX_LTRREADING", FALSE },
    { WS_EX_MDICHILD, "WS_EX_MDICHILD", FALSE },
    { WS_EX_NOACTIVATE, "WS_EX_NOACTIVATE", FALSE },
    { WS_EX_NOINHERITLAYOUT, "WS_EX_NOINHERITLAYOUT", FALSE },
    { WS_EX_NOPARENTNOTIFY, "WS_EX_NOPARENTNOTIFY", FALSE },
    { WS_EX_OVERLAPPEDWINDOW, "WS_EX_OVERLAPPEDWINDOW", TRUE },
    { WS_EX_PALETTEWINDOW, "WS_EX_PALETTEWINDOW", TRUE },
    { WS_EX_RIGHT, "WS_EX_RIGHT", FALSE },
    { WS_EX_RIGHTSCROLLBAR, "WS_EX_RIGHTSCROLLBAR", FALSE },
    { WS_EX_RTLREADING, "WS_EX_RTLREADING", FALSE },
    { WS_EX_STATICEDGE, "WS_EX_STATICEDGE", FALSE },
    { WS_EX_TOOLWINDOW, "WS_EX_TOOLWINDOW", FALSE },
    { WS_EX_TOPMOST, "WS_EX_TOPMOST", FALSE },
    { WS_EX_TRANSPARENT, "WS_EX_TRANSPARENT", FALSE },
    { WS_EX_WINDOWEDGE, "WS_EX_WINDOWEDGE", FALSE }
};

static void PrintWindowStyles(UINT32 style)
{
    qDebug(TAG, "\tWindow Styles:\t{");

    for (size_t i = 0; i < ARRAYSIZE(WINDOW_STYLES); i++)
    {
        if (style & WINDOW_STYLES[i].style)
        {
            if (WINDOW_STYLES[i].multi)
            {
                if ((style & WINDOW_STYLES[i].style) != WINDOW_STYLES[i].style)
                    continue;
            }

            qDebug(TAG, "\t\t%s", WINDOW_STYLES[i].name);
        }
    }
}

static void PrintExtendedWindowStyles(UINT32 style)
{
    qDebug(TAG, "\tExtended Window Styles:\t{");

    for (size_t i = 0; i < ARRAYSIZE(EXTENDED_WINDOW_STYLES); i++)
    {
        if (style & EXTENDED_WINDOW_STYLES[i].style)
        {
            if (EXTENDED_WINDOW_STYLES[i].multi)
            {
                if ((style & EXTENDED_WINDOW_STYLES[i].style) != EXTENDED_WINDOW_STYLES[i].style)
                    continue;
            }

            qDebug(TAG, "\t\t%s", EXTENDED_WINDOW_STYLES[i].name);
        }
    }
}

static void PrintRailWindowState(const WINDOW_ORDER_INFO* orderInfo,
                                 const WINDOW_STATE_ORDER* windowState)
{
    if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_NEW)
        qDebug(TAG, "WindowCreate: WindowId: 0x%08X", orderInfo->windowId);
    else
        qDebug(TAG, "WindowUpdate: WindowId: 0x%08X", orderInfo->windowId);

    qDebug(TAG, "{");

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_OWNER)
    {
        qDebug(TAG, "\tOwnerWindowId: 0x%08X", windowState->ownerWindowId);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_STYLE)
    {
        qDebug(TAG, "\tStyle: 0x%08X ExtendedStyle: 0x%08X", windowState->style,
               windowState->extendedStyle);
        PrintWindowStyles(windowState->style);
        PrintExtendedWindowStyles(windowState->extendedStyle);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_SHOW)
    {
        qDebug(TAG, "\tShowState: %u", windowState->showState);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TITLE)
    {

#if FreeRDP_VERSION_MAJOR >= 3
        const WCHAR* str = (const WCHAR*)windowState->titleInfo.string;
        char* title =
            ConvertWCharNToUtf8Alloc(str, windowState->titleInfo.length / sizeof(WCHAR), NULL);
        qDebug(TAG, "\tTitleInfo: %s (length = %hu)", title, windowState->titleInfo.length);
        free(title);
#else \
        //TODO:
#endif
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_OFFSET)
    {
        qDebug(TAG, "\tClientOffsetX: %d ClientOffsetY: %d", windowState->clientOffsetX,
               windowState->clientOffsetY);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE)
    {
        qDebug(TAG, "\tClientAreaWidth: %u ClientAreaHeight: %u", windowState->clientAreaWidth,
               windowState->clientAreaHeight);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RP_CONTENT)
    {
        qDebug(TAG, "\tRPContent: %u", windowState->RPContent);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ROOT_PARENT)
    {
        qDebug(TAG, "\tRootParentHandle: 0x%08X", windowState->rootParentHandle);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET)
    {
        qDebug(TAG, "\tWindowOffsetX: %d WindowOffsetY: %d", windowState->windowOffsetX,
               windowState->windowOffsetY);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_CLIENT_DELTA)
    {
        qDebug(TAG, "\tWindowClientDeltaX: %d WindowClientDeltaY: %d",
               windowState->windowClientDeltaX, windowState->windowClientDeltaY);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE)
    {
        qDebug(TAG, "\tWindowWidth: %u WindowHeight: %u", windowState->windowWidth,
               windowState->windowHeight);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS)
    {
        RECTANGLE_16* rect;
        qDebug(TAG, "\tnumWindowRects: %u", windowState->numWindowRects);

        for (UINT32 index = 0; index < windowState->numWindowRects; index++)
        {
            rect = &windowState->windowRects[index];
            qDebug(TAG, "\twindowRect[%u]: left: %hu top: %hu right: %hu bottom: %hu", index,
                   rect->left, rect->top, rect->right, rect->bottom);
        }
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET)
    {
        qDebug(TAG, "\tvisibileOffsetX: %d visibleOffsetY: %d", windowState->visibleOffsetX,
               windowState->visibleOffsetY);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY)
    {
        RECTANGLE_16* rect;
        qDebug(TAG, "\tnumVisibilityRects: %u", windowState->numVisibilityRects);

        for (UINT32 index = 0; index < windowState->numVisibilityRects; index++)
        {
            rect = &windowState->visibilityRects[index];
            qDebug(TAG, "\tvisibilityRect[%u]: left: %hu top: %hu right: %hu bottom: %hu", index,
                   rect->left, rect->top, rect->right, rect->bottom);
        }
    }

    qDebug(TAG, "}");
}

static void PrintRailIconInfo(const WINDOW_ORDER_INFO* orderInfo, const ICON_INFO* iconInfo)
{
    qDebug(TAG, "ICON_INFO");
    qDebug(TAG, "{");
    qDebug(TAG, "\tbigIcon: %s",
           (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ICON_BIG) ? "true" : "false");
    qDebug(TAG, "\tcacheEntry; 0x%08X", iconInfo->cacheEntry);
    qDebug(TAG, "\tcacheId: 0x%08X", iconInfo->cacheId);
    qDebug(TAG, "\tbpp: %u", iconInfo->bpp);
    qDebug(TAG, "\twidth: %u", iconInfo->width);
    qDebug(TAG, "\theight: %u", iconInfo->height);
    qDebug(TAG, "\tcbColorTable: %u", iconInfo->cbColorTable);
    qDebug(TAG, "\tcbBitsMask: %u", iconInfo->cbBitsMask);
    qDebug(TAG, "\tcbBitsColor: %u", iconInfo->cbBitsColor);
    qDebug(TAG, "\tcolorTable: %p", (void*)iconInfo->colorTable);
    qDebug(TAG, "\tbitsMask: %p", (void*)iconInfo->bitsMask);
    qDebug(TAG, "\tbitsColor: %p", (void*)iconInfo->bitsColor);
    qDebug(TAG, "}");
}

CRail* CRail::GetRail(rdpContext *context)
{
    CConnectFreeRDP::ClientContext* pContext =
        (CConnectFreeRDP::ClientContext*) context;
    if(!pContext)
        return nullptr;
    CConnectFreeRDP* pConnect = pContext->pThis;
    if(!pConnect)
        return nullptr;
    CRail* pRail = pConnect->m_pRail;
    return pRail;
}

BOOL CRail::cbWindowCommon(rdpContext *context,
                           const WINDOW_ORDER_INFO *orderInfo,
                           const WINDOW_STATE_ORDER *windowState)
{
    qDebug(log) << Q_FUNC_INFO;
    CRail* pRail = GetRail(context);
    if(!pRail) {
        qCritical(log) << "GetRail fail";
        return FALSE;
    }
    return pRail->onWindowCommon(context, orderInfo, windowState);
}

BOOL CRail::onWindowCommon(rdpContext *context,
                           const WINDOW_ORDER_INFO *orderInfo,
                           const WINDOW_STATE_ORDER *windowState)
{
    UINT32 fieldFlags = orderInfo->fieldFlags;
    PrintRailWindowState(orderInfo, windowState);

    if (fieldFlags & WINDOW_ORDER_STATE_NEW)
    {

    }

    /* Keep track of any position/size update so that we can force a refresh of the window */
    if ((fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET) ||
        (fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE) ||
        (fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_OFFSET) ||
        (fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE) ||
        (fieldFlags & WINDOW_ORDER_FIELD_WND_CLIENT_DELTA) ||
        (fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET) ||
        (fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY))
    {

    }

    /* Update Parameters */

    if (fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET)
    {
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE)
    {
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_X)
    {
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_Y)
    {
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_OWNER)
    {
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_STYLE)
    {
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_SHOW)
    {
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_TITLE)
    {
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_OFFSET)
    {

    }

    if (fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE)
    {

    }

    if (fieldFlags & WINDOW_ORDER_FIELD_WND_CLIENT_DELTA)
    {

    }

    if (fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS)
    {

    }

    if (fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET)
    {

    }

    if (fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY)
    {
    }

    /* Update Window */

    if (fieldFlags & WINDOW_ORDER_FIELD_STYLE)
    {
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_SHOW)
    {

    }

    if (fieldFlags & WINDOW_ORDER_FIELD_TITLE)
    {

    }


    if (fieldFlags & (WINDOW_ORDER_STATE_NEW | WINDOW_ORDER_FIELD_STYLE))
    {

    }

    /* We should only be using the visibility rects for shaping the window */
    /*if (fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS)
	{
	    xf_SetWindowRects(xfc, appWindow, appWindow->windowRects, appWindow->numWindowRects);
	}*/

    return TRUE;
}

BOOL CRail::cbWindowDelete(rdpContext *context,
                           const WINDOW_ORDER_INFO *orderInfo)
{
    qDebug(log) << Q_FUNC_INFO;
    //TODO:
    return TRUE;
}

BOOL CRail::cbWindowIcon(rdpContext *context,
                         const WINDOW_ORDER_INFO *orderInfo,
                         const WINDOW_ICON_ORDER *windowIcon)
{
    qDebug(log) << Q_FUNC_INFO;
    //TODO:
    return TRUE;
}

BOOL CRail::cbWindowCachedIcon(
    rdpContext *context,
    const WINDOW_ORDER_INFO *orderInfo,
    const WINDOW_CACHED_ICON_ORDER *windowCachedIcon)
{
    qDebug(log) << Q_FUNC_INFO;
    return TRUE;
}

BOOL CRail::cbNotifyIconCreate(
    rdpContext* context,
    const WINDOW_ORDER_INFO* orderInfo,
    const NOTIFY_ICON_STATE_ORDER* notifyIconState)
{
    qDebug(log) << Q_FUNC_INFO;
    return TRUE;
}

BOOL CRail::cbNotifyIconUpdate(
    rdpContext *context,
    const WINDOW_ORDER_INFO *orderInfo,
    const NOTIFY_ICON_STATE_ORDER *notifyIconState)
{
    qDebug(log) << Q_FUNC_INFO;
    return TRUE;
}

BOOL CRail::cbNotifyIconDelete(
    rdpContext *context,
    const WINDOW_ORDER_INFO *orderInfo)
{
    qDebug(log) << Q_FUNC_INFO;
    return TRUE;
}

BOOL CRail::cbMonitoredDesktop(
    rdpContext *context,
    const WINDOW_ORDER_INFO *orderInfo,
    const MONITORED_DESKTOP_ORDER *monitoredDesktop)
{
    qDebug(log) << Q_FUNC_INFO;
    return TRUE;
}

BOOL CRail::cbNonMonitoredDesktop(rdpContext *context,
                                  const WINDOW_ORDER_INFO *orderInfo)
{
    qDebug(log) << Q_FUNC_INFO;
    return TRUE;
}
