//! @author: Kang Lin(kl222@126.com)

#include "ViewTable.h"
#include <QResizeEvent>
#include <QTabBar>

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
    if(m_pTab)
        delete m_pTab;
}

void CViewTable::slotCurrentChanged(int index)
{
    CFrmViewer* pView = qobject_cast<CFrmViewer*>(GetViewer(index));
    if(pView)
    {
        QScrollArea* pScroll = GetScrollArea(index);
        if(pScroll)
        {   if(CFrmViewer::Original == pView->AdaptWindows())
                pScroll->setWidgetResizable(false);
            else
                pScroll->setWidgetResizable(true);
        }
        emit sigAdaptWindows(pView->AdaptWindows());
    }
}

void CViewTable::slotTabCloseRequested(int index)
{
    QScrollArea* pScroll = GetScrollArea(index);
    if(!pScroll) return;
    QWidget* pView = pScroll->takeWidget();
    delete pScroll;
    emit sigCloseView(pView);
}

int CViewTable::AddView(QWidget *pView)
{
    QScrollArea* pScroll = new QScrollArea(m_pTab);
    pScroll->setWidget(pView);
    pScroll->setAlignment(Qt::AlignCenter);
    pScroll->setBackgroundRole(QPalette::Dark);
    pScroll->setFocusPolicy(Qt::NoFocus);
    pScroll->setWidgetResizable(true);
    int nIndex = m_pTab->addTab(pScroll, pView->windowTitle());
    m_pTab->setCurrentIndex(nIndex);
    return 0;
}

int CViewTable::RemoveView(QWidget *pView)
{
    int nIndex = GetViewIndex(pView);
    if(-1  == nIndex) return 0;
    QScrollArea* pScroll = GetScrollArea(nIndex);
    if(!pScroll) return 0;
    // the clild windows is deleted by CConnecter
    pScroll->takeWidget();
    m_pTab->removeTab(nIndex);
    delete pScroll;
    return 0;
}

QWidget* CViewTable::GetCurrentView()
{
    QScrollArea* pScroll = qobject_cast<QScrollArea*>(m_pTab->currentWidget());
    return pScroll->widget();
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
    if(bFull)
    {
        m_pTab->tabBar()->hide();
        m_pTab->showFullScreen();
    } else {
        m_pTab->showNormal();
        m_pTab->tabBar()->show();
    }
    return 0;
}

void CViewTable::SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw, QWidget* p)
{
    int nIndex = m_pTab->currentIndex();
    CFrmViewer* pView = nullptr;
    if(p)
    {
        pView = qobject_cast<CFrmViewer*>(p);
        if(!pView) return;
    } else {
        pView = qobject_cast<CFrmViewer*>(GetViewer(nIndex));
    }
    if(pView)
        pView->SetAdaptWindows(aw);
    
    nIndex = GetViewIndex(p);
    if(-1 == nIndex)
        nIndex = m_pTab->currentIndex();
    QScrollArea* pScroll = GetScrollArea(nIndex);
    if(pScroll)
    {   if(CFrmViewer::Original == aw)
            pScroll->setWidgetResizable(false);
        else
            pScroll->setWidgetResizable(true);
    }
}

QScrollArea* CViewTable::GetScrollArea(int index)
{
    if(index < 0 || index >= m_pTab->count())
        return nullptr;
    return qobject_cast<QScrollArea*>(m_pTab->widget(index));
}

QWidget *CViewTable::GetViewer(int index)
{
    QScrollArea* pScroll = GetScrollArea(index);
    if(!pScroll) return nullptr;
    
    return qobject_cast<CFrmViewer*>(pScroll->widget());
}

int CViewTable::GetViewIndex(QWidget *pView)
{
    for(int i = 0; i < m_pTab->count(); i++)
    {
        if(GetViewer(i) == pView)
            return i;
    }
    return -1;
}

void CViewTable::resizeEvent(QResizeEvent *event)
{
    if(!m_pTab)
        return;
    m_pTab->move(0, 0);
    m_pTab->resize(event->size());
}
