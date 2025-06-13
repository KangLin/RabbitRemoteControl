#include <QVBoxLayout>
#include <QLoggingCategory>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include "FrmActive.h"

static Q_LOGGING_CATEGORY(log, "App.FrmActive")

CFrmActive::CFrmActive(QVector<COperate*> &operates,
                               CParameterApp &parameterApp,
                               QMenu* pOperate,
                               QAction* pStop,
                               RabbitCommon::CRecentMenu *pRecentMenu,
                               QWidget *parent)
    : QWidget(parent)
    , m_pDockTitleBar(nullptr)
    , m_pOperate(pOperate)
    , m_pStop(pStop)
    , m_pRecentMenu(pRecentMenu)
    , m_Operates(operates)
    , m_ParameterApp(parameterApp)
    , m_pTableView(nullptr)
    , m_pModel(nullptr)
    , m_pMenu(nullptr)
    , m_pToolBar(nullptr)
    , m_nId(3)
{
    bool check = false;
    setFocusPolicy(Qt::NoFocus);
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("List active"));

    QVBoxLayout* pLayout = new QVBoxLayout(this);
    if(!pLayout) {
        qCritical(log) << "new QVBoxLayout(this) fail";
        return;
    }
    setLayout(pLayout);

    m_pToolBar = new QToolBar(this);
    m_pToolBar->setVisible(m_ParameterApp.GetDockListActiveShowToolBar());
    // Create recent menu
    auto ptbRecent = new QToolButton(m_pToolBar);
    ptbRecent->setFocusPolicy(Qt::NoFocus);
    ptbRecent->setPopupMode(QToolButton::InstantPopup);
    //ptbRecent->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ptbRecent->setMenu(m_pRecentMenu);
    ptbRecent->setIcon(m_pRecentMenu->icon());
    ptbRecent->setText(m_pRecentMenu->title());
    ptbRecent->setToolTip(m_pRecentMenu->title());
    ptbRecent->setStatusTip(m_pRecentMenu->title());
    m_pToolBar->addWidget(ptbRecent);
    // Create start menu
    auto ptbStart = new QToolButton(m_pToolBar);
    ptbStart->setFocusPolicy(Qt::NoFocus);
    ptbStart->setPopupMode(QToolButton::InstantPopup);
    //ptbStart->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ptbStart->setMenu(m_pOperate);
    ptbStart->setIcon(m_pOperate->icon());
    ptbStart->setText(m_pOperate->title());
    ptbStart->setToolTip(m_pOperate->title());
    ptbStart->setStatusTip(m_pOperate->title());
    m_pToolBar->addWidget(ptbStart);

    m_pToolBar->addAction(m_pStop);
    layout()->addWidget(m_pToolBar);

    m_pDockTitleBar = new RabbitCommon::CTitleBar(parent);
    // Create tools pushbutton in title bar
    m_pMenu = new QMenu(tr("Tools"), m_pDockTitleBar);
    m_pMenu->addMenu(m_pRecentMenu);
    m_pMenu->addMenu(m_pOperate);
    m_pMenu->addAction(m_pStop);
    QPushButton* pTools = m_pDockTitleBar->CreateSmallPushButton(
        QIcon::fromTheme("tools"), m_pDockTitleBar);
    pTools->setToolTip(tr("Tools"));
    pTools->setMenu(m_pMenu);
    m_pMenu->addAction(m_pStop);
    m_pMenu->addSeparator();
    auto pShowToolBar = m_pMenu->addAction(tr("Show tool bar"), this, [&](){
        QAction* a = (QAction*)sender();
        if(a) {
            m_pToolBar->setVisible(a->isChecked());
            m_ParameterApp.SetDockListActiveShowToolBar(a->isChecked());
            m_ParameterApp.Save();
        }
    });
    pShowToolBar->setCheckable(true);
    pShowToolBar->setChecked(m_ParameterApp.GetDockListActiveShowToolBar());

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
    m_pModel->setHorizontalHeaderItem(2, new QStandardItem(tr("Type")));
    m_pModel->setHorizontalHeaderItem(m_nId, new QStandardItem(tr("ID")));

    slotLoad();

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
        QHeaderView::Interactive);
    //以下设置列宽函数必须要数据加载完成后使用,才能应用
    //See: https://blog.csdn.net/qq_40450386/article/details/86083759
    //m_pTableView->resizeColumnsToContents(); //设置所有列宽度自适应内容
    //m_pTableView->resizeColumnToContents(0); //设置第0列宽度自适应内容
    //m_pTableView->resizeColumnToContents(1); //设置第0列宽度自适应内容
    //m_pTableView->resizeColumnToContents(2); //设置第0列宽度自适应内容
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

CFrmActive::~CFrmActive()
{}

void CFrmActive::slotCustomContextMenu(const QPoint &pos)
{
    QMenu menu;
    int r = m_pTableView->currentIndex().row();
    if(-1 < r && r < m_Operates.size())
    {
        auto c = m_Operates[r];
        if(c) {
            auto m = c->GetMenu();
            //menu.addMenu(m);
            if(m)
                menu.addActions(m->actions());
        }
    }
    if(!menu.isEmpty())
        menu.addSeparator();
    menu.addMenu(m_pRecentMenu);
    menu.addMenu(m_pOperate);
    menu.addAction(m_pStop);
    menu.exec(mapToGlobal(pos));
}

void CFrmActive::slotClicked(const QModelIndex &index)
{
    QVariant v = m_pModel->item(index.row(), m_nId)->data();
    COperate* c = v.value<COperate*>();
    emit sigChanged(c);
}

void CFrmActive::slotLoad()
{
    if(!m_pModel)
        return;
    m_pModel->removeRows(0, m_pModel->rowCount());
    foreach(auto c, m_Operates) {
        QList<QStandardItem*> lstItem;
        QStandardItem* pName = new QStandardItem(c->Icon(), c->Name());
        pName->setToolTip(c->Description());
        lstItem << pName;
        QStandardItem* pProtocol = new QStandardItem(c->Protocol());
        lstItem << pProtocol;
        QStandardItem* pType = new QStandardItem(c->GetTypeName());
        lstItem << pType;
        QStandardItem* pId = new QStandardItem(c->Id());
        QVariant v;
        v.setValue(c);
        pId->setData(v);
        lstItem << pId;
        m_pModel->appendRow(lstItem);
    }

    //以下设置列宽函数必须要数据加载完成后使用,才能应用
    //See: https://blog.csdn.net/qq_40450386/article/details/86083759
    //m_pTableView->resizeColumnsToContents(); //设置所有列宽度自适应内容
    m_pTableView->resizeColumnToContents(0); //设置第0列宽度自适应内容
    //m_pTableView->resizeColumnToContents(m_nId); //设置第1列宽度自适应内容
}

void CFrmActive::slotViewChanged(const QWidget *pView)
{
    int nIndex = -1;
    if(m_Operates.size() != m_pModel->rowCount())
        slotLoad();
    foreach (auto c, m_Operates) {
        nIndex++;
        if(c->GetViewer() == pView)
            break;
    }
    if(-1 >= nIndex)
        return;
    m_pTableView->selectRow(nIndex);
}
