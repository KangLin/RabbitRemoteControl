// Author: Kang Lin <kl222@126.com>

#include "ViewSplitterContainer.h"
#include <QLoggingCategory>
#include <QVBoxLayout>
#include <QEvent>

static Q_LOGGING_CATEGORY(log, "App.View.Splitter.Container")
CViewSplitterContainer::CViewSplitterContainer(QWidget *pView, CParameterApp *pPara)
    : QFrame{}
    , m_pTab(nullptr)
    , m_pView(pView)
    , m_pParameterApp(pPara)
{
    Q_ASSERT(m_pView && m_pParameterApp);
    m_pTab = new RabbitCommon::CTitleBar(this);
    if(!m_pTab)
        return;
    m_pTab->VisibleFloatButton(false);
    m_pTab->VisibleMaximizeButton(false);
    m_pTab->VisibleMinimizeButton(false);
    m_pTab->installEventFilter(this);

    SetVisibleTab(true);
    m_pTab->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    if(m_pView) {
        m_pView->show();
        m_pTab->setWindowTitle(m_pView->windowTitle());
    }

    m_pTab->setContextMenuPolicy(Qt::CustomContextMenu);
    bool check = connect(m_pTab, SIGNAL(customContextMenuRequested(QPoint)),
                         this, SLOT(slotCustomContextMenuRequested(QPoint)));
    Q_ASSERT(check);

    auto pLayout = new QVBoxLayout(this);
    if(pLayout)
        setLayout(pLayout);
    Q_ASSERT(pLayout);
    pLayout->setContentsMargins(0, 0, 0, 0);
    slotTabPositionChanged();
}

CViewSplitterContainer::~CViewSplitterContainer()
{
    qDebug(log) << Q_FUNC_INFO;
    if(layout())
        layout()->removeWidget(m_pView);
}

QWidget* CViewSplitterContainer::GetView()
{
    return m_pView;
}

void CViewSplitterContainer::SetPrompt(const QString &szPrompt)
{
    m_pTab->setToolTip(szPrompt);
}

int CViewSplitterContainer::SetVisibleTab(bool bVisible)
{
    m_pTab->setVisible(bVisible);
    return 0;
}

bool CViewSplitterContainer::GetVisibleTab()
{
    return m_pTab->isVisible();
}

void CViewSplitterContainer::slotTabPositionChanged()
{
    if(!m_pParameterApp || !m_pView)
        return;

    qDebug(log) << Q_FUNC_INFO << m_pParameterApp->GetTabPosition();
    switch(m_pParameterApp->GetTabPosition())
    {
    case QTabWidget::TabPosition::North:
    case QTabWidget::TabPosition::West:
    {
        auto pLayout = layout();
        pLayout->addWidget(m_pTab);
        pLayout->addWidget(m_pView);
        break;
    }
    case QTabWidget::TabPosition::South:
    case QTabWidget::TabPosition::East:
    {
        auto pLayout = layout();
        pLayout->addWidget(m_pView);
        pLayout->addWidget(m_pTab);
        break;
    }
    }
    m_pView->show();
}

void CViewSplitterContainer::closeEvent(QCloseEvent *event)
{
    qDebug(log) << Q_FUNC_INFO;
    emit sigCloseView(m_pView);
}

void CViewSplitterContainer::slotCustomContextMenuRequested(const QPoint &pos)
{
    QPoint p = pos;
    p = m_pTab->mapToGlobal(pos);
    emit customContextMenuRequested(p);
}

bool CViewSplitterContainer::eventFilter(QObject *watched, QEvent *event)
{
    //qDebug(log) << "Event filter:" << watched << event->type();
    if(watched == m_pTab && event->type() == QEvent::MouseButtonPress)
    {
        emit sigFouceIn(m_pView);
    }
    return false;
}
