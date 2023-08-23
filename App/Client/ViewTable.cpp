// Author: Kang Lin <kl222@126.com>

#include "ViewTable.h"
#include <QResizeEvent>
#include <QTabBar>
#include <QStyleOption>
#include <QDebug>
#include <QScrollBar>
#include <QFileInfo>
#include <QDir>

#include "mainwindow.h"

CViewTable::CViewTable(QWidget *parent) : CView(parent),
    m_pTab(nullptr)
{
    bool check = false;
    setFocusPolicy(Qt::NoFocus);

    m_pTab = new QTabWidget(this);
    m_pTab->setTabsClosable(true);
    m_pTab->setUsesScrollButtons(true);
    m_pTab->setMovable(true);
    m_pTab->setFocusPolicy(Qt::NoFocus);
    if(this->parent())
    {
        MainWindow* p = dynamic_cast<MainWindow*>(this->parent());
        if(p)
        {
            m_pMainWindow = p;
            m_pTab->setTabPosition(p->m_Parameter.GetTabPosition());
            check = connect(&p->m_Parameter, SIGNAL(sigTabPositionChanged()),
                            this, SLOT(slotTabPositionChanged()));
            Q_ASSERT(check);
        }
    }
    
    check = connect(m_pTab, SIGNAL(tabCloseRequested(int)),
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
        CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(GetViewer(i));
        if(pScroll) delete pScroll;
    }
    if(m_pTab)
        delete m_pTab;
}

void CViewTable::slotCurrentChanged(int index)
{
    //qDebug() << "CViewTable::slotCurrentChanged";
    CViewFrmScroll* pView = qobject_cast<CViewFrmScroll*>(GetViewer(index));
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
    CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(pView);
    if(pScroll)
        emit sigCloseView(pScroll->GetViewer());
    else
        emit sigCloseView(pView);
}

void CViewTable::slotTabPositionChanged()
{
    MainWindow* p = dynamic_cast<MainWindow*>(parent());
    if(!p || !m_pTab) return;
    m_pTab->setTabPosition(p->m_Parameter.GetTabPosition());
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
        CViewFrmScroll* pScroll = new CViewFrmScroll(p, m_pTab);
        if(pScroll)
        {
            pScroll->SetAdaptWindows(p->GetAdaptWindows());
            pScroll->setFocusPolicy(Qt::NoFocus);
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
    CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(GetViewer(nIndex));
    // Note: The following order cannot be changed
    m_pTab->removeTab(nIndex);
    if(pScroll) delete pScroll;
    return 0;
}

void CViewTable::SetWidowsTitle(QWidget* pView, const QString& szTitle,
                                const QIcon &icon, const QString &szToolTip)
{
    if(!pView) return;
    pView->setWindowTitle(szTitle);
    int nIndex = GetViewIndex(pView);
    m_pTab->setTabText(nIndex, szTitle);
    if(m_pMainWindow->m_Parameter.GetEnableTabToolTip())
        m_pTab->setTabToolTip(nIndex, szToolTip);
    else
        m_pTab->setTabToolTip(nIndex, "");
    if(m_pMainWindow->m_Parameter.GetEnableTabIcon())
        m_pTab->setTabIcon(nIndex, icon);
    else
        m_pTab->setTabIcon(nIndex, QIcon());
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

int CViewTable::Screenslot(const QString &szFile, bool bRemoteDesktop)
{
    QFileInfo fi(szFile);
    QString ext = fi.suffix().toUpper();
    QDir d;
    if(!d.exists(fi.path()))
        d.mkpath(fi.path());
    CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(GetViewer(m_pTab->currentIndex()));
    if(!pScroll) return -1;
    CFrmViewer* pView = pScroll->GetViewer();
    if(!pView) return -2;
    if(bRemoteDesktop)
    {
        QImage img = pView->GrabImage();
        if(img.save(szFile, ext.toStdString().c_str()))
            return 0;
        return -3;
    }
    
    int w = pScroll->size().width();
    int h = pScroll->size().height();
    QScrollBar* pHBar = pScroll->horizontalScrollBar();
    QScrollBar* pVBar = pScroll->verticalScrollBar();
    if(pHBar && pHBar->isVisible()) h -= pHBar->height();
    if(pVBar && pVBar->isVisible()) w -= pVBar->width();
    QPixmap pixmap(QSize(w, h));
    pScroll->render(&pixmap);
    if(pixmap.toImage().save(szFile, ext.toStdString().c_str()))
    {
        return 0;
    }
    return -4;
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
        CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(GetViewer(m_pTab->currentIndex()));
        if(pScroll)
            pScroll->SetAdaptWindows(aw);
    }
}

double CViewTable::GetZoomFactor()
{
    CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(GetViewer(m_pTab->currentIndex()));
    if(!pScroll) return -1;
    CFrmViewer* pView = pScroll->GetViewer();
    if(!pView) return -1;
    return pView->GetZoomFactor();
}

void CViewTable::slotZoomFactor(double v)
{
    CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(GetViewer(m_pTab->currentIndex()));
    if(!pScroll) return;
    CFrmViewer* pView = pScroll->GetViewer();
    if(!pView) return;
    pView->SetZoomFactor(v);
}

void CViewTable::slotZoomIn()
{
    CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(GetViewer(m_pTab->currentIndex()));
    if(!pScroll) return;
    CFrmViewer* pView = pScroll->GetViewer();
    if(!pView) return;
    pView->SetZoomFactor(pView->GetZoomFactor() + 0.1);
}

void CViewTable::slotZoomOut()
{
    CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(GetViewer(m_pTab->currentIndex()));
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
        CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(p);
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
    CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(pView);
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

QSize CViewTable::GetDesktopSize()
{
    QSize s;
    CViewFrmScroll* pScroll = qobject_cast<CViewFrmScroll*>(GetViewer(m_pTab->currentIndex()));
    if(!pScroll) return s;
    CFrmViewer* pView = pScroll->GetViewer();
    if(!pView) return s;
    s = pView->GetDesktopSize();
//    int marginW = pView->style()->pixelMetric(
//                QStyle::PM_FocusFrameHMargin) << 1;
//    int marginH = pView->style()->pixelMetric(
//                QStyle::PM_FocusFrameVMargin) << 1;
//    QMargins cm = pView->contentsMargins();
//    s.setWidth(marginW + cm.left() + cm.right() + s.width());
//    s.setHeight(marginH + cm.top() + cm.bottom() + s.height());
//    pView->resize(s);
    pScroll->resize(pView->frameSize());
//    QStyleOptionTabWidgetFrame option;
//    s = m_pTab->style()->sizeFromContents(QStyle::CT_TabWidget, &option, s);
    m_pTab->resize(pScroll->frameSize());
    resize(m_pTab->frameSize());
    return frameSize();
}
