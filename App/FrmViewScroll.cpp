#include "FrmViewScroll.h"
#include <QDebug>

CFrmViewScroll::CFrmViewScroll(CFrmViewer *pView, QWidget *parent) : QScrollArea(parent)
{
    if(pView)
        setWidget(pView);
    setAlignment(Qt::AlignCenter);
    setBackgroundRole(QPalette::Dark);
    setFocusPolicy(Qt::NoFocus);
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
