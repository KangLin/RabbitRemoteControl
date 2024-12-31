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

#define HEX(x) QString("0x%1").arg((x), 8, 16, QLatin1Char('0'))
#define WINDOW_ID(x) QString("WindowId: 0x%1").arg((x), 8, 16, QLatin1Char('0'))

class CConnectFreeRDP;
class CRailManageWindows;

/*!
 * \~chinese
 * \brief 远程应用程序信息
 *
 * \~englis
 * \brief Remote application information
 */
class CRailInfo {
    Q_GADGET
public:
    CRailInfo();
    CRailInfo(const WINDOW_ORDER_INFO *orderInfo,
              const WINDOW_STATE_ORDER *windowState = nullptr);
    CRailInfo(const CRailInfo &info);

    WINDOW_ORDER_INFO m_OrderInfo;

    UINT32 m_OwnerWindowId;
    UINT32 m_Style;
    UINT32 m_ExtendedStyle;

    QString m_szTitle;
    QRect m_rectWindow;

    UINT32 m_ShowStatus;

    QRect m_Update;
    QRect m_Visibility;
};

/*!
 * \brief The remote application
 */
class CRail : public QObject
{
    Q_OBJECT

public:
    explicit CRail(CConnectFreeRDP* pConnect, QObject *parent = nullptr);
    virtual ~CRail();

    int Init(RailClientContext* pContext);
    int Clean(RailClientContext* pContext);

    static CRail* GetRail(rdpContext *context);

Q_SIGNALS:
    void sigWindow(QSharedPointer<CRailInfo> info);
    void sigWindowDelete(QSharedPointer<CRailInfo> info);

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
};

#endif // RAIL_H
