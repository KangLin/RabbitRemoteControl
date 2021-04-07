#include "FrmViewScroll.h"
#include <QDebug>
#include <QGuiApplication>

CFrmViewScroll::CFrmViewScroll(CFrmViewer *pView, QWidget *parent) : QScrollArea(parent)
{
    //BUG: 滚动条接收TAB键调整，导致视图接收不到TAB键
    setFocusPolicy(Qt::NoFocus);
    if(pView)
        setWidget(pView);
    setAlignment(Qt::AlignCenter);
    //setBackgroundRole(QPalette::Dark);
    setWidgetResizable(true);
}

CFrmViewScroll::~CFrmViewScroll()
{
    //qDebug() << "CFrmViewScroll::~CFrmViewScroll()";
    QWidget* pView = takeWidget();
    Q_UNUSED(pView);
}

void CFrmViewScroll::SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw)
{
    CFrmViewer* pView = qobject_cast<CFrmViewer*>(widget());
    if(!pView) return;
    if(CFrmViewer::Original == aw)
        setWidgetResizable(false);
    else
        setWidgetResizable(true);
    pView->SetAdaptWindows(aw);
    return;
}

CFrmViewer::ADAPT_WINDOWS CFrmViewScroll::AdaptWindows()
{
    CFrmViewer* pView = qobject_cast<CFrmViewer*>(widget());
    if(pView)
        return pView->AdaptWindows();
    return CFrmViewer::Auto;
}

CFrmViewer* CFrmViewScroll::GetViewer()
{
    return qobject_cast<CFrmViewer*>(widget());
}
