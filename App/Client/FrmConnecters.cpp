#include <QVBoxLayout>
#include <QLoggingCategory>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include "FrmConnecters.h"

static Q_LOGGING_CATEGORY(log, "App.FrmConnecters")

CFrmConnecters::CFrmConnecters(QVector<CConnecter*> &Connecters,
                               CParameterApp &parameterApp,
                               QMenu* pConnect,
                               QAction* pDisconnect,
                               RabbitCommon::CRecentMenu *pRecentMenu,
                               QWidget *parent)
    : QWidget(parent)
    , m_pDockTitleBar(nullptr)
    , m_pConnect(pConnect)
    , m_pDisconnect(pDisconnect)
    , m_pRecentMenu(pRecentMenu)
    , m_Connecters(Connecters)
    , m_ParameterApp(parameterApp)
    , m_pTableView(nullptr)
    , m_pModel(nullptr)
    , m_pMenu(nullptr)
    , m_pToolBar(nullptr)
    , m_nId(2)
{
    bool check = false;
    setFocusPolicy(Qt::NoFocus);
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("List connections"));

    QVBoxLayout* pLayout = new QVBoxLayout(this);
    if(!pLayout) {
        qCritical(log) << "new QVBoxLayout(this) fail";
        return;
    }
    setLayout(pLayout);

    m_pToolBar = new QToolBar(this);
    m_pToolBar->setVisible(m_ParameterApp.GetDockListConnectersShowToolBar());
    // Create recent menu
    auto ptbRecent = new QToolButton(m_pToolBar);
    ptbRecent->setFocusPolicy(Qt::NoFocus);
    ptbRecent->setPopupMode(QToolButton::InstantPopup);
    //m_ptbConnect->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ptbRecent->setMenu(m_pRecentMenu);
    ptbRecent->setIcon(m_pRecentMenu->icon());
    ptbRecent->setText(m_pRecentMenu->title());
    ptbRecent->setToolTip(m_pRecentMenu->title());
    ptbRecent->setStatusTip(m_pRecentMenu->title());
    m_pToolBar->addWidget(ptbRecent);
    // Create connect menu
    auto ptbConnect = new QToolButton(m_pToolBar);
    ptbConnect->setFocusPolicy(Qt::NoFocus);
    ptbConnect->setPopupMode(QToolButton::InstantPopup);
    //m_ptbConnect->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ptbConnect->setMenu(m_pConnect);
    ptbConnect->setIcon(m_pConnect->icon());
    ptbConnect->setText(m_pConnect->title());
    ptbConnect->setToolTip(m_pConnect->title());
    ptbConnect->setStatusTip(m_pConnect->title());
    m_pToolBar->addWidget(ptbConnect);

    m_pToolBar->addAction(m_pDisconnect);
    layout()->addWidget(m_pToolBar);

    m_pDockTitleBar = new RabbitCommon::CTitleBar(parent);
    // Create tools pushbutton in title bar
    m_pMenu = new QMenu(tr("Tools"), m_pDockTitleBar);
    m_pMenu->addMenu(m_pRecentMenu);
    m_pMenu->addMenu(m_pConnect);
    m_pMenu->addAction(m_pDisconnect);
    QPushButton* pTools = m_pDockTitleBar->CreateSmallPushButton(
        QIcon::fromTheme("tools"), m_pDockTitleBar);
    pTools->setToolTip(tr("Tools"));
    pTools->setMenu(m_pMenu);
    m_pMenu->addAction(m_pDisconnect);
    m_pMenu->addSeparator();
    auto pShowToolBar = m_pMenu->addAction(tr("Show tool bar"), this, [&](){
        QAction* a = (QAction*)sender();
        if(a) {
            m_pToolBar->setVisible(a->isChecked());
            m_ParameterApp.SetDockListConnectersShowToolBar(a->isChecked());
            m_ParameterApp.Save();
        }
    });
    pShowToolBar->setCheckable(true);
    pShowToolBar->setChecked(m_ParameterApp.GetDockListConnectersShowToolBar());

    QList<QWidget*> lstWidget;
    lstWidget << pTools;
    m_pDockTitleBar->AddWidgets(lstWidget);

    m_pTableView = new QTableView(this);
    if(!m_pTableView) {
        qCritical(log) << "new QTableView fail";
        return;
    }
    layout()->addWidget(m_pTableView);
    m_pTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(m_pTableView,
                    SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(slotCustomContextMenu(const QPoint &)));
    Q_ASSERT(check);
    check = connect(m_pTableView, SIGNAL(clicked(const QModelIndex &)),
                    this, SLOT(slotClicked(const QModelIndex&)));
    Q_ASSERT(check);

    m_pModel = new QStandardItemModel(m_pTableView);
    m_pTableView->setModel(m_pModel);
    m_pTableView->verticalHeader()->hide();
    m_pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pModel->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
    m_pModel->setHorizontalHeaderItem(1, new QStandardItem(tr("Protocol")));
    m_pModel->setHorizontalHeaderItem(m_nId, new QStandardItem(tr("ID")));

    slotLoadConnecters();

    //必须在 setModel 后,才能应用
    /*第二个参数可以为：
    QHeaderView::Interactive     ：0 用户可设置，也可被程序设置成默认大小
    QHeaderView::Fixed           ：2 用户不可更改列宽
    QHeaderView::Stretch         ：1 根据空间，自动改变列宽，用户与程序不能改变列宽
    QHeaderView::ResizeToContents：3 根据内容改变列宽，用户与程序不能改变列宽
    */
    m_pTableView->horizontalHeader()->setSectionResizeMode(
#if defined(DEBUG) && !defined(Q_OS_ANDROID)
        0,
#endif
        QHeaderView::ResizeToContents);
    //以下设置列宽函数必须要数据加载完成后使用,才能应用
    //See: https://blog.csdn.net/qq_40450386/article/details/86083759
    //m_pTableView->resizeColumnsToContents(); //设置所有列宽度自适应内容
    //m_pTableView->resizeColumnToContents(0); //设置第0列宽度自适应内容
    //m_pTableView->resizeColumnToContents(m_nId); //设置第1列宽度自适应内容

    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex;
    if(pSelect)
        lstIndex = pSelect->selectedRows();
    if(m_pModel->rowCount() > 0 && lstIndex.isEmpty())
    {
        m_pTableView->selectRow(0);
    }
}

CFrmConnecters::~CFrmConnecters()
{}

void CFrmConnecters::slotCustomContextMenu(const QPoint &pos)
{
    QMenu menu;
    menu.addMenu(m_pRecentMenu);
    menu.addMenu(m_pConnect);
    menu.addAction(m_pDisconnect);
    menu.addSeparator();
    int r = m_pTableView->currentIndex().row();
    if(-1 < r && r < m_Connecters.size())
    {
        auto c = m_Connecters[r];
        if(c) {
            auto m = c->GetMenu();
            //menu.addMenu(m);
            if(m)
                menu.addActions(m->actions());
        }
    }
    menu.exec(mapToGlobal(pos));
}

void CFrmConnecters::slotClicked(const QModelIndex &index)
{
    QVariant v = m_pModel->item(index.row(), m_nId)->data();
    CConnecter* c = v.value<CConnecter*>();
    emit sigConnecterChanged(c);
}

void CFrmConnecters::slotLoadConnecters()
{
    if(!m_pModel)
        return;
    m_pModel->removeRows(0, m_pModel->rowCount());
    foreach(auto c, m_Connecters) {
        QList<QStandardItem*> lstItem;
        QStandardItem* pName = new QStandardItem(c->Icon(), c->Name());
        pName->setToolTip(c->Description());
        lstItem << pName;
        QStandardItem* pProtocol = new QStandardItem(c->Protocol());
        lstItem << pProtocol;
        QStandardItem* pId = new QStandardItem(c->Id());
        QVariant v;
        v.setValue(c);
        pId->setData(v);
        lstItem << pId;
        m_pModel->appendRow(lstItem);
    }
}

void CFrmConnecters::slotViewChanged(const QWidget *pView)
{
    int nIndex = -1;
    if(m_Connecters.size() != m_pModel->rowCount())
        slotLoadConnecters();
    foreach (auto c, m_Connecters) {
        nIndex++;
        if(c->GetViewer() == pView)
            break;
    }
    if(-1 >= nIndex)
        return;
    m_pTableView->selectRow(nIndex);
}
