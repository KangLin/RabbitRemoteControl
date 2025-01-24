// Author: Kang Lin <kl222@126.com>

#include "RAILManageWindows.h"
#include "ConnectFreeRDP.h"
#include <QApplication>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "FreeRDP.Rail.ManageWindows")
static Q_LOGGING_CATEGORY(logWin, "FreeRDP.Rail.ManageWindows.Window")
static Q_LOGGING_CATEGORY(logUpdate, "FreeRDP.Rail.Update.Window")
static Q_LOGGING_CATEGORY(logInvalid, "FreeRDP.Rail.Update.Invalid")

CRAILManageWindows::CRAILManageWindows(QObject *parent)
    : QObject{parent}
    , m_pRail(nullptr)
{}

void CRAILManageWindows::slotClean()
{
    foreach(CRAILWindow* pWin, m_Windows) {
        if(pWin) {
            pWin->close();
            pWin->deleteLater();
        }
    }
    m_Windows.clear();
    disconnect();
    m_pRail = nullptr;
    return;
}

int CRAILManageWindows::Connect(CRAIL *pRail)
{
    bool check = false;
    m_pRail = pRail;
    Q_ASSERT(pRail && m_pConnect);
    check = connect(pRail, &CRAIL::sigWindow,
                    this,  &CRAILManageWindows::slotWindow);
    Q_ASSERT(check);
    check = connect(pRail, &CRAIL::sigWindowDelete,
                    this, &CRAILManageWindows::slotWindowDelete);
    Q_ASSERT(check);
    check = connect(pRail, &CRAIL::sigWindowIcon,
                    this, &CRAILManageWindows::slotWindowIcon);
    Q_ASSERT(check);
    check = connect(m_pConnect, SIGNAL(sigUpdateRect(const QRect&, const QImage&)),
                    this, SLOT(slotInvalidateRect(const QRect&, const QImage&)));
    Q_ASSERT(check);
    check = connect(m_pConnect, &CConnectFreeRDP::sigSetDesktopSize,
                    this, &CRAILManageWindows::slotSetDesktopSize);
    Q_ASSERT(check);
    return 0;
}

/*
 * [2.2.1.3.1.2.1 New or Existing Window](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/2d36758d-3fb5-4823-8c4b-9b81a9c8ff3e)
 */
void CRAILManageWindows::slotWindow(QSharedPointer<CRAILInfo> info)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!m_pRail) return;

    UINT32 fieldFlags = info->m_FieldFlags;
    UINT32 windowId = info->m_WindowId;

    CRAILWindow* pWin = nullptr;
    auto it = m_Windows.find(windowId);
    if(m_Windows.end() != it)
        pWin = m_Windows.value(windowId);
    if (!pWin && (fieldFlags & WINDOW_ORDER_STATE_NEW))
    {
        qDebug(log) << "Create:" << WINDOW_ID(windowId) << "Owner:" << HEX(info->m_OwnerWindowId);
        if(0 != info->m_OwnerWindowId) {
            auto itParent = m_Windows.find(info->m_OwnerWindowId);
            if(m_Windows.end() == it)
                pWin = new CRAILWindow();
            else
                pWin = new CRAILWindow(itParent.value());
        } else
            pWin = new CRAILWindow();
        if(!pWin) {
            qCritical(log) << "new CRailWindow() fail";
            return;
        }
        pWin->SetWindowId(windowId);
        pWin->m_pRail = m_pRail;
        pWin->m_pManage = this;
        m_Windows.insert(windowId, pWin);
        int nRet = 0;
        if(m_pConnect) {
            //qDebug(log) << "m_pConnect->SetRailViewer:" << pWin;
            nRet = m_pConnect->SetRAILViewer(pWin);
            if(nRet) {
                qCritical(log) << "Connect SetViewer fail" << nRet;
                return;
            }
            //TODO:
            bool check = connect(
                pWin, &CRAILWindow::destroyed,
                this, [&](QObject * obj){
                    CRAILWindow* pW = qobject_cast<CRAILWindow*>(obj);
                    if(pW)
                        WindowDelete(pW->GetWindowId());
                });
            Q_ASSERT(check);
        }
    } else {
        qDebug(log) << "Update:" << WINDOW_ID(windowId) << "Owner:" << HEX(info->m_OwnerWindowId);
    }

    if(!pWin) {
        qCritical(log) << "Get window fail";
        return;
    }

    /* Update Parameters */

    if (fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE) {
        pWin->SetWindowWidth(info->m_WindowWidth);
        pWin->SetWindowHeight(info->m_WindowHeight);
        pWin->resize(pWin->GetWindowWidth(), pWin->GetWindowHeight());
        pWin->slotSetDesktopSize(pWin->GetWindowWidth(), pWin->GetWindowHeight());
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET) {
        pWin->SetWindowOffsetX(info->m_WindowOffsetX);
        pWin->SetWindowOffsetY(info->m_WindowOffsetY);

        /* 现在很多 linux 用 wayland 作为桌面显示，这样会出现一个问题，
           由于没有坐标系统，导致无边框窗体无法拖动和定位（即 QWidge::move 失效）。
          （一般是 Qt6 开始强制默认优先用 wayland ，之前 Qt5 是默认有 xcb 则优先用 xcb），
           你需要在 main 函数最前面加一行 qputenv("QT_QPA_PLATFORM", "xcb");
         */
        pWin->move(pWin->GetWindowOffsetX(), pWin->GetWindowOffsetY());
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_SHOW)
    {
        pWin->Show(info->m_ShowState);
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

    // [Window Styles](https://learn.microsoft.com/windows/win32/winmsg/window-styles]
    if (fieldFlags & WINDOW_ORDER_FIELD_STYLE)
    {
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
        UpdateWindow(pWin, info.data(), false);
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET)
    {
        pWin->SetVisibleOffsetX(info->m_VisibleOffsetX);
        pWin->SetVisibleOffsetY(info->m_VisibleOffsetY);
    }

    if (fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY)
    {
        //UpdateWindow(pWin, info.data(), true);
    }
}

void CRAILManageWindows::slotWindowDelete(QSharedPointer<CRAILInfo> info)
{
    if(!m_pRail) return;
    UINT32 windowId = info->m_WindowId;
    WindowDelete(windowId);
}

void CRAILManageWindows::WindowDelete(UINT32 windowId)
{
    auto it = m_Windows.find(windowId);
    if(m_Windows.end() == it)
        return;
    CRAILWindow* pWin = it.value();
    m_Windows.remove(windowId);
    if(pWin) {
        pWin->SetServerExit(true);
        pWin->close();
        pWin->deleteLater();
    }
}

void CRAILManageWindows::slotWindowIcon(QSharedPointer<CRAILInfo> info, QImage img)
{
    UINT32 flags = info->m_FieldFlags;
    if(!(WINDOW_ORDER_ICON & flags))
        return;

    auto it = m_Windows.find(info->m_WindowId);
    if(m_Windows.end() == it)
        return;
    CRAILWindow* pWin = it.value();
    if(pWin)
        pWin->setWindowIcon(QIcon(QPixmap::fromImage(img)));
}

int CRAILManageWindows::UpdateWindow(
    CRAILWindow *pWin, CRAILInfo *info, bool bVisiable)
{
    if (info->m_Updates.isEmpty()) {
        qDebug(logUpdate) << "UpdateWindow: the update rects is empty";
        return 0;
    }

    if(SW_HIDE == pWin->GetShowState()
        || SW_SHOWMINIMIZED == pWin->GetShowState()) {
        qDebug(logUpdate) << WINDOW_ID(pWin->GetWindowId()) << "Widow is hide"
                          << pWin->GetShowState();
        return 0;
    }

    QVector<QRect> *rects;
    if(bVisiable)
        rects = &info->m_Visibilities;
    else
        rects = &info->m_Updates;
    foreach (QRect ro, *rects) {
        UINT32 x = info->m_WindowOffsetX;
        UINT32 y = info->m_WindowOffsetY;
        if(bVisiable)
        {
            x = info->m_VisibleOffsetX;
            y = info->m_VisibleOffsetY;
        }
        QRect rImg(ro.x() + x, ro.y() + y, ro.width(), ro.height());
        QRect ri = rImg.intersected(QRect(m_Desktop));
        if(!ri.isValid()) {
            qDebug(logUpdate) << WINDOW_ID(pWin->GetWindowId())
                              << "intersecte invalid" << ri;
            continue;
        }
        QImage img = m_pConnect->GetImage(ri);
        QRect ru = QRect(ri.x() - x, ri.y() - y, ri.width(), ri.height());
        pWin->slotUpdateRect(ru, img);
        qDebug(logUpdate) << WINDOW_ID(pWin->GetWindowId())
                          << "Origin:" << ro << "Image:" << rImg
                          << "Intersecte:" << ri << "Update:" << ru;
    }
    return 0;
}

void CRAILManageWindows::slotInvalidateRect(const QRect &rect, const QImage &img)
{
    if(!rect.isValid() || img.isNull()) {
        qCritical(logInvalid) << "The parameters is null";
        return;
    }
    foreach(auto pWin, m_Windows) {
        if(!pWin) {
            qWarning(logInvalid) << "The window is null";
            continue;
        }
        if(SW_HIDE == pWin->GetShowState()
            || SW_SHOWMINIMIZED == pWin->GetShowState()) {
            qDebug(logInvalid) << WINDOW_ID(pWin->GetWindowId())
                               << "Widow is hide"
                               << pWin->GetShowState();
            continue;
        }

        QRect ri;
        const QRect rw(pWin->GetWindowOffsetX(), pWin->GetWindowOffsetY(),
                       pWin->GetWindowWidth(), pWin->GetWindowHeight());
        ri = rw.intersected(m_Desktop);
        if(ri.isValid())
            ri = ri.intersected(rect);
        if(!ri.isValid()) {
            qDebug(logInvalid) << WINDOW_ID(pWin->GetWindowId())
                               << "intersecte invalid" << ri;
            continue;
        }
        QRect r(ri.x() - rect.x(), ri.y() - rect.y(), ri.width(), ri.height());
        QImage cImg = img.copy(r);
        QRect ru(ri.x() - rw.x(), ri.y() - rw.y(), ri.width(), ri.height());
        pWin->slotUpdateRect(ru, cImg);
        qDebug(logInvalid) << WINDOW_ID(pWin->GetWindowId())
                           << "Window:" << rw << "Invalid:" << rect
                           << "Intersected:" << ri << "update:" << ru;
    }
}

int CRAILManageWindows::slotSetDesktopSize(int width, int height)
{
    m_Desktop = QRect(0, 0, width, height);
    return 0;
}
