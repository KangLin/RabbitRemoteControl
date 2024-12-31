// Author: Kang Lin <kl222@126.com>

// See:
// - Overview of RemoteApp: https://learn.microsoft.com/en-us/previous-versions/windows/it-pro/windows-server-2008-R2-and-2008/cc755055(v=ws.11)?redirectedfrom=MSDN
// - RAIL exec error: execResult=RAIL_EXEC_E_NOT_IN_ALLOWLIST NtError=0x15.
//   See: https://github.com/FreeRDP/FreeRDP/issues/998
//    this is a configuration problem on your server see #595 on how to add a command to the allow list or
//    set "fAllowUnlistedRemotePrograms" to 1 in "HKLM\SOFTWARE\Policies\Microsoft\Windows NT\Terminal Services" to allow all commands.

#include <QLoggingCategory>
#include "winpr/string.h"
#include "Rail.h"
#include "ConnectFreeRDP.h"

static Q_LOGGING_CATEGORY(log, "FreeRDP.Rail")
static Q_LOGGING_CATEGORY(logWin, "FreeRDP.Rail.ManageWindows.Window")
static Q_LOGGING_CATEGORY(logInfoWin, "FreeRDP.Rail.Info.Window")
static Q_LOGGING_CATEGORY(logIconInfo, "FreeRDP.Rail.Info.Icon")
static Q_LOGGING_CATEGORY(logUpdate, "FreeRDP.Rail.Update.Window")

int g_CRailInfo = qRegisterMetaType<CRailInfo>("CRailInfo");
int g_QSharedPointer_CRailInfo = qRegisterMetaType<QSharedPointer<CRailInfo> >("QSharedPointer<CRailInfo>");

QString GetString(RAIL_UNICODE_STRING s)
{
    QString szText;
    const WCHAR* str = (const WCHAR*)s.string;
    char* pText = NULL;

    if (s.length == 0)
    {
        return szText;
    }
#if FreeRDP_VERSION_MAJOR >= 3
    pText = ConvertWCharNToUtf8Alloc(
        str, s.length / sizeof(WCHAR), NULL);
#else
    if(ConvertFromUnicode(
            CP_UTF8, 0, str,
            s.length / 2, &pText, 0, NULL,
            NULL) < 1) {
        qCritical(log) <<  "failed to convert window title";
        return szText;
    }
#endif
    if(pText) {
        szText = pText;
        free(pText);
    } else
        qCritical(log) <<  "failed to convert string";

    return szText;
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
    qDebug(logInfoWin, "\tWindow Styles: {");

    for (size_t i = 0; i < ARRAYSIZE(WINDOW_STYLES); i++)
    {
        if (style & WINDOW_STYLES[i].style)
        {
            if (WINDOW_STYLES[i].multi)
            {
                if ((style & WINDOW_STYLES[i].style) != WINDOW_STYLES[i].style)
                    continue;
            }

            qDebug(logInfoWin, "\t\t%s", WINDOW_STYLES[i].name);
        }
    }

    qDebug(logInfoWin, "\t}");
}

static void PrintExtendedWindowStyles(UINT32 style)
{
    qDebug(logInfoWin, "\tExtended Window Styles: {");
    for (size_t i = 0; i < ARRAYSIZE(EXTENDED_WINDOW_STYLES); i++)
    {
        if (style & EXTENDED_WINDOW_STYLES[i].style)
        {
            if (EXTENDED_WINDOW_STYLES[i].multi)
            {
                if ((style & EXTENDED_WINDOW_STYLES[i].style) != EXTENDED_WINDOW_STYLES[i].style)
                    continue;
            }

            qDebug(logInfoWin, "\t\t%s", EXTENDED_WINDOW_STYLES[i].name);
        }
    }
    qDebug(logInfoWin, "\t}");
}

static void PrintRailWindowState(const WINDOW_ORDER_INFO* orderInfo,
                                 const WINDOW_STATE_ORDER* windowState)
{
    if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_NEW)
        qDebug(logInfoWin, "WindowCreate: WindowId: 0x%08X", orderInfo->windowId);
    else
        qDebug(logInfoWin, "WindowUpdate: WindowId: 0x%08X", orderInfo->windowId);

    qDebug(logInfoWin, "{");

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_OWNER)
    {
        qDebug(logInfoWin, "\tOwnerWindowId: 0x%08X", windowState->ownerWindowId);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_STYLE)
    {
        qDebug(logInfoWin, "\tStyle: 0x%08X ExtendedStyle: 0x%08X",
               windowState->style, windowState->extendedStyle);
        PrintWindowStyles(windowState->style);
        PrintExtendedWindowStyles(windowState->extendedStyle);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_SHOW)
    {
        qDebug(logInfoWin, "\tShowState: %u", windowState->showState);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TITLE)
    {
        QString szTitle = GetString(windowState->titleInfo);
        qDebug(logInfoWin, "\tTitleInfo: %s (length = %hu)",
               szTitle.toStdString().c_str(),
               windowState->titleInfo.length);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_OFFSET)
    {
        qDebug(logInfoWin, "\tClientOffsetX: %d ClientOffsetY: %d",
               windowState->clientOffsetX, windowState->clientOffsetY);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE)
    {
        qDebug(logInfoWin, "\tClientAreaWidth: %u ClientAreaHeight: %u",
               windowState->clientAreaWidth, windowState->clientAreaHeight);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RP_CONTENT)
    {
        qDebug(logInfoWin, "\tRPContent: %u", windowState->RPContent);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ROOT_PARENT)
    {
        qDebug(logInfoWin, "\tRootParentHandle: 0x%08X",
               windowState->rootParentHandle);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET)
    {
        qDebug(logInfoWin, "\tWindowOffsetX: %d WindowOffsetY: %d",
               windowState->windowOffsetX, windowState->windowOffsetY);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_CLIENT_DELTA)
    {
        qDebug(logInfoWin, "\tWindowClientDeltaX: %d WindowClientDeltaY: %d",
               windowState->windowClientDeltaX, windowState->windowClientDeltaY);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE)
    {
        qDebug(logInfoWin, "\tWindowWidth: %u WindowHeight: %u",
               windowState->windowWidth, windowState->windowHeight);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS)
    {
        RECTANGLE_16* rect;
        qDebug(logInfoWin, "\tnumWindowRects: %u", windowState->numWindowRects);

        for (UINT32 index = 0; index < windowState->numWindowRects; index++)
        {
            rect = &windowState->windowRects[index];
            qDebug(logInfoWin, "\twindowRect[%u]: left: %hu top: %hu right: %hu bottom: %hu", index,
                   rect->left, rect->top, rect->right, rect->bottom);
        }
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET)
    {
        qDebug(logInfoWin, "\tvisibileOffsetX: %d visibleOffsetY: %d",
               windowState->visibleOffsetX, windowState->visibleOffsetY);
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY)
    {
        RECTANGLE_16* rect;
        qDebug(logInfoWin, "\tnumVisibilityRects: %u",
               windowState->numVisibilityRects);

        for (UINT32 index = 0; index < windowState->numVisibilityRects; index++)
        {
            rect = &windowState->visibilityRects[index];
            qDebug(logInfoWin, "\tvisibilityRect[%u]: left: %hu top: %hu right: %hu bottom: %hu", index,
                   rect->left, rect->top, rect->right, rect->bottom);
        }
    }

    qDebug(logInfoWin, "}");
}

static void PrintRailIconInfo(const WINDOW_ORDER_INFO* orderInfo, const ICON_INFO* iconInfo)
{
    qDebug(logIconInfo, "ICON_INFO");
    qDebug(logIconInfo, "{");
    qDebug(logIconInfo, "\tbigIcon: %s",
           (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ICON_BIG) ? "true" : "false");
    qDebug(logIconInfo, "\tcacheEntry; 0x%08X", iconInfo->cacheEntry);
    qDebug(logIconInfo, "\tcacheId: 0x%08X", iconInfo->cacheId);
    qDebug(logIconInfo, "\tbpp: %u", iconInfo->bpp);
    qDebug(logIconInfo, "\twidth: %u", iconInfo->width);
    qDebug(logIconInfo, "\theight: %u", iconInfo->height);
    qDebug(logIconInfo, "\tcbColorTable: %u", iconInfo->cbColorTable);
    qDebug(logIconInfo, "\tcbBitsMask: %u", iconInfo->cbBitsMask);
    qDebug(logIconInfo, "\tcbBitsColor: %u", iconInfo->cbBitsColor);
    qDebug(logIconInfo, "\tcolorTable: %p", (void*)iconInfo->colorTable);
    qDebug(logIconInfo, "\tbitsMask: %p", (void*)iconInfo->bitsMask);
    qDebug(logIconInfo, "\tbitsColor: %p", (void*)iconInfo->bitsColor);
    qDebug(logIconInfo, "}");
}

CRailInfo::CRailInfo()
    : m_Style(0)
    , m_ExtendedStyle(0)
    , m_ShowStatus(0)
{
    memset(&m_OrderInfo, 0, sizeof(WINDOW_ORDER_INFO));
}

CRailInfo::CRailInfo(const WINDOW_ORDER_INFO *orderInfo,
                     const WINDOW_STATE_ORDER *windowState)
    : CRailInfo()
{
    if(orderInfo)
        memcpy(&m_OrderInfo, orderInfo, sizeof(WINDOW_ORDER_INFO));
    if(!windowState)
        return;
    m_OwnerWindowId = windowState->ownerWindowId;
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_STYLE) {
        m_Style = windowState->style;
        m_ExtendedStyle = windowState->extendedStyle;
    }
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TITLE) {
        m_szTitle = GetString(windowState->titleInfo);
    }
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET) {
        m_rectWindow.setX(windowState->windowOffsetX);
        m_rectWindow.setY(windowState->windowOffsetY);
    }
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE) {
        m_rectWindow.setWidth(windowState->windowWidth);
        m_rectWindow.setHeight(windowState->windowHeight);
    }
    if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET)
        || (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE)) {
        QString szMsg;
        if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_NEW)
            szMsg = "new:";
        else
            szMsg = "update:";
        qDebug(logWin) << szMsg << WINDOW_ID(orderInfo->windowId)
                    << "windowOffsetX:" << windowState->windowOffsetX
                    << "windowOffsetY:" << windowState->windowOffsetY
                    << "width:" << windowState->windowWidth
                    << "height:" << windowState->windowHeight
                    << "rect:" << m_rectWindow;
    }
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_SHOW)
        m_ShowStatus = windowState->showState;
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS) {
        for(int i = 0; i < windowState->numWindowRects; i++)
        {
            RECTANGLE_16* p = &windowState->windowRects[i];
            QRect r;
            r.setLeft(p->left);
            r.setTop(p->top);
            r.setWidth(p->right - p->left);
            r.setHeight(p->bottom - p->top);
            m_Update = m_Update.united(r);
        }
        if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_NEW)
            qDebug(logUpdate) << "new:" << WINDOW_ID(orderInfo->windowId) << m_Update;
        else
            qDebug(logUpdate) << "update:" << WINDOW_ID(orderInfo->windowId) << m_Update;
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY) {
        for(int i = 0; i < windowState->numVisibilityRects; i++) {
            RECTANGLE_16* p = &windowState->visibilityRects[i];
            QRect r;
            r.setLeft(p->left);
            r.setTop(p->top);
            r.setWidth(p->right - p->left);
            r.setHeight(p->bottom - p->top);
            m_Visibility = m_Visibility.united(r);
        }
        if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_NEW)
            qDebug(logUpdate) << "new:" << WINDOW_ID(orderInfo->windowId) << "visiblity" << m_Update;
        else
            qDebug(logUpdate) << "update:" << WINDOW_ID(orderInfo->windowId) << "visiblity" << m_Update;
    }
}

CRailInfo::CRailInfo(const CRailInfo &info)
{
    m_OrderInfo = info.m_OrderInfo;
    m_OwnerWindowId = info.m_OwnerWindowId;
    m_Style = info.m_Style;
    m_ExtendedStyle = info.m_ExtendedStyle;
    m_szTitle = info.m_szTitle;
    m_rectWindow = info.m_rectWindow;
    m_ShowStatus = info.m_ShowStatus;
    m_Update = info.m_Update;
    m_Visibility = info.m_Visibility;
}

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
    disconnect(m_pConnect);
    return 0;
}

static const char* error_code_names[] = { "RAIL_EXEC_S_OK",
                                         "RAIL_EXEC_E_HOOK_NOT_LOADED",
                                         "RAIL_EXEC_E_DECODE_FAILED",
                                         "RAIL_EXEC_E_NOT_IN_ALLOWLIST",
                                         "RAIL_EXEC_E_FILE_NOT_FOUND",
                                         "RAIL_EXEC_E_FAIL",
                                         "RAIL_EXEC_E_SESSION_LOCKED" };


UINT CRail::cbServerExecuteResult(
    RailClientContext *context,
    const RAIL_EXEC_RESULT_ORDER *execResult)
{
    qDebug(log) << Q_FUNC_INFO;
    if(RAIL_EXEC_S_OK != execResult->execResult)
    {
        QString szErr = tr("Exec remote application fail") + " ["
                        + error_code_names[execResult->execResult]
                        + "(" + QString::number(execResult->rawResult)
                        + ")]: " + GetString(execResult->exeOrFile);
        qCritical(log) << szErr << "flags:" << execResult->flags;
        CRail* pRail = (CRail*)context->custom;
        rdpContext* pContext = (rdpContext*)pRail->m_pConnect->m_pContext;
#if FreeRDP_VERSION_MAJOR >= 3
        freerdp_abort_connect_context(pContext); 
#else
        freerdp_abort_connect(pContext->instance);
#endif
        emit pRail->m_pConnect->sigShowMessageBox(
            tr("Exec remote application fail"), szErr, QMessageBox::Critical);
    }
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
    char argsAndFile[520] = { 0 };
    RAIL_EXEC_ORDER exec = { 0 };
    RAIL_SYSPARAM_ORDER sysparam = { 0 };
    RAIL_CLIENT_STATUS_ORDER clientStatus = { 0 };

    rdpContext* pContext = (rdpContext*)m_pConnect->m_pContext;
    rdpSettings* settings = pContext->settings;

    clientStatus.flags = TS_RAIL_CLIENTSTATUS_ALLOWLOCALMOVESIZE;

    if(freerdp_settings_get_bool(settings, FreeRDP_AutoReconnectionEnabled))
        clientStatus.flags |= TS_RAIL_CLIENTSTATUS_AUTORECONNECT;

    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_ZORDER_SYNC;
    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_WINDOW_RESIZE_MARGIN_SUPPORTED;
    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_APPBAR_REMOTING_SUPPORTED;
    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_POWER_DISPLAY_REQUEST_SUPPORTED;
    clientStatus.flags |= TS_RAIL_CLIENTSTATUS_BIDIRECTIONAL_CLOAK_SUPPORTED;
    status = context->ClientInformation(context, &clientStatus);

    if (status != CHANNEL_RC_OK)
        return status;

    if(freerdp_settings_get_bool(settings,
                                  FreeRDP_RemoteAppLanguageBarSupported))
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
    sysparam.workArea.right =
        freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    sysparam.workArea.bottom =
        freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);
    sysparam.dragFullWindows = FALSE;
    status = context->ClientSystemParam(context, &sysparam);

    if (status != CHANNEL_RC_OK)
        return status;

    exec.RemoteApplicationProgram = (char*)
        freerdp_settings_get_string(settings, FreeRDP_RemoteApplicationProgram);
    exec.RemoteApplicationWorkingDir = (char*)
        freerdp_settings_get_string(settings, FreeRDP_ShellWorkingDirectory);
    const char* RemoteApplicationFile =
        freerdp_settings_get_string(settings, FreeRDP_RemoteApplicationFile);
    const char* RemoteApplicationCmdLine =
        freerdp_settings_get_string(settings, FreeRDP_RemoteApplicationCmdLine);
    if (RemoteApplicationFile && RemoteApplicationCmdLine)
    {
        (void)_snprintf(argsAndFile, ARRAYSIZE(argsAndFile), "%s %s", RemoteApplicationCmdLine,
                         RemoteApplicationFile);
        exec.RemoteApplicationArguments = argsAndFile;
    }
    else if (RemoteApplicationFile)
        exec.RemoteApplicationArguments = (char*)RemoteApplicationFile;
    else
        exec.RemoteApplicationArguments = (char*)RemoteApplicationCmdLine;
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
    qDebug(log) << Q_FUNC_INFO << localMoveSize->windowId
                << localMoveSize->isMoveSizeStart
                << localMoveSize->moveSizeType
                << localMoveSize->posX << localMoveSize->posY;
    return CHANNEL_RC_OK;
}

UINT CRail::cbServerMinMaxInfo(
    RailClientContext *context,
    const RAIL_MINMAXINFO_ORDER *minMaxInfo)
{
    qDebug(log) << Q_FUNC_INFO << WINDOW_ID(minMaxInfo->windowId) << "Max:"
                << minMaxInfo->maxPosX << minMaxInfo->maxPosY
                << minMaxInfo->maxWidth << minMaxInfo->maxHeight
                << minMaxInfo->maxTrackWidth << minMaxInfo->maxTrackHeight
                << "min:" << minMaxInfo->minTrackWidth << minMaxInfo->minTrackHeight;
    return CHANNEL_RC_OK;
}

UINT CRail::cbServerLanguageBarInfo(
    RailClientContext *context,
    const RAIL_LANGBAR_INFO_ORDER *langBarInfo)
{
    qDebug(log) << Q_FUNC_INFO << HEX(langBarInfo->languageBarStatus);
    return CHANNEL_RC_OK;
}

UINT CRail::cbServerGetAppIdResponse(
    RailClientContext *context,
    const RAIL_GET_APPID_RESP_ORDER *getAppIdResp)
{
    qDebug(log) << Q_FUNC_INFO
                << WINDOW_ID(getAppIdResp->windowId);
    return CHANNEL_RC_OK;
}

void CRail::RegisterUpdateCallbacks(rdpUpdate* update)
{
    rdpWindowUpdate* window = update->window;
    window->WindowCreate = cbWindowCreate;
    window->WindowUpdate = cbWindowUpdate;
    window->WindowDelete = cbWindowDelete;
    window->WindowIcon = cbWindowIcon;
    window->WindowCachedIcon = cbWindowCachedIcon;
    window->NotifyIconCreate = cbNotifyIconCreate;
    window->NotifyIconUpdate = cbNotifyIconUpdate;
    window->NotifyIconDelete = cbNotifyIconDelete;
    window->MonitoredDesktop = cbMonitoredDesktop;
    window->NonMonitoredDesktop = cbNonMonitoredDesktop;
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

BOOL CRail::cbWindowCreate(rdpContext *context,
                           const WINDOW_ORDER_INFO *orderInfo,
                           const WINDOW_STATE_ORDER *windowState)
{
    qDebug(log) << Q_FUNC_INFO;
    PrintRailWindowState(orderInfo, windowState);
    CRail* pRail = GetRail(context);
    if(!pRail) {
        qCritical(log) << "GetRail fail";
        return FALSE;
    }
    QSharedPointer<CRailInfo> info(new CRailInfo(orderInfo, windowState));
    emit pRail->sigWindow(info);
    return TRUE;
}

BOOL CRail::cbWindowUpdate(rdpContext *context,
                           const WINDOW_ORDER_INFO *orderInfo,
                           const WINDOW_STATE_ORDER *windowState)
{
    qDebug(log) << Q_FUNC_INFO;
    PrintRailWindowState(orderInfo, windowState);
    CRail* pRail = GetRail(context);
    if(!pRail) {
        qCritical(log) << "GetRail fail";
        return FALSE;
    }
    QSharedPointer<CRailInfo> info(new CRailInfo(orderInfo, windowState));
    emit pRail->sigWindow(info);
    return TRUE;
}

BOOL CRail::cbWindowDelete(rdpContext *context,
                           const WINDOW_ORDER_INFO *orderInfo)
{
    qDebug(log) << Q_FUNC_INFO;
    CRail* pRail = GetRail(context);
    if(!pRail) {
        qCritical(log) << "GetRail fail";
        return FALSE;
    }
    QSharedPointer<CRailInfo> info(new CRailInfo(orderInfo));
    emit pRail->sigWindowDelete(info);
    return TRUE;
}

BOOL CRail::cbWindowIcon(rdpContext *context,
                         const WINDOW_ORDER_INFO *orderInfo,
                         const WINDOW_ICON_ORDER *windowIcon)
{
    qDebug(log) << Q_FUNC_INFO;
    PrintRailIconInfo(orderInfo, windowIcon->iconInfo);
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
    const ICON_INFO* iconInfo = &(notifyIconState->icon);
    PrintRailIconInfo(orderInfo, iconInfo);
    return TRUE;
}

BOOL CRail::cbNotifyIconUpdate(
    rdpContext *context,
    const WINDOW_ORDER_INFO *orderInfo,
    const NOTIFY_ICON_STATE_ORDER *notifyIconState)
{
    qDebug(log) << Q_FUNC_INFO;
    const ICON_INFO* iconInfo = &(notifyIconState->icon);
    PrintRailIconInfo(orderInfo, iconInfo);
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
