// Author: Kang Lin <kl222@126.com>

#include "RailManageWindows.h"
#include "ConnectFreeRDP.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "FreeRDP.Rail.ManageWindows")
static Q_LOGGING_CATEGORY(logWin, "FreeRDP.Rail.ManageWindows.Window")
static Q_LOGGING_CATEGORY(logUpdate, "FreeRDP.Rail.Update.Window")
static Q_LOGGING_CATEGORY(logInvalid, "FreeRDP.Rail.Update.Invalid")

CRailManageWindows::CRailManageWindows(QObject *parent)
    : QObject{parent}
{}

int CRailManageWindows::Clean()
{
    foreach(CRailWindow* pWin, m_Windows) {
        if(pWin) {
            pWin->close();
            pWin->deleteLater();
        }
    }
    m_Windows.clear();
    return 0;
}

int CRailManageWindows::Connect(CRail *pRail)
{
    bool check = false;
    Q_ASSERT(pRail && m_pConnect);
    check = connect(
        pRail, &CRail::sigWindow,
        this,  &CRailManageWindows::slotWindow);
    Q_ASSERT(check);
    check = connect(
        pRail, &CRail::sigWindowDelete,
        this, &CRailManageWindows::slotWindowDelete);
    Q_ASSERT(check);
    check = connect(m_pConnect, &CConnectFreeRDP::sigRailInvalidateRect,
                    this, &CRailManageWindows::slotInvalidateRect);
    Q_ASSERT(check);
    check = connect(m_pConnect, &CConnectFreeRDP::sigSetDesktopSize,
                    this, &CRailManageWindows::slotSetDesktopSize);
    Q_ASSERT(check);
    return 0;
}

void CRailManageWindows::slotWindow(QSharedPointer<CRailInfo> info)
{
    qDebug(log) << Q_FUNC_INFO;
    UINT32 fieldFlags = info->m_OrderInfo.fieldFlags;
    UINT32 windowId = info->m_OrderInfo.windowId;

    CRailWindow* pWin = nullptr;
    auto it = m_Windows.find(windowId);
    if(m_Windows.end() != it)
        pWin = m_Windows.value(windowId);
    if (!pWin && (fieldFlags & WINDOW_ORDER_STATE_NEW))
    {
        qDebug(log) << "Create window:" << WINDOW_ID(info->m_OrderInfo.windowId);
        pWin = new CRailWindow();
        if(!pWin) {
            qCritical(log) << "new CRailWindow() fail";
            return;
        }
        m_Windows.insert(windowId, pWin);
        pWin->m_Info = *info;
        int nRet = 0;
        if(m_pConnect) {
            //qDebug(log) << "m_pConnect->SetRailViewer:" << pWin;
            nRet = m_pConnect->SetRailViewer(pWin);
            if(nRet) {
                qCritical(log) << "Connect SetViewer fail" << nRet;
                return;
            }
            //TODO: 可以不需要，因为远程会删除它。
            bool check = connect(pWin, &CRailWindow::destroyed,
                                 this, [&](QObject * obj){
                                     CRailWindow* pW = qobject_cast<CRailWindow*>(obj);
                                     if(pW)
                                         WindowDelete(pW->m_Info.m_OrderInfo.windowId);
                                 });
            Q_ASSERT(check);
        }

        // qDebug(log) << "Window flags:" << pWin->windowFlags();
        // pWin->resize(info->m_rectWindow.width(), info->m_rectWindow.height());
        // pWin->move(info->m_rectWindow.topLeft());
        // qDebug(logWin) << WINDOW_ID(info->m_OrderInfo.windowId)
        //                << "Origin size:"
        //                << pWin->frameGeometry()
        //                << pWin->geometry() << info->m_rectWindow;
        // pWin->slotSetDesktopSize(info->m_rectWindow.width(),
        //                          info->m_rectWindow.height());
    } else {
        qDebug(log) << "Update window:" << WINDOW_ID(info->m_OrderInfo.windowId);
    }

    if(!pWin) {
        qCritical(log) << "Get window fail";
        return;
    }

    /* Update Parameters */

    if ((fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET)
        || (fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE))
    {
        QRect r = pWin->m_Info.m_rectWindow;
        if (fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE) {
            qDebug(logWin) << WINDOW_ID(info->m_OrderInfo.windowId)
            << "window size:" << info->m_rectWindow
            << "old:" << r
            << "maximumSize:" << pWin->maximumSize()
            << "minimumSize:" << pWin->minimumSize();
            r.setWidth(info->m_rectWindow.width());
            r.setHeight(info->m_rectWindow.height());
            pWin->resize(r.width(), r.height());
            pWin->slotSetDesktopSize(r.width(), r.height());
        }

        if (fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET) {
            qDebug(logWin) << WINDOW_ID(info->m_OrderInfo.windowId)
            << "window offset:" << info->m_rectWindow << "old:" << r;
            r.setTopLeft(info->m_rectWindow.topLeft());

            /* 现在很多 linux 用 wayland 作为桌面显示，这样会出现一个问题，
               由于没有坐标系统，导致无边框窗体无法拖动和定位（即 QWidge::move 失效）。
              （一般是 Qt6 开始强制默认优先用 wayland ，之前 Qt5 是默认有 xcb 则优先用 xcb），
               你需要在 main 函数最前面加一行 qputenv("QT_QPA_PLATFORM", "xcb");
            */
            pWin->move(r.x(), r.y());
        }

        pWin->m_Info.m_rectWindow = r;

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
        pWin->Show(info->m_ShowStatus);
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_TITLE)
    {
        pWin->setWindowTitle(info->m_szTitle);
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
        UpdateWindow(pWin, info.data());
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET)
    {

    }

    if (fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY)
    {
    }

}

void CRailManageWindows::slotWindowDelete(QSharedPointer<CRailInfo> info)
{
    UINT32 windowId = info->m_OrderInfo.windowId;
    WindowDelete(windowId);
}

void CRailManageWindows::WindowDelete(UINT32 windowId)
{
    CRailWindow* pWin = m_Windows.value(windowId);
    if(pWin) {
        pWin->close();
        pWin->deleteLater();
    }
    m_Windows.remove(windowId);
}

int CRailManageWindows::UpdateWindow(CRailWindow *pWin, CRailInfo *info)
{
    QRect rect;
    if (!info->m_Update.isValid()) {
        qDebug(logUpdate) << "UpdateWindow: the update rects is empty";
        return 0;
    }
    rect = info->m_Update;
    QRect rw = pWin->m_Info.m_rectWindow;
    QRect rImg(rect.x() + rw.x(), rect.y() + rw.y(), rect.width(), rect.height());
    rImg = rImg.intersected(QRect(m_Desktop));
    rect = QRect(rImg.x() - rw.x(), rImg.y() - rw.y(), rImg.width(), rImg.height());
    QImage img = m_pConnect->GetImage(rImg);
    qDebug(logUpdate) << "UpdateWindow" << WINDOW_ID(info->m_OrderInfo.windowId)
                      << pWin->m_Info.m_rectWindow
                      << "window update:" << rect << info->m_Update
                      << "update:" << rImg
                      << "Image:" << img.rect();
    pWin->slotUpdateRect(rect, img);
    return 0;
}

void CRailManageWindows::slotInvalidateRect(QRect rect)
{
    foreach(auto pWin, m_Windows) {
        if(!pWin)
            continue;
        if(!pWin->m_Info.m_ShowStatus)
            continue;
        QRect ri;
        const QRect rw = pWin->m_Info.m_rectWindow;
        ri = rw.intersected(m_Desktop);
        ri = ri.intersected(rect);
        if(ri.isValid()) {
            qDebug(logInvalid) << WINDOW_ID(pWin->m_Info.m_OrderInfo.windowId)
            << pWin->m_Info.m_rectWindow
            << "update:" << ri;
            QImage img = m_pConnect->GetImage(ri);
            QRect rImg(ri.x() - rw.x(), ri.y() - rw.y(), ri.width(), ri.height());
            pWin->slotUpdateRect(rImg, img);
        }
    }
}

int CRailManageWindows::slotSetDesktopSize(int width, int height)
{
    m_Desktop = QRect(0, 0, width, height);
    return 0;
}
