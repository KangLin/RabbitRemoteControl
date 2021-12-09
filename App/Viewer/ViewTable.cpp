// Author: Kang Lin <kl222@126.com>

#include "ViewTable.h"
#include <QResizeEvent>
#include <QTabBar>
#include <QDebug>

CViewTable::CViewTable(QWidget *parent) : CView(parent),
    m_pTab(nullptr)
{
    m_pTab = new QTabWidget(this);
    m_pTab->setTabsClosable(true);
    m_pTab->setUsesScrollButtons(true);
    m_pTab->setMovable(true);
    m_pTab->setFocusPolicy(Qt::NoFocus);
    
    bool check = connect(m_pTab, SIGNAL(tabCloseRequested(int)),
                    this, SLOT(slotTabCloseRequested(int)));
    Q_ASSERT(check);
    check = connect(m_pTab, SIGNAL(currentChanged(int)),
            this, SLOT(slotCurrentChanged(int)));
    Q_ASSERT(check);
}

CViewTable::~CViewTable()
{
    for(int i = 0; i < m_pTab->count(); i++)
    {
        CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(GetViewer(i));
        if(pScroll) delete pScroll;
    }
    if(m_pTab)
        delete m_pTab;
}

void CViewTable::slotCurrentChanged(int index)
{
    //qDebug() << "CViewTable::slotCurrentChanged";
    CFrmViewScroll* pView = qobject_cast<CFrmViewScroll*>(GetViewer(index));
    if(pView)
    {
        emit sigAdaptWindows(pView->AdaptWindows());
    } else // is terminal windows
        emit sigAdaptWindows(CFrmViewer::Disable);
}

void CViewTable::slotTabCloseRequested(int index)
{
    QWidget* pView = GetViewer(index);
    if(!pView) return;
    CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(pView);
    if(pScroll)
        emit sigCloseView(pScroll->GetViewer());
    else
        emit sigCloseView(pView);
}

void CViewTable::slotSystemCombination()
{
    QWidget* pView = GetCurrentView();
    if(!pView) return;
    CFrmViewer* pFrmViewer = qobject_cast<CFrmViewer*>(pView);
    if(pFrmViewer)
        pFrmViewer->slotSystemCombination();
}

int CViewTable::AddView(QWidget *pView)
{
    int nIndex = -1;

    CFrmViewer* p = qobject_cast<CFrmViewer*>(pView);
    if(p)
    {
        CFrmViewScroll* pScroll = new CFrmViewScroll(p, m_pTab);
        if(pScroll)
        {
            pScroll->SetAdaptWindows(p->GetAdaptWindows());
            nIndex = m_pTab->addTab(pScroll, p->windowTitle());
        }
    } else
        nIndex = m_pTab->addTab(pView, pView->windowTitle());
    m_pTab->setCurrentIndex(nIndex);

    return 0;
}

int CViewTable::RemoveView(QWidget *pView)
{
    int nIndex = GetViewIndex(pView);
    if(-1 == nIndex) return 0;
    CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(GetViewer(nIndex));
    // Note: The following order cannot be changed
    m_pTab->removeTab(nIndex);
    if(pScroll) delete pScroll;
    return 0;
}

void CViewTable::SetWidowsTitle(QWidget* pView, const QString& szTitle)
{
    if(!pView) return;
    pView->setWindowTitle(szTitle);
    int nIndex = GetViewIndex(pView);
    m_pTab->setTabText(nIndex, szTitle);
}

int CViewTable::SetFullScreen(bool bFull)
{
    if(!m_pTab) return -1;
    ShowTabBar(!bFull);
    if(bFull)
    {
        m_pTab->showFullScreen();
    } else {
        m_pTab->showNormal();
    }
    return 0;
}

int CViewTable::ShowTabBar(bool bShow)
{
    m_pTab->tabBar()->setVisible(bShow);
    return 0;
}

void CViewTable::SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw, QWidget* p)
{
    CFrmViewer* pView = nullptr;
    if(p)
    {
        pView = qobject_cast<CFrmViewer*>(p);
        if(pView)
        {
            if(CFrmViewer::Auto == aw)
                pView->SetAdaptWindows(pView->GetAdaptWindows());
            else
                pView->SetAdaptWindows(aw);
        }
    } else {
        CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(GetViewer(m_pTab->currentIndex()));
        if(pScroll)
            pScroll->SetAdaptWindows(aw);
    }
}

double CViewTable::GetZoomFactor()
{
    CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(GetViewer(m_pTab->currentIndex()));
    if(!pScroll) return -1;
    CFrmViewer* pView = pScroll->GetViewer();
    if(!pView) return -1;
    return pView->GetZoomFactor();
}

void CViewTable::slotZoomFactor(double v)
{
    CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(GetViewer(m_pTab->currentIndex()));
    if(!pScroll) return;
    CFrmViewer* pView = pScroll->GetViewer();
    if(!pView) return;
    pView->SetZoomFactor(v);
}

void CViewTable::slotZoomIn()
{
    CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(GetViewer(m_pTab->currentIndex()));
    if(!pScroll) return;
    CFrmViewer* pView = pScroll->GetViewer();
    if(!pView) return;
    pView->SetZoomFactor(pView->GetZoomFactor() + 0.1);
}

void CViewTable::slotZoomOut()
{
    CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(GetViewer(m_pTab->currentIndex()));
    if(!pScroll) return;
    CFrmViewer* pView = pScroll->GetViewer();
    if(!pView) return;
    pView->SetZoomFactor(pView->GetZoomFactor() - 0.1);
}

QWidget *CViewTable::GetViewer(int index)
{
    if(index < 0 || index >= m_pTab->count())
        return nullptr;
    
    return m_pTab->widget(index);
}

int CViewTable::GetViewIndex(QWidget *pView)
{
    for(int i = 0; i < m_pTab->count(); i++)
    {
        QWidget* p = GetViewer(i);
        if(p == pView)
            return i;
        CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(p);
        if(pScroll)
            if(pScroll->GetViewer() == pView)
                return i;
    }
    return -1;
}

// @note The return QWidget* must is same as CConnecter::GetViewer()
QWidget* CViewTable::GetCurrentView()
{
    QWidget* pView = m_pTab->currentWidget();
    if(!pView) return pView;
    CFrmViewScroll* pScroll = qobject_cast<CFrmViewScroll*>(pView);
    if(pScroll)
        return pScroll->GetViewer();
    return pView;
}

void CViewTable::resizeEvent(QResizeEvent *event)
{
    if(!m_pTab)
        return;
    m_pTab->move(0, 0);
    m_pTab->resize(event->size());
}
