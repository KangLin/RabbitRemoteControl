// Author: Kang Lin <kl222@126.com>

#include <QResizeEvent>
#include <QTabBar>
#include <QStyleOption>
#include <QDebug>
#include <QScrollBar>
#include <QFileInfo>
#include <QDir>
#include <QLoggingCategory>
#include "mainwindow.h"
#include "ViewTable.h"

static Q_LOGGING_CATEGORY(log, "App.View.Table")
static Q_LOGGING_CATEGORY(logRecord, "App.View.Table.Record")

CViewTable::CViewTable(QWidget *parent) : CView(parent),
    m_pTab(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    bool check = false;
    setFocusPolicy(Qt::NoFocus);

    m_pTab = new QTabWidget(this);
    m_pTab->setTabsClosable(true);
    m_pTab->setUsesScrollButtons(true);
    m_pTab->setMovable(true);
    m_pTab->setFocusPolicy(Qt::NoFocus);
    m_pTab->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
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
    check = connect(m_pTab->tabBar(),
                    SIGNAL(customContextMenuRequested(const QPoint&)),
                    this, SIGNAL(customContextMenuRequested(const QPoint&)));
    Q_ASSERT(check);
}

CViewTable::~CViewTable()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pTab)
        delete m_pTab;
}

void CViewTable::slotCurrentChanged(int index)
{
    qDebug(log) << "CViewTable::slotCurrentChanged";
    emit sigCurrentChanged(GetViewer(index));
}

void CViewTable::slotTabCloseRequested(int index)
{
    QWidget* pView = GetViewer(index);
    if(!pView) return;
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
    if(!pView)
    {
        qCritical(log) << "CViewTable::AddView: The pView is nullptr";
        return -1;
    }
    //qDebug(log) << "CViewTable::AddView: Window title:" << pView->windowTitle();
    nIndex = m_pTab->addTab(pView, pView->windowTitle());
    m_pTab->setCurrentIndex(nIndex);

    return 0;
}

int CViewTable::RemoveView(QWidget *pView)
{
    int nIndex = GetViewIndex(pView);
    if(-1 == nIndex) return 0;
    // Note: The following order cannot be changed
    m_pTab->removeTab(nIndex);
    return 0;
}

void CViewTable::SetWidowsTitle(QWidget* pView, const QString& szTitle,
                                const QIcon &icon, const QString &szToolTip)
{
    if(!pView) {
        qCritical(log) << "CViewTable::SetWidowsTitle: The pView is nullptr";
        return;
    }
    //qDebug(log) << "CViewTable::SetWidowsTitle: Window title:" << szTitle;
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
        m_szStyleSheet = m_pTab->styleSheet();
        //qDebug(log) << m_szStyleSheet;
        m_pTab->setStyleSheet("QTabWidget::pane{top:0px;left:0px;border:none;}");
        m_pTab->showFullScreen();
    } else {
        m_pTab->setStyleSheet(m_szStyleSheet);
        m_pTab->showNormal();
    }
    return 0;
}

int CViewTable::ShowTabBar(bool bShow)
{
    m_pTab->tabBar()->setVisible(bShow);
    return 0;
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
    }
    return -1;
}

// \note The return QWidget* must is same as CConnecter::GetViewer()
QWidget* CViewTable::GetCurrentView()
{
    QWidget* pView = m_pTab->currentWidget();
    if(!pView) return pView;
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
    return frameSize();
}
