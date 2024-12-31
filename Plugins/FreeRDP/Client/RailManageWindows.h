// Author: Kang Lin <kl222@126.com>

#ifndef RAILMANAGEWINDOWS_H
#define RAILMANAGEWINDOWS_H

#include "RailWindow.h"
#include "Rail.h"

/*!
 * \~chinese 管理远程应用程序窗口
 * \~english Manage remote application window
 */
class CRailManageWindows : public QObject
{
    Q_OBJECT
public:
    explicit CRailManageWindows(QObject *parent = nullptr);

    int Clean();

    int Connect(CRail* pRail);

    CConnectFreeRDP* m_pConnect;

public Q_SLOTS:
    void slotWindow(QSharedPointer<CRailInfo> info);
    void slotWindowDelete(QSharedPointer<CRailInfo> info);
    void slotInvalidateRect(QRect rect);

private:
    void WindowDelete(UINT32 windowId);
    int UpdateWindow(CRailWindow* pWin, CRailInfo* info);

private:
    QMap<UINT32, CRailWindow*> m_Windows;
};

#endif // RAILMANAGEWINDOWS_H
