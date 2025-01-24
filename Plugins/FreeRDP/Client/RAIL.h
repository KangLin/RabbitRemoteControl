// Author: Kang Lin <kl222@126.com>

#ifndef RAIL_H
#define RAIL_H

#include <QObject>
#include <QRect>
#include <QVector>

#include <freerdp/client/rail.h>
#include <freerdp/gdi/gdi.h>
#include <freerdp/gdi/dc.h>
#include <freerdp/gdi/region.h>

#define HEX(x) ("0x" + QString("%1").arg((x), 8, 16, QLatin1Char('0')).toUpper())
#define WINDOW_ID(x) ("WindowId: 0x" + QString("%1").arg((x), 8, 16, QLatin1Char('0')).toUpper())

class CConnectFreeRDP;
class CRAILManageWindows;

#if defined(_MSC_VER)
    #include <WinUser.h>
#else
    #define SW_HIDE             0
    #define SW_SHOWNORMAL       1
    #define SW_NORMAL           1
    #define SW_SHOWMINIMIZED    2
    #define SW_SHOWMAXIMIZED    3
    #define SW_MAXIMIZE         3
    #define SW_SHOWNOACTIVATE   4
    #define SW_SHOW             5
    #define SW_MINIMIZE         6
    #define SW_SHOWMINNOACTIVE  7
    #define SW_SHOWNA           8
    #define SW_RESTORE          9
    #define SW_SHOWDEFAULT      10
    #define SW_FORCEMINIMIZE    11
    #define SW_MAX              11
#endif

/*!
 * \~chinese
 * \brief 远程应用程序信息
 *
 * \~englis
 * \brief Remote application information
 *
 * \~
 * \see [2.2.1.3.1.2.1 New or Existing Window](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/2d36758d-3fb5-4823-8c4b-9b81a9c8ff3e)
 */
class CRAILInfo {
    Q_GADGET
public:
    CRAILInfo();
    CRAILInfo(const WINDOW_ORDER_INFO *orderInfo,
              const WINDOW_STATE_ORDER *windowState);
    CRAILInfo(const WINDOW_ORDER_INFO *orderInfo);
    CRAILInfo(const CRAILInfo &info);

    UINT32 m_WindowId;
    UINT32 m_FieldFlags;
    UINT32 m_NotifyIconId;

    UINT32 m_OwnerWindowId;
    UINT32 m_Style;
    UINT32 m_ExtendedStyle;
    /*!
     * | Value | Meaning                                         |
     * |:------|:------------------------------------------------|
     * | 0x00  |Do not show the window.                          |
     * | 0x02  |Show the window minimized.                       |
     * | 0x03  |Show the window maximized.                       |
     * | 0x05  |Show the window in its current size and position.|
    */
    UINT32 m_ShowState;

    QString m_szTitle;
    INT32 m_WindowOffsetX;
    INT32 m_WindowOffsetY;
    UINT32 m_WindowWidth;
    UINT32 m_WindowHeight;

    QVector<QRect> m_Updates;
    INT32 m_VisibleOffsetX;
    INT32 m_VisibleOffsetY;
    QVector<QRect> m_Visibilities;

    /*! If this field is set to 0x00,
     * then the client SHOULD add a tab to the taskbar button group for the window,
     * if supported by the operating system,
     * instead of adding a new taskbar button for the window.
     * If this field is set to 0x01, then the client SHOULD remove the tab
     * from the taskbar button group for the window.
     */
    BYTE m_TaskbarButton;

    /*! If this field is set to 0x01, then the window SHOULD be registered as an application desktop toolbar.
     * If this field is set to 0x00, then the application desktop toolbar SHOULD be deregistered.
     */
    UINT8 m_AppBarState;
    /*! The value of this field indicates the edge to which the application desktop toolbar SHOULD be anchored.
     *  This field MUST be set to one of the following possible values.
     * | Value | Meaning                  |
     * |:------|:-------------------------|
     * | 0x00  |Anchor to the left edge.  |
     * | 0x02  |Anchor to the top edge.   |
     * | 0x03  |Anchor to the right edge. |
     * | 0x05  |Anchor to the bottom edge.|
     */
    UINT8 m_AppBarEdge;
};

/*!
 * \~chinese 本地集成的远程应用程序(RAIL)
 * \~english Remote applications integrated locally (RAIL)
 */
class CRAIL : public QObject
{
    Q_OBJECT

public:
    explicit CRAIL(CConnectFreeRDP* pConnect, QObject *parent = nullptr);
    virtual ~CRAIL();

    int Init(RailClientContext* pContext);
    int Clean(RailClientContext* pContext);

    static CRAIL* GetRail(rdpContext *context);

Q_SIGNALS:
    void sigWindow(QSharedPointer<CRAILInfo> info);
    void sigWindowDelete(QSharedPointer<CRAILInfo> info);
    void sigWindowIcon(QSharedPointer<CRAILInfo> info, QImage img);

private:
    static UINT cbServerExecuteResult(
        RailClientContext* context,
        const RAIL_EXEC_RESULT_ORDER* execResult);
    static UINT cbServerSystemParam(
        RailClientContext* context,
        const RAIL_SYSPARAM_ORDER* sysparam
        );
    static UINT cbServerHandshake(
        RailClientContext* context,
        const RAIL_HANDSHAKE_ORDER* handshake
        );
    static UINT cbServerHandshakeEx(
        RailClientContext* context,
        const RAIL_HANDSHAKE_EX_ORDER* handshakeEx
        );
    static UINT cbServerLocalMoveSize(
        RailClientContext* context,
        const RAIL_LOCALMOVESIZE_ORDER* localMoveSize
        );
    static UINT cbServerMinMaxInfo(
        RailClientContext* context,
        const RAIL_MINMAXINFO_ORDER* minMaxInfo
        );
    static UINT cbServerLanguageBarInfo(
        RailClientContext* context,
        const RAIL_LANGBAR_INFO_ORDER* langBarInfo
        );
    static UINT cbServerGetAppIdResponse(
        RailClientContext* context,
        const RAIL_GET_APPID_RESP_ORDER* getAppIdResp
        );

    UINT ServerStartCmd(RailClientContext *context);
    void RegisterUpdateCallbacks(rdpUpdate* update);

    QImage ConvertIcon(const ICON_INFO* iconInfo);

    static BOOL cbWindowCreate(rdpContext* context,
                               const WINDOW_ORDER_INFO* orderInfo,
                               const WINDOW_STATE_ORDER* windowState);
    static BOOL cbWindowUpdate(rdpContext* context,
                               const WINDOW_ORDER_INFO* orderInfo,
                               const WINDOW_STATE_ORDER* windowState);
    static BOOL cbWindowDelete(rdpContext* context,
                               const WINDOW_ORDER_INFO* orderInfo);
    static BOOL cbWindowIcon(rdpContext* context,
                             const WINDOW_ORDER_INFO* orderInfo,
                             const WINDOW_ICON_ORDER* windowIcon);
    static BOOL cbWindowCachedIcon(
        rdpContext* context,
        const WINDOW_ORDER_INFO* orderInfo,
        const WINDOW_CACHED_ICON_ORDER* windowCachedIcon);
    static BOOL cbNotifyIconCreate(
        rdpContext* context,
        const WINDOW_ORDER_INFO* orderInfo,
        const NOTIFY_ICON_STATE_ORDER* notifyIconState);
    static BOOL cbNotifyIconUpdate(
        rdpContext* context,
        const WINDOW_ORDER_INFO* orderInfo,
        const NOTIFY_ICON_STATE_ORDER* notifyIconState);
    static BOOL cbNotifyIconDelete(
        rdpContext* context, const WINDOW_ORDER_INFO* orderInfo);
    static BOOL cbMonitoredDesktop(
        rdpContext* context, const WINDOW_ORDER_INFO* orderInfo,
        const MONITORED_DESKTOP_ORDER* monitoredDesktop);
    static BOOL cbNonMonitoredDesktop(
        rdpContext* context, const WINDOW_ORDER_INFO* orderInfo);

private:
    CConnectFreeRDP* m_pConnect;
    RailClientContext* m_pContext;
    int m_nMaxIconCache;
    int m_nMaxIconCacheEntry;
    QVector<QVector<QImage> > m_IconCacheEntry;

public:
    virtual bool event(QEvent *event) override;
public Q_SLOTS:
    /*!
     * - [MS-RDPERP 2.2.2.6.1 Client Activate PDU (TS_RAIL_ORDER_ACTIVATE)](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/8765a613-1659-4423-beb3-a6cd3b4808d8)
     */
    void slotClientActivate(UINT32 windowId, bool bEnable);
    /*!
     * - [MS-RDPERP 2.2.2.6.3 Client System Command PDU (TS_RAIL_ORDER_SYSCOMMAND)](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/cdda9345-2f82-4e38-8814-1a932b934241)
     *
     *   #define SC_SIZE 0xF000
     *   #define SC_MOVE 0xF010
     *   #define SC_MINIMIZE 0xF020
     *   #define SC_MAXIMIZE 0xF030
     *   #define SC_CLOSE 0xF060
     *   #define SC_KEYMENU 0xF100
     *   #define SC_RESTORE 0xF120
     *   #define SC_DEFAULT 0xF160
     */
    void slotClientSystemCommand(UINT32 windowId, UINT16 command);
private:
    int onClientActivate(UINT32 windowId, bool bEnable);
    int onClientSystemCommand(UINT32 windowId, UINT16 command);
};

#endif // RAIL_H
