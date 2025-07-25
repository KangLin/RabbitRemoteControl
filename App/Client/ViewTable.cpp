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

CViewTable::CViewTable(CParameterApp *pPara, QWidget *parent)
    : CView(pPara, parent),
    m_pTab(nullptr)
{
    qDebug(log) << Q_FUNC_INFO << this;
    bool check = false;
    //setFocusPolicy(Qt::NoFocus);

    m_pTab = new QTabWidget(this);
    m_pTab->setTabsClosable(true);
    m_pTab->setUsesScrollButtons(true);
    m_pTab->setMovable(true);
    //m_pTab->setFocusPolicy(Qt::NoFocus);

    if(m_pParameterApp)
    {
        if(m_pParameterApp->GetTabElided())
            m_pTab->setElideMode(Qt::ElideLeft);
        m_pTab->setTabPosition(m_pParameterApp->GetTabPosition());
        check = connect(m_pParameterApp, SIGNAL(sigTabPositionChanged()),
                        this, SLOT(slotTabPositionChanged()));
        Q_ASSERT(check);
        check = connect(m_pParameterApp, &CParameterApp::sigTabElided,
                        this, [&](){
            if(m_pParameterApp->GetTabElided())
                m_pTab->setElideMode(Qt::ElideLeft);
            else
                m_pTab->setElideMode(Qt::ElideNone);
        });
    }

    check = connect(m_pTab, SIGNAL(tabCloseRequested(int)),
                    this, SLOT(slotTabCloseRequested(int)));
    Q_ASSERT(check);
    check = connect(m_pTab, SIGNAL(currentChanged(int)),
                    this, SLOT(slotCurrentChanged(int)));
    Q_ASSERT(check);

    m_pTab->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(m_pTab->tabBar(), &QTabBar::customContextMenuRequested,
                    this, [&](const QPoint& pos){
                        QPoint p = pos;
                        p = m_pTab->tabBar()->mapToGlobal(pos);
                        emit customContextMenuRequested(p);
                    });

    Q_ASSERT(check);
}

CViewTable::~CViewTable()
{
    qDebug(log) << Q_FUNC_INFO << this;
    if(m_pTab) {
        m_pTab->clear();
        delete m_pTab;
    }
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
    if(!m_pParameterApp)
        return;
    m_pTab->setTabPosition(m_pParameterApp->GetTabPosition());
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
    nIndex = m_pTab->indexOf(pView);
    if(-1 == nIndex) {
        nIndex = m_pTab->addTab(pView, pView->windowTitle());
    }
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
    if(m_pParameterApp->GetEnableTabToolTip())
        m_pTab->setTabToolTip(nIndex, szToolTip);
    else
        m_pTab->setTabToolTip(nIndex, "");
    if(m_pParameterApp->GetEnableTabIcon())
        m_pTab->setTabIcon(nIndex, icon);
    else
        m_pTab->setTabIcon(nIndex, QIcon());
}

int CViewTable::SetFullScreen(bool bFull)
{
    if(!m_pTab) return -1;
    if(bFull)
    {
        m_szStyleSheet = m_pTab->styleSheet();
        SetVisibleTab(false);
        //qDebug(log) << m_szStyleSheet;
        m_pTab->setStyleSheet("QTabWidget::pane{top:0px;left:0px;border:none;}");
        m_pTab->showFullScreen();
    } else {
        SetVisibleTab(m_pParameterApp->GetTabBar());
        m_pTab->setStyleSheet(m_szStyleSheet);
        m_pTab->showNormal();
    }
    return 0;
}

int CViewTable::SetVisibleTab(bool bVisible)
{
    m_pTab->tabBar()->setVisible(bVisible);
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

int CViewTable::SetCurrentView(QWidget* pView)
{
    int nIndex = m_pTab->indexOf(pView);
    if(-1 != nIndex) {
        m_pTab->setCurrentIndex(nIndex);
        return 0;
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
