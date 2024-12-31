// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QMouseEvent>

#include "RailWindow.h"

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

static Q_LOGGING_CATEGORY(log, "FreeRDP.Rail.Window")
CRailWindow::CRailWindow(QWidget *parent)
    : CFrmViewer{parent}
{
    //qDebug(log) << "CRailWindow origin flags:" << windowFlags();
    setWindowFlags(Qt::FramelessWindowHint);
    //qDebug(log) << "CRailWindow initial flags:" << windowFlags();
    slotSetAdaptWindows(ADAPT_WINDOWS::Original);
    setEnabled(true);
    qDebug(log) << "minimumSize:" << minimumSize()
                << "maximumSize:" << maximumSize();
}

int CRailWindow::Show(UINT32 state)
{
    switch(state){
    case SW_HIDE:
        hide();
        break;
    case SW_SHOWNORMAL:
        showNormal();
        setFocus();
        break;
    case SW_SHOWMINIMIZED:
        showMinimized();
        setFocus();
        break;
    case SW_SHOWMAXIMIZED:
        showMaximized();
        setFocus();
        break;
    case SW_SHOWNOACTIVATE:
        showNormal();
        break;
    case SW_SHOW:
        show();
        setFocus();
        break;
    case SW_MINIMIZE:
    case SW_SHOWMINNOACTIVE:
        showMinimized();
        break;
    case SW_SHOWNA:
        show();
        break;
    case SW_RESTORE:
    case SW_SHOWDEFAULT:
        show();
        break;
    case SW_FORCEMINIMIZE:
        showMinimized();
        break;
    default:
        break;
    }
    return 0;
}

void CRailWindow::mousePressEvent(QMouseEvent *event)
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
    //qDebug(log) << "CRailWindow::mousePressEvent" << event << event->button() << event->buttons() << pos;
    emit sigMousePressEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();
}

void CRailWindow::mouseReleaseEvent(QMouseEvent *event)
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
    //qDebug(log) << "CRailWindow::mouseReleaseEvent" << event << event->button() << event->buttons() << pos;
    emit sigMouseReleaseEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();
}

void CRailWindow::mouseMoveEvent(QMouseEvent *event)
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
    //qDebug(log) << "CRailWindow::mouseMoveEvent" << event->button() << event->buttons() << pos;
    emit sigMouseMoveEvent(event, QPoint(pos.x(), pos.y()));
    emit sigMouseMoveEvent(event);
    event->accept();
}

void CRailWindow::wheelEvent(QWheelEvent *event)
{
    QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->pos();
#endif
    if(TranslationMousePoint(pos, pos)) return;
    //qDebug(log) << "CRailWindow::wheelEvent" << event->buttons() << event->angleDelta() << pos;
    emit sigWheelEvent(event, QPoint(pos.x(), pos.y()));
    event->accept();
}

int CRailWindow::TranslationMousePoint(QPointF inPos, QPointF &outPos)
{
    outPos.setX(m_Info.m_rectWindow.x() + inPos.x());
    outPos.setY(m_Info.m_rectWindow.y() + inPos.y());
    return 0;
}
