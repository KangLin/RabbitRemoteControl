// Author: Kang Lin <kl222@126.com>

#ifndef RAILMANAGEWINDOWS_H
#define RAILMANAGEWINDOWS_H

#include "RAILWindow.h"
#include "RAIL.h"

/*!
 * \~chinese 管理远程应用程序窗口
 * \note 它运行在主线程中（UI线程）
 *
 * \~english Manage remote application window
 * \note It is run on main thread(UI thread)
 */
class CRAILManageWindows : public QObject
{
    Q_OBJECT
public:
    explicit CRAILManageWindows(QObject *parent = nullptr);

    int Connect(CRAIL* pRail);

    CConnectFreeRDP* m_pConnect;

    void WindowDelete(UINT32 windowId);

public Q_SLOTS:
    void slotWindow(QSharedPointer<CRAILInfo> info);
    void slotWindowDelete(QSharedPointer<CRAILInfo> info);
    void slotWindowIcon(QSharedPointer<CRAILInfo> info, QImage img);
    void slotInvalidateRect(const QRect& rect, const QImage& img);
    int slotSetDesktopSize(int width, int height);
    void slotClean();

private:
    int UpdateWindow(CRAILWindow* pWin, CRAILInfo* info, bool bVisiable);

private:
    QMap<UINT32, CRAILWindow*> m_Windows;
    QRect m_Desktop;
    CRAIL* m_pRail;
};

#endif // RAILMANAGEWINDOWS_H
