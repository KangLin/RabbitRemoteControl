#include "FrmViewScroll.h"

CFrmViewScroll::CFrmViewScroll(QWidget *parent) : QScrollArea(parent)
{
    m_pView = new CFrmViewer(this);
    
    setWidget(m_pView);
    setAlignment(Qt::AlignCenter);
    setBackgroundRole(QPalette::Dark);
    setFocusPolicy(Qt::NoFocus);
    setWidgetResizable(true);
}

void CFrmViewScroll::SetAdaptWindows(ADAPT_WINDOWS aw)
{
    if(!m_pView) return;
    if(Original == aw)
        setWidgetResizable(false);
    else
        setWidgetResizable(true);
    m_pView->SetAdaptWindows(aw);
    return;
}

ADAPT_WINDOWS CFrmViewScroll::AdaptWindows()
{
    if(m_pView)
        return m_pView->AdaptWindows();
    return Auto;
}

CFrmViewer* CFrmViewScroll::GetViewer()
{
    return m_pView;
}
