// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QMouseEvent>
#include "RAILManageWindows.h"
#include "RAILWindow.h"

static Q_LOGGING_CATEGORY(log, "FreeRDP.Rail.Window")
static Q_LOGGING_CATEGORY(logMouse, "FreeRDP.Rail.Window.Mouse")

CRAILWindow::CRAILWindow(QWidget *parent)
    : CFrmViewer{parent}
    , m_pRail(nullptr)
    , m_pManage(nullptr)
    , m_WindowId(0)
    , m_ShowState(0)
    , m_WindowOffsetX(0)
    , m_WindowOffsetY(0)
    , m_WindowWidth(0)
    , m_WindowHeight(0)
    , m_VisibleOffsetX(0)
    , m_VisibleOffsetY(0)
    , m_bServerExit(false)
{
    installEventFilter(this);
    //qDebug(log) << "CRailWindow origin flags:" << windowFlags();
    setWindowFlags(Qt::FramelessWindowHint);
    //qDebug(log) << "CRailWindow initial flags:" << windowFlags();
    slotSetAdaptWindows(ADAPT_WINDOWS::Original);
    setEnabled(true);
    setVisible(false);
}

bool CRAILWindow::eventFilter(QObject *watched, QEvent *event)
{
    //qDebug(log) << WINDOW_ID(GetWindowId()) << Q_FUNC_INFO << event << WINDOW_ID(GetWindowId());
    switch(event->type()) {
    case QEvent::WindowStateChange:
    {
        QWindowStateChangeEvent * stateEvent = dynamic_cast<QWindowStateChangeEvent*>(event);
        if(!stateEvent) break;
        if(Qt::WindowMinimized == stateEvent->oldState() && m_pRail)
            m_pRail->slotClientSystemCommand(GetWindowId(), SC_RESTORE);
        break;
    }
    case QEvent::WindowDeactivate:
    {
        // if(m_pRail)
        //     m_pRail->slotClientActivate(GetWindowId(), false);
        break;
    }
    case QEvent::WindowActivate: {
        // if(m_pRail)
        //     m_pRail->slotClientActivate(GetWindowId(), true);
        break;
    }
    default:
        break;
    }
    return false ;
}

/*!
 *  [2.2.1.3.1.2.1 New or Existing Window](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/2d36758d-3fb5-4823-8c4b-9b81a9c8ff3e)
 *
 * | Value | Meaning                                         |
 * |:------|:------------------------------------------------|
 * | 0x00  |Do not show the window.                          |
 * | 0x02  |Show the window minimized.                       |
 * | 0x03  |Show the window maximized.                       |
 * | 0x05  |Show the window in its current size and position.|
*/
int CRAILWindow::Show(UINT32 state)
{
    qDebug(log) << WINDOW_ID(GetWindowId()) << "Show state:" << state;
    m_ShowState = state;
    switch(state){
    case SW_HIDE:
        hide();
        break;
    case SW_SHOWMINIMIZED:
        showMinimized();
        break;
    case SW_SHOWMAXIMIZED:
        showMaximized();
        break;
    case SW_SHOW:
        show();
        break;
    default:
        qWarning(log) << WINDOW_ID(GetWindowId()) << "Don't support show state:" << state
                      << "See: https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rdperp/2d36758d-3fb5-4823-8c4b-9b81a9c8ff3e";
        break;
    }
    return 0;
}

void CRAILWindow::SetServerExit(bool bExit)
{
    m_bServerExit = bExit;
}

void CRAILWindow::mousePressEvent(QMouseEvent *event)
{
    QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->pos();
#endif

    if(TranslationMousePoint(pos, pos)) return;
    // event->buttons() 产生事件时，按键的状态
    // event->button() 触发当前事件的按键
    /*
    qDebug(logMouse) << WINDOW_ID(GetWindowId()) << "CRailWindow::mousePressEvent"
                     << event << event->button() << event->buttons() << pos;//*/
    emit sigMousePressEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();
}

void CRAILWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->pos();
#endif
    if(TranslationMousePoint(pos, pos)) return;
    // event->buttons() 产生事件时，按键的状态
    // event->button() 触发当前事件的按键
    /*
    qDebug(logMouse) << WINDOW_ID(GetWindowId()) << "CRailWindow::mouseReleaseEvent"
                     << event << event->button() << event->buttons() << pos;//*/
    emit sigMouseReleaseEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();
}

void CRAILWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->pos();
#endif
    if(TranslationMousePoint(pos, pos)) return;
    // event->buttons() 产生事件时，按键的状态
    // event->button() 触发当前事件的按键
    /*
    qDebug(logMouse) << WINDOW_ID(GetWindowId()) << "CRailWindow::mouseMoveEvent"
                     << event->button() << event->buttons() << pos;//*/
    emit sigMouseMoveEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();
}

void CRAILWindow::wheelEvent(QWheelEvent *event)
{
    QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->pos();
#endif
    if(TranslationMousePoint(pos, pos)) return;
    /*
    qDebug(logMouse) << WINDOW_ID(GetWindowId()) << "CRailWindow::wheelEvent"
                     << event->buttons() << event->angleDelta() << pos;//*/
    emit sigWheelEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();
}

int CRAILWindow::TranslationMousePoint(QPointF inPos, QPointF &outPos)
{
    outPos.setX(GetWindowOffsetX() + inPos.x());
    outPos.setY(GetWindowOffsetY() + inPos.y());
    return 0;
}

void CRAILWindow::closeEvent(QCloseEvent *event)
{
    qDebug(log) << WINDOW_ID(GetWindowId()) << Q_FUNC_INFO << event;
    if(m_pManage)
        m_pManage->WindowDelete(GetWindowId());
    if(m_pRail && !m_bServerExit)
        m_pRail->slotClientSystemCommand(GetWindowId(), SC_CLOSE);
}

void CRAILWindow::focusInEvent(QFocusEvent *event)
{
    //qDebug(log) << WINDOW_ID(GetWindowId()) << Q_FUNC_INFO << event;
    // if(Qt::MouseFocusReason == event->reason())
    //     return;
    // if(m_pRail)
    //     m_pRail->slotClientActivate(GetWindowId(), true);
}

void CRAILWindow::focusOutEvent(QFocusEvent *event)
{
    //qDebug(log) << WINDOW_ID(GetWindowId()) << Q_FUNC_INFO << event;
    // if(Qt::MouseFocusReason == event->reason())
    //     return;
    // if(m_pRail)
    //     m_pRail->slotClientActivate(GetWindowId(), true);
}

INT32 CRAILWindow::GetVisibleOffsetX() const
{
    return m_VisibleOffsetX;
}

void CRAILWindow::SetVisibleOffsetX(INT32 newVisibleOffsetX)
{
    m_VisibleOffsetX = newVisibleOffsetX;
}

INT32 CRAILWindow::GetVisibleOffsetY() const
{
    return m_VisibleOffsetY;
}

void CRAILWindow::SetVisibleOffsetY(INT32 newVisibleOffsetY)
{
    m_VisibleOffsetY = newVisibleOffsetY;
}

UINT32 CRAILWindow::GetShowState() const
{
    return m_ShowState;
}

void CRAILWindow::SetShowState(UINT32 newShowState)
{
    m_ShowState = newShowState;
}

UINT32 CRAILWindow::GetWindowId() const
{
    return m_WindowId;
}

void CRAILWindow::SetWindowId(UINT32 newWindowId)
{
    m_WindowId = newWindowId;
}

INT32 CRAILWindow::GetWindowOffsetX() const
{
    return m_WindowOffsetX;
}

void CRAILWindow::SetWindowOffsetX(INT32 newWindowOffsetX)
{
    m_WindowOffsetX = newWindowOffsetX;
}

INT32 CRAILWindow::GetWindowOffsetY() const
{
    return m_WindowOffsetY;
}

void CRAILWindow::SetWindowOffsetY(INT32 newWindowOffsetY)
{
    m_WindowOffsetY = newWindowOffsetY;
}

UINT32 CRAILWindow::GetWindowWidth() const
{
    return m_WindowWidth;
}

void CRAILWindow::SetWindowWidth(UINT32 newWindowWidth)
{
    m_WindowWidth = newWindowWidth;
}

UINT32 CRAILWindow::GetWindowHeight() const
{
    return m_WindowHeight;
}

void CRAILWindow::SetWindowHeight(UINT32 newWindowHeight)
{
    m_WindowHeight = newWindowHeight;
}
