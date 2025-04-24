// Author: Kang Lin <kl222@126.com>

#include "ViewSplitterContainer.h"
#include <QLoggingCategory>
#include <QVBoxLayout>

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

    SetVisibleTab(true);
    m_pTab->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    if(m_pView) {
        m_pView->show();
        m_pTab->setWindowTitle(m_pView->windowTitle());
    }

    m_pTab->setContextMenuPolicy(Qt::CustomContextMenu);
    bool check = connect(m_pTab,
                    SIGNAL(customContextMenuRequested(const QPoint&)),
                    this, SIGNAL(customContextMenuRequested(const QPoint&)));
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

QSize CViewSplitterContainer::minimumSizeHint() const
{
    int w = 0, h = 0;
    if(m_pTab) {
        w = m_pTab->minimumSizeHint().width();
        h = m_pTab->minimumSizeHint().height();
    }

    if(m_pView) {
        w = qMax(w, m_pView->minimumSizeHint().width());
        h = h + m_pView->minimumSizeHint().height();
    }
    //qDebug(log) << "Width:" << w << "Height:" << h;
    return QSize(w, h);
}
