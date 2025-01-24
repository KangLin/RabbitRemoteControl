// Author: Kang Lin <kl222@126.com>

/*! See:
 * - [[MS-RDPERP]: Remote Desktop Protocol: Remote Programs Virtual Channel Extension](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/83275957-2d0e-4c52-88d1-1b4c998c6bec)
 *   - [2.2.1.3.1.2.1 New or Existing Window](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/2d36758d-3fb5-4823-8c4b-9b81a9c8ff3e)
 *   - [2.2.1.3.1.2.2 Window Icon](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/6e2ee1eb-33d1-4ad9-a3c2-cc6fb2e4ad4e)
 *   - [2.2.1.3.2.2.1 New or Existing Notification Icons](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/54be5b64-200a-4b7d-a57a-20e523bcf3ab)
 *   - [3.1.1.1 Server State Machine](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/247135fb-6f7e-43b8-a988-f88ccdac12e7)
 *
 * - Overview of RemoteApp: https://learn.microsoft.com/en-us/previous-versions/windows/it-pro/windows-server-2008-R2-and-2008/cc755055(v=ws.11)?redirectedfrom=MSDN
 * - RAIL exec error: execResult=RAIL_EXEC_E_NOT_IN_ALLOWLIST NtError=0x15.
 *   See: https://github.com/FreeRDP/FreeRDP/issues/998
 *    this is a configuration problem on your server see #595 on how to add a command to the allow list or
 *    set "fAllowUnlistedRemotePrograms" to 1 in "HKLM\SOFTWARE\Policies\Microsoft\Windows NT\Terminal Services" to allow all commands.
 */

#include "winpr/string.h"
#include "RAIL.h"
#include "RAILEvent.h"

#include "ConnectFreeRDP.h"

#include <QApplication>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "FreeRDP.Rail")
static Q_LOGGING_CATEGORY(logInfoWin, "FreeRDP.Rail.Info.Window")
static Q_LOGGING_CATEGORY(logInfoIcon, "FreeRDP.Rail.Info.Icon")

int g_CRailInfo = qRegisterMetaType<CRAILInfo>("CRailInfo");
int g_QSharedPointer_CRailInfo = qRegisterMetaType<QSharedPointer<CRAILInfo> >("QSharedPointer<CRailInfo>");

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

//! - [2.2.1.3.1.2.2 Window Icon](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/6e2ee1eb-33d1-4ad9-a3c2-cc6fb2e4ad4e)
//! - [2.2.1.3.1.2.3 Cached Icon](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/4e42886d-b7e6-4f27-aad6-da2e297edf17)
//! - [2.2.1.3.2.2.1 New or Existing Notification Icons](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/54be5b64-200a-4b7d-a57a-20e523bcf3ab)
static void PrintRailIconInfo(const WINDOW_ORDER_INFO* orderInfo, const ICON_INFO* iconInfo)
{
    qDebug(logInfoIcon, "ICON_INFO");
    qDebug(logInfoIcon, "{");
    qDebug(logInfoIcon, "\tbigIcon: %s",
           (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ICON_BIG) ? "true" : "false");
    qDebug(logInfoIcon, "\tcacheEntry; 0x%08X", iconInfo->cacheEntry);
    qDebug(logInfoIcon, "\tcacheId: 0x%08X", iconInfo->cacheId);
    qDebug(logInfoIcon, "\tbpp: %u", iconInfo->bpp);
    qDebug(logInfoIcon, "\twidth: %u", iconInfo->width);
    qDebug(logInfoIcon, "\theight: %u", iconInfo->height);
    qDebug(logInfoIcon, "\tcbColorTable: %u", iconInfo->cbColorTable);
    qDebug(logInfoIcon, "\tcbBitsMask: %u", iconInfo->cbBitsMask);
    qDebug(logInfoIcon, "\tcbBitsColor: %u", iconInfo->cbBitsColor);
    qDebug(logInfoIcon, "\tcolorTable: %p", (void*)iconInfo->colorTable);
    qDebug(logInfoIcon, "\tbitsMask: %p", (void*)iconInfo->bitsMask);
    qDebug(logInfoIcon, "\tbitsColor: %p", (void*)iconInfo->bitsColor);
    qDebug(logInfoIcon, "}");
}

CRAILInfo::CRAILInfo()
    : m_WindowId(0)
    , m_FieldFlags(0)
    , m_NotifyIconId(0)
    , m_OwnerWindowId(0)
    , m_Style(0)
    , m_ExtendedStyle(0)
    , m_ShowState(0)
    , m_WindowOffsetX(0)
    , m_WindowOffsetY(0)
    , m_WindowWidth(0)
    , m_WindowHeight(0)
    , m_VisibleOffsetX(0)
    , m_VisibleOffsetY(0)
    , m_TaskbarButton(0)
    , m_AppBarState(0)
    , m_AppBarEdge(0)
{
}

//! [2.2.1.3.1.2.1 New or Existing Window](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/2d36758d-3fb5-4823-8c4b-9b81a9c8ff3e)
CRAILInfo::CRAILInfo(const WINDOW_ORDER_INFO *orderInfo,
                     const WINDOW_STATE_ORDER *windowState)
    : CRAILInfo()
{
    if(!orderInfo || !windowState) return;

    m_WindowId = orderInfo->windowId;
    m_FieldFlags = orderInfo->fieldFlags;
    m_NotifyIconId = orderInfo->notifyIconId;
    if(WINDOW_ORDER_FIELD_OWNER & m_FieldFlags)
        m_OwnerWindowId = windowState->ownerWindowId;
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_STYLE) {
        m_Style = windowState->style;
        m_ExtendedStyle = windowState->extendedStyle;
    }
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_SHOW)
        m_ShowState = windowState->showState;
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TITLE) {
        m_szTitle = GetString(windowState->titleInfo);
    }
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET) {
        m_WindowOffsetX = windowState->windowOffsetX;
        m_WindowOffsetY = windowState->windowOffsetY;
    }
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE) {
        m_WindowWidth = windowState->windowWidth;
        m_WindowHeight = windowState->windowHeight;
    }
    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS) {
        for(int i = 0; i < windowState->numWindowRects; i++)
        {
            RECTANGLE_16* p = &windowState->windowRects[i];
            QRect r(p->left, p->top, p->right - p->left, p->bottom - p->top);
            m_Updates.push_back(r);
        }
    }

    if(WINDOW_ORDER_FIELD_VIS_OFFSET & orderInfo->fieldFlags) {
        m_VisibleOffsetX = windowState->visibleOffsetX;
        m_VisibleOffsetY = windowState->visibleOffsetY;
    }

    if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY) {
        for(int i = 0; i < windowState->numVisibilityRects; i++) {
            RECTANGLE_16* p = &windowState->visibilityRects[i];
            QRect r(p->left, p->top, p->right - p->left, p->bottom - p->top);
            m_Visibilities.push_back(r);
        }
    }

    if(WINDOW_ORDER_FIELD_TASKBAR_BUTTON & orderInfo->fieldFlags)
        m_TaskbarButton = windowState->TaskbarButton;

    if(WINDOW_ORDER_FIELD_APPBAR_STATE & orderInfo->fieldFlags) {
        m_AppBarState = windowState->AppBarState;
    }

    if(WINDOW_ORDER_FIELD_APPBAR_EDGE & orderInfo->fieldFlags) {
        m_AppBarEdge = windowState->AppBarEdge;
    }
}

CRAILInfo::CRAILInfo(const WINDOW_ORDER_INFO *orderInfo)
    : CRAILInfo()
{
    if(!orderInfo) return;

    m_WindowId = orderInfo->windowId;
    m_FieldFlags = orderInfo->fieldFlags;
    m_NotifyIconId = orderInfo->notifyIconId;
}

CRAILInfo::CRAILInfo(const CRAILInfo &info)
{
    m_WindowId = info.m_WindowId;
    m_FieldFlags = info.m_FieldFlags;
    m_NotifyIconId = info.m_NotifyIconId;
    m_OwnerWindowId = info.m_OwnerWindowId;
    m_Style = info.m_Style;
    m_ExtendedStyle = info.m_ExtendedStyle;
    m_ShowState = info.m_ShowState;
    m_szTitle = info.m_szTitle;
    m_WindowOffsetX = info.m_WindowOffsetX;
    m_WindowOffsetY = info.m_WindowOffsetY;
    m_WindowWidth = info.m_WindowWidth;
    m_WindowHeight = info.m_WindowHeight;
    m_Updates = info.m_Updates;
    m_VisibleOffsetX = info.m_VisibleOffsetX;
    m_VisibleOffsetY = info.m_VisibleOffsetY;
    m_Visibilities = info.m_Visibilities;
    m_TaskbarButton = info.m_TaskbarButton;
    m_AppBarState = info.m_AppBarState;
    m_AppBarEdge = info.m_AppBarEdge;
}

CRAIL::CRAIL(CConnectFreeRDP* pConnect, QObject *parent)
    : QObject{parent}
    , m_pConnect(pConnect)
    , m_pContext(nullptr)
    , m_nMaxIconCache(0)
    , m_nMaxIconCacheEntry(0)
{
    qDebug(log) << "CRail::CRail()";
}

CRAIL::~CRAIL()
{
    qDebug(log) << "CRail::~CRail()";
}

int CRAIL::Init(RailClientContext *pContext)
{
    qDebug(log) << "CRail::Init()";
    if(!pContext) {
        qCritical(log) << Q_FUNC_INFO << "The parameter is null";
        return 0;
    }
    m_pContext = pContext;
    pContext->custom = this;
    // [3.1.1.1 Server State Machine](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/247135fb-6f7e-43b8-a988-f88ccdac12e7)
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
    auto settings = p->settings;
    m_nMaxIconCache = freerdp_settings_get_uint32(
        settings, FreeRDP_RemoteAppNumIconCaches);
    m_nMaxIconCacheEntry =
        freerdp_settings_get_uint32(settings,
                                    FreeRDP_RemoteAppNumIconCacheEntries);
    for(int i = 0; i < m_nMaxIconCacheEntry; i++) {
        QVector<QImage> image;
        for(int j = 0; j < m_nMaxIconCache; j++) {
            image.push_back(QImage());
        }
        m_IconCacheEntry.push_back(image);
    }

    return 0;
}

int CRAIL::Clean(RailClientContext *pContext)
{
    qDebug(log) << "CRail::Clean()";
    disconnect(m_pConnect);
    m_IconCacheEntry.clear();
    return 0;
}

static const char* error_code_names[] = { "RAIL_EXEC_S_OK",
                                         "RAIL_EXEC_E_HOOK_NOT_LOADED",
                                         "RAIL_EXEC_E_DECODE_FAILED",
                                         "RAIL_EXEC_E_NOT_IN_ALLOWLIST",
                                         "RAIL_EXEC_E_FILE_NOT_FOUND",
                                         "RAIL_EXEC_E_FAIL",
                                         "RAIL_EXEC_E_SESSION_LOCKED" };


UINT CRAIL::cbServerExecuteResult(
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
        CRAIL* pRail = (CRAIL*)context->custom;
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

UINT CRAIL::cbServerSystemParam(
    RailClientContext *context,
    const RAIL_SYSPARAM_ORDER *sysparam)
{
    qDebug(log) << Q_FUNC_INFO;
    return CHANNEL_RC_OK;
}

//! [[MS-RDPERP] 2.2.2.3.1 Client Execute PDU (TS_RAIL_ORDER_EXEC)](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/98a6e3c3-c2a9-42cc-ad91-0d9a6c211138)
UINT CRAIL::ServerStartCmd(RailClientContext* context)
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

UINT CRAIL::cbServerHandshake(
    RailClientContext *context,
    const RAIL_HANDSHAKE_ORDER *handshake)
{
    qDebug(log) << Q_FUNC_INFO;
#if FreeRDP_VERSION_MAJOR >= 3
    return client_rail_server_start_cmd(context);
#else
    CRAIL* pRail = (CRAIL*)context->custom;
    return pRail->ServerStartCmd(context);
#endif
}

UINT CRAIL::cbServerHandshakeEx(
    RailClientContext *context,
    const RAIL_HANDSHAKE_EX_ORDER *handshakeEx)
{
    qDebug(log) << Q_FUNC_INFO;
#if FreeRDP_VERSION_MAJOR >= 3
    return client_rail_server_start_cmd(context);
#else
    CRAIL* pRail = (CRAIL*)context->custom;
    return pRail->ServerStartCmd(context);
#endif
}

UINT CRAIL::cbServerLocalMoveSize(
    RailClientContext *context,
    const RAIL_LOCALMOVESIZE_ORDER *localMoveSize)
{
    qDebug(log) << Q_FUNC_INFO << WINDOW_ID(localMoveSize->windowId)
                << localMoveSize->isMoveSizeStart
                << localMoveSize->moveSizeType
                << localMoveSize->posX << localMoveSize->posY;
    return CHANNEL_RC_OK;
}

UINT CRAIL::cbServerMinMaxInfo(
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

UINT CRAIL::cbServerLanguageBarInfo(
    RailClientContext *context,
    const RAIL_LANGBAR_INFO_ORDER *langBarInfo)
{
    qDebug(log) << Q_FUNC_INFO << HEX(langBarInfo->languageBarStatus);
    return CHANNEL_RC_OK;
}

UINT CRAIL::cbServerGetAppIdResponse(
    RailClientContext *context,
    const RAIL_GET_APPID_RESP_ORDER *getAppIdResp)
{
    qDebug(log) << Q_FUNC_INFO
                << WINDOW_ID(getAppIdResp->windowId);
    return CHANNEL_RC_OK;
}

void CRAIL::RegisterUpdateCallbacks(rdpUpdate* update)
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

CRAIL* CRAIL::GetRail(rdpContext *context)
{
    CConnectFreeRDP::ClientContext* pContext =
        (CConnectFreeRDP::ClientContext*) context;
    if(!pContext)
        return nullptr;
    CConnectFreeRDP* pConnect = pContext->pThis;
    if(!pConnect)
        return nullptr;
    CRAIL* pRail = pConnect->m_pRail;
    return pRail;
}

BOOL CRAIL::cbWindowCreate(rdpContext *context,
                           const WINDOW_ORDER_INFO *orderInfo,
                           const WINDOW_STATE_ORDER *windowState)
{
    qDebug(log) << Q_FUNC_INFO;
    PrintRailWindowState(orderInfo, windowState);
    CRAIL* pRail = GetRail(context);
    if(!pRail) {
        qCritical(log) << "GetRail fail";
        return FALSE;
    }
    QSharedPointer<CRAILInfo> info(new CRAILInfo(orderInfo, windowState));
    emit pRail->sigWindow(info);
    return TRUE;
}

BOOL CRAIL::cbWindowUpdate(rdpContext *context,
                           const WINDOW_ORDER_INFO *orderInfo,
                           const WINDOW_STATE_ORDER *windowState)
{
    qDebug(log) << Q_FUNC_INFO;
    PrintRailWindowState(orderInfo, windowState);
    CRAIL* pRail = GetRail(context);
    if(!pRail) {
        qCritical(log) << "GetRail fail";
        return FALSE;
    }
    QSharedPointer<CRAILInfo> info(new CRAILInfo(orderInfo, windowState));
    emit pRail->sigWindow(info);
    return TRUE;
}

BOOL CRAIL::cbWindowDelete(rdpContext *context,
                           const WINDOW_ORDER_INFO *orderInfo)
{
    qDebug(log) << Q_FUNC_INFO;
    CRAIL* pRail = GetRail(context);
    if(!pRail) {
        qCritical(log) << "GetRail fail";
        return FALSE;
    }
    QSharedPointer<CRAILInfo> info(new CRAILInfo(orderInfo));
    emit pRail->sigWindowDelete(info);
    return TRUE;
}

QImage CRAIL::ConvertIcon(const ICON_INFO *iconInfo)
{
    QImage img(iconInfo->width, iconInfo->height, QImage::Format_ARGB32);
    if (!freerdp_image_copy_from_icon_data(
            img.bits(), PIXEL_FORMAT_BGRA32, 0, 0, 0,
            iconInfo->width,
            iconInfo->height, iconInfo->bitsColor,
            iconInfo->cbBitsColor, iconInfo->bitsMask,
            iconInfo->cbBitsMask, iconInfo->colorTable,
            iconInfo->cbColorTable, iconInfo->bpp))
        qCritical(log) << "Copy icon fail";
    return img;
}

BOOL CRAIL::cbWindowIcon(rdpContext *context,
                         const WINDOW_ORDER_INFO *orderInfo,
                         const WINDOW_ICON_ORDER *windowIcon)
{
    qDebug(log) << Q_FUNC_INFO;
    PrintRailIconInfo(orderInfo, windowIcon->iconInfo);
    CRAIL* pRail = GetRail(context);
    if(!pRail) return TRUE;
    QSharedPointer<CRAILInfo> info(new CRAILInfo(orderInfo));
    QImage img = pRail->ConvertIcon(windowIcon->iconInfo);
    if(!img.isNull()) {
        if(windowIcon->iconInfo->cacheId < pRail->m_nMaxIconCache
            && windowIcon->iconInfo->cacheEntry < pRail->m_nMaxIconCacheEntry)
            pRail->m_IconCacheEntry[windowIcon->iconInfo->cacheEntry][windowIcon->iconInfo->cacheId] = img;
        emit pRail->sigWindowIcon(info, img);
    }
    return TRUE;
}

BOOL CRAIL::cbWindowCachedIcon(
    rdpContext *context,
    const WINDOW_ORDER_INFO *orderInfo,
    const WINDOW_CACHED_ICON_ORDER *windowCachedIcon)
{
    qDebug(log) << Q_FUNC_INFO;
    CRAIL* pRail = GetRail(context);
    if(!pRail) return TRUE;
    QSharedPointer<CRAILInfo> info(new CRAILInfo(orderInfo));
    CACHED_ICON_INFO icon = windowCachedIcon->cachedIcon;
    if(icon.cacheId < pRail->m_nMaxIconCache
        && icon.cacheEntry < pRail->m_nMaxIconCacheEntry) {
        QImage img = pRail->m_IconCacheEntry[icon.cacheEntry][icon.cacheId];
        if(!img.isNull())
            emit pRail->sigWindowIcon(info, img);
    } else {
        qCritical(log) << "The icon is in cache:"
                       << icon.cacheEntry << icon.cacheId
                       << pRail->m_IconCacheEntry << pRail->m_nMaxIconCache;
    }

    return TRUE;
}

BOOL CRAIL::cbNotifyIconCreate(
    rdpContext* context,
    const WINDOW_ORDER_INFO* orderInfo,
    const NOTIFY_ICON_STATE_ORDER* notifyIconState)
{
    qDebug(log) << Q_FUNC_INFO;
    const ICON_INFO* iconInfo = &(notifyIconState->icon);
    PrintRailIconInfo(orderInfo, iconInfo);
    return TRUE;
}

BOOL CRAIL::cbNotifyIconUpdate(
    rdpContext *context,
    const WINDOW_ORDER_INFO *orderInfo,
    const NOTIFY_ICON_STATE_ORDER *notifyIconState)
{
    qDebug(log) << Q_FUNC_INFO;
    const ICON_INFO* iconInfo = &(notifyIconState->icon);
    PrintRailIconInfo(orderInfo, iconInfo);
    return TRUE;
}

BOOL CRAIL::cbNotifyIconDelete(
    rdpContext *context,
    const WINDOW_ORDER_INFO *orderInfo)
{
    qDebug(log) << Q_FUNC_INFO;
    return TRUE;
}

BOOL CRAIL::cbMonitoredDesktop(
    rdpContext *context,
    const WINDOW_ORDER_INFO *orderInfo,
    const MONITORED_DESKTOP_ORDER *monitoredDesktop)
{
    qDebug(log) << Q_FUNC_INFO;
    return TRUE;
}

BOOL CRAIL::cbNonMonitoredDesktop(rdpContext *context,
                                  const WINDOW_ORDER_INFO *orderInfo)
{
    qDebug(log) << Q_FUNC_INFO;
    return TRUE;
}

bool CRAIL::event(QEvent *event)
{
    qDebug(log) << "CRAIL::event" << event;
    CRAILEvent* e = static_cast<CRAILEvent*>(event);
    switch (event->type()) {
    case CRAILEvent::Activate:
        onClientActivate(e->m_WindowId, e->m_bActivate);
        break;
    case CRAILEvent::SystemCommand:
        onClientSystemCommand(e->m_WindowId, e->m_SystemCommand);
    default:
        return QObject::event(event);
    }

    event->accept();
    return true;
}

/*!
 * - [MS-RDPERP 2.2.2.6.1 Client Activate PDU (TS_RAIL_ORDER_ACTIVATE)](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/8765a613-1659-4423-beb3-a6cd3b4808d8)
 */
int CRAIL::onClientActivate(UINT32 windowId, bool bEnable)
{
    if (windowId > UINT32_MAX)
        return -1;
    RAIL_ACTIVATE_ORDER activate = { 0 };
    WINPR_ASSERT(windowId <= UINT32_MAX);
    activate.windowId = windowId;
    activate.enabled = bEnable;
    m_pContext->ClientActivate(m_pContext, &activate);
    return 0;
}

void CRAIL::slotClientActivate(UINT32 windowId, bool bEnable)
{
    CRAILEvent *e = new CRAILEvent(CRAILEvent::Activate);
    e->m_WindowId = windowId;
    e->m_bActivate = bEnable;
    QApplication::postEvent(this, e);
    m_pConnect->WakeUp();
}

/*!
 * - [MS-RDPERP 2.2.2.6.3 Client System Command PDU (TS_RAIL_ORDER_SYSCOMMAND)](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/cdda9345-2f82-4e38-8814-1a932b934241)
 */
int CRAIL::onClientSystemCommand(UINT32 windowId, UINT16 command)
{
    if (windowId > UINT32_MAX)
        return -1;
    RAIL_SYSCOMMAND_ORDER syscommand = { 0 };
    syscommand.windowId = windowId;
    syscommand.command = command;
    m_pContext->ClientSystemCommand(m_pContext, &syscommand);
    return 0;
}

void CRAIL::slotClientSystemCommand(UINT32 windowId, UINT16 command)
{
    CRAILEvent *e = new CRAILEvent(CRAILEvent::SystemCommand);
    e->m_WindowId = windowId;
    e->m_SystemCommand = command;
    QApplication::postEvent(this, e);
    m_pConnect->WakeUp();
}
