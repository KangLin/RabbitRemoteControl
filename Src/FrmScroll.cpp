// Author: Kang Lin <kl222@126.com>

#include <QScrollBar>
#include <QLoggingCategory>

#include "FrmScroll.h"

static Q_LOGGING_CATEGORY(log, "Client.FrmScroll")
CFrmScroll::CFrmScroll(CFrmViewer *pView, QWidget *parent) : QScrollArea(parent)
{
    qDebug(log) << Q_FUNC_INFO;
    setFocusPolicy(Qt::NoFocus);
    if(pView)
    {
        setWidget(pView);
        pView->show();
        bool check = connect(pView,
                             SIGNAL(sigMouseMoveEvent(QMouseEvent*)),
                             this,
                             SLOT(slotMouseMoveEvent(QMouseEvent*)));
        Q_ASSERT(check);
    }
    setAlignment(Qt::AlignCenter);
    //setBackgroundRole(QPalette::Dark);
    setWidgetResizable(false);
    slotSetAdaptWindows(GetViewer()->GetAdaptWindows());
}

CFrmScroll::~CFrmScroll()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CFrmScroll::slotSetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw)
{
    CFrmViewer* pView = qobject_cast<CFrmViewer*>(widget());
    if(!pView) return;
    if(CFrmViewer::ADAPT_WINDOWS::Original == aw || CFrmViewer::ADAPT_WINDOWS::Zoom == aw)
    {
        if(widgetResizable())
            setWidgetResizable(false);
    } else {
        if(!widgetResizable())
            setWidgetResizable(true);
    }
    pView->slotSetAdaptWindows(aw);
    return;
}

CFrmViewer::ADAPT_WINDOWS CFrmScroll::AdaptWindows()
{
    CFrmViewer* pView = qobject_cast<CFrmViewer*>(widget());
    if(pView)
        return pView->GetAdaptWindows();
    return CFrmViewer::ADAPT_WINDOWS::Auto;
}

CFrmViewer* CFrmScroll::GetViewer()
{
    return qobject_cast<CFrmViewer*>(widget());
}

void CFrmScroll::slotMouseMoveEvent(QMouseEvent *event)
{
    int area = 10;
    CFrmViewer* pFrmView = qobject_cast<CFrmViewer*>(sender());
    QScrollBar* pHBar = horizontalScrollBar();
    QScrollBar* pVBar = verticalScrollBar();
    if(pHBar->isHidden() && pVBar->isHidden())
        return;
    
    int nVWidth = 0;
    if(!pVBar->isHidden()) nVWidth = pVBar->geometry().width();
    QPoint pos = pFrmView->mapToParent(event->pos());
    if(pos.x() < area && pHBar->value() > pHBar->minimum())
        pHBar->setValue(pHBar->value() - pHBar->singleStep());
    else if(pos.x() > width() - area - nVWidth
            && pHBar->value() < pHBar->maximum())
        pHBar->setValue(pHBar->value() + pHBar->singleStep());
    
    int nHHeight = 0;
    if(!pHBar->isHidden()) nHHeight = pHBar->geometry().height();
    if(pos.y() < area && pVBar->value() > pVBar->minimum())
        pVBar->setValue(pVBar->value() - pVBar->singleStep());
    else if(pos.y() > height() - area - nHHeight
            && pVBar->value() < pVBar->maximum())
        pVBar->setValue(pVBar->value() + pVBar->singleStep());
}
