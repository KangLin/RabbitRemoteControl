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
//    CFrmViewer* pView = GetViewer(index);
//    if(!pView) return;
//    switch (pView->AdaptWindows()) {
//    case CFrmViewer::Auto:
//    case CFrmViewer::OriginalCenter:
//    case CFrmViewer::Original:
//        ui->actionOriginal_O->setChecked(true);
//        break;
//    case CFrmViewer::Zoom:
//        ui->actionZoom_Z->setChecked(true);
//        break;
//    case CFrmViewer::AspectRation:
//        ui->actionKeep_AspectRation_K->setChecked(true);
//        break;
//    }
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
//    if(ui->actionZoom_Z->isChecked()) {
//        pScroll->setWidgetResizable(true);
//        pView->SetAdaptWindows(CFrmViewer::Zoom);
//    } else if(ui->actionKeep_AspectRation_K->isChecked()) {
//        pScroll->setWidgetResizable(true);
//        pView->SetAdaptWindows(CFrmViewer::AspectRation);
//    }

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
    QScrollArea* pScroll = dynamic_cast<QScrollArea*>(m_pTab->currentWidget());
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

QScrollArea* CViewTable::GetScrollArea(int index)
{
    return dynamic_cast<QScrollArea*>(m_pTab->widget(index));
}

QWidget *CViewTable::GetViewer(int index)
{
    QScrollArea* pScroll = GetScrollArea(index);
    if(!pScroll) return nullptr;
    
    return dynamic_cast<CFrmViewer*>(pScroll->widget());
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
