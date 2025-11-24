#include "FrmListRecent.h"
#include "RabbitCommonDir.h"
#include <QVBoxLayout>
#include <QDateTime>
#include <QStandardItem>
#include <QDir>
#include <QHeaderView>
#include <QMenu>
#include <QLoggingCategory>
#include "mainwindow.h"

static Q_LOGGING_CATEGORY(log, "App.FrmListRecent")

CFrmListRecent::CFrmListRecent(
    MainWindow *pMainWindow, CManager *pManager,
    CParameterApp &parameterApp, bool bDock, QWidget *parent) :
    QWidget(parent),
    m_pMainWindow(pMainWindow),
    m_ParameterApp(parameterApp),
    m_pToolBar(nullptr),
    m_ptbOperate(nullptr),
    m_pMenuNew(nullptr),
    m_pModel(nullptr),
    m_pManager(pManager),
    m_bDock(bDock),
    m_pDockTitleBar(nullptr)
{
    bool check = false;
    //setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_DeleteOnClose);
    setLayout(new QVBoxLayout(this));
    setWindowTitle(tr("Recently list"));

    m_pToolBar = new QToolBar(this);

    m_pStart = m_pToolBar->addAction(
        QIcon::fromTheme("media-playback-start"), tr("Start"),
        this, SLOT(slotStart()));
    m_pStart->setStatusTip(tr("Start"));
    m_pStart->setToolTip(tr("Start"));
    m_pEditOperate = m_pToolBar->addAction(
        QIcon::fromTheme("edit-connect"), tr("Edit and Start"),
        this, SLOT(slotEditConnect()));
    m_pEditOperate->setStatusTip(tr("Edit and Start"));
    m_pEditOperate->setToolTip(tr("Edit and Start"));
    m_pToolBar->addSeparator();

    m_ptbOperate = new QToolButton(m_pToolBar);
    m_ptbOperate->setFocusPolicy(Qt::NoFocus);
    m_ptbOperate->setPopupMode(QToolButton::InstantPopup);
    //m_ptbOperate->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_pMenuNew = new QMenu(tr("New"), this);
    m_pMenuNew->setIcon(QIcon::fromTheme("add"));
    m_ptbOperate->setMenu(m_pMenuNew);
    m_ptbOperate->setIcon(m_pMenuNew->icon());
    m_ptbOperate->setText(tr("New"));
    m_ptbOperate->setToolTip(tr("New"));
    m_ptbOperate->setStatusTip(tr("New"));
    m_pToolBar->addWidget(m_ptbOperate);
    check = connect(&m_ParameterApp, SIGNAL(sigStartByTypeChanged()),
                    this, SLOT(slotStartByType()));
    Q_ASSERT(check);
    m_pManager->EnumPlugins(this);
    m_pEdit = m_pToolBar->addAction(QIcon::fromTheme("edit"), tr("Edit"),
                                    this, SLOT(slotEdit()));
    m_pEdit->setStatusTip(tr("Edit"));
    m_pEdit->setToolTip(tr("Edit"));
    m_pCopy = m_pToolBar->addAction(QIcon::fromTheme("edit-copy"), tr("Copy"),
                                    this, SLOT(slotCopy()));
    m_pCopy->setStatusTip(tr("Copy"));
    m_pCopy->setToolTip(tr("Copy"));
    m_pDelete = m_pToolBar->addAction(
        QIcon::fromTheme("edit-delete"), tr("Delete"),
        this, SLOT(slotDelete()));
    m_pDelete->setToolTip(tr("Delete"));
    m_pDelete->setStatusTip(tr("Delete"));
    m_pDetail = m_pToolBar->addAction(
        QIcon::fromTheme("dialog-information"), tr("Detail"),
        this, SLOT(slotDetail()));
    m_pDetail->setCheckable(true);
    m_pDetail->setChecked(false);
    m_pDetail->setToolTip(tr("Detail"));
    m_pDetail->setStatusTip(tr("Detail"));
    m_pToolBar->addSeparator();
    m_pAddToFavorite = m_pToolBar->addAction(QIcon::fromTheme("emblem-favorite"), tr("Add to favorite"),
                          this, SLOT(slotAddToFavorite()));
    m_pAddToFavorite->setStatusTip(m_pAddToFavorite->text());
    m_pAddToFavorite->setToolTip(m_pAddToFavorite->text());
    m_pAddToFavorite->setEnabled(false);
    if(m_bDock) {
        m_pRefresh = m_pToolBar->addAction(
            QIcon::fromTheme("view-refresh"),
            tr("Refresh"),
            this, SLOT(slotLoadFiles()));
        m_pRefresh->setToolTip(tr("Refresh"));
        m_pRefresh->setStatusTip(tr("Refresh"));
    } else {
        QAction* pClose = m_pToolBar->addAction(
            QIcon::fromTheme("window-close"), tr("Close"),
            this, SLOT(close()));
        pClose->setStatusTip(tr("Close"));
        pClose->setToolTip(tr("Close"));
    }

    layout()->addWidget(m_pToolBar);

    if(bDock) {
        m_pDockTitleBar = new RabbitCommon::CTitleBar(parent);
        // Create tools pushbutton in title bar
        QMenu* pMenu = new QMenu(tr("Tools"), m_pDockTitleBar);
        QPushButton* pTools = m_pDockTitleBar->CreateSmallPushButton(
            QIcon::fromTheme("tools"), m_pDockTitleBar);
        pTools->setToolTip(tr("Tools"));
        pTools->setMenu(pMenu);
        QList<QWidget*> lstWidget;
        lstWidget << pTools;
        m_pDockTitleBar->AddWidgets(lstWidget);
        
        pMenu->addAction(m_pStart);
        pMenu->addAction(m_pEditOperate);
        pMenu->addMenu(m_pMenuNew);
        pMenu->addAction(m_pEdit);
        pMenu->addAction(m_pCopy);
        pMenu->addAction(m_pDelete);
        pMenu->addAction(m_pDetail);
        pMenu->addAction(m_pAddToFavorite);
        pMenu->addAction(m_pRefresh);
        pMenu->addSeparator();
        auto pShowToolBar = pMenu->addAction(tr("Show tool bar"), this, [&](){
            QAction* a = (QAction*)sender();
            if(a) {
                m_pToolBar->setVisible(a->isChecked());
                m_ParameterApp.SetDockListRecentShowToolBar(a->isChecked());
                m_ParameterApp.Save();
            }
        });
        pShowToolBar->setCheckable(true);
        pShowToolBar->setChecked(m_ParameterApp.GetDockListRecentShowToolBar());
        m_pToolBar->setVisible(m_ParameterApp.GetDockListRecentShowToolBar());
    }

    Q_ASSERT(m_pManager);
    m_pTableView = new QTableView(this);
    m_pTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(m_pTableView,
                    SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(slotCustomContextMenu(const QPoint &)));
    Q_ASSERT(check);
    check = connect(m_pTableView, SIGNAL(doubleClicked(const QModelIndex &)),
                    this, SLOT(slotDoubleClicked(const QModelIndex&)));
    Q_ASSERT(check);
    layout()->addWidget(m_pTableView);

    m_pModel = new QStandardItemModel(m_pTableView);
    m_pTableView->setModel(m_pModel);
    m_pTableView->verticalHeader()->hide();
    m_pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pModel->setHorizontalHeaderItem(ColumnNo::Name, new QStandardItem(tr("Name")));
    m_pModel->setHorizontalHeaderItem(ColumnNo::Protocol, new QStandardItem(tr("Protocol")));
    m_pModel->setHorizontalHeaderItem(ColumnNo::Type, new QStandardItem(tr("Type")));
    m_pModel->setHorizontalHeaderItem(ColumnNo::Date, new QStandardItem(tr("Date")));
    m_pModel->setHorizontalHeaderItem(ColumnNo::ID, new QStandardItem(tr("ID")));
    m_pModel->setHorizontalHeaderItem(ColumnNo::File, new QStandardItem(tr("File")));
    if(!m_pDetail->isChecked()) {
        m_pTableView->hideColumn(ColumnNo::ID);
        m_pTableView->hideColumn(ColumnNo::File);
    }
    
    slotLoadFiles();

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
    //m_pTableView->resizeColumnToContents(2); //设置第1列宽度自适应内容
    //m_pTableView->resizeColumnToContents(3); //设置第1列宽度自适应内容

    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex;
    if(pSelect)
        lstIndex = pSelect->selectedRows();
    if(m_pModel->rowCount() > 0 && lstIndex.isEmpty())
    {
        m_pTableView->selectRow(0);
    }

}

CFrmListRecent::~CFrmListRecent()
{
}

void CFrmListRecent::slotLoadFiles()
{
    m_pModel->removeRows(0, m_pModel->rowCount());
    QString szPath = RabbitCommon::CDir::Instance()->GetDirUserData();
    QDir dir(szPath);
    QStringList files = dir.entryList(QStringList() << "*.rrc",
                                      QDir::Files, QDir::Time);
    foreach (auto fileName, files) {
        QString szFile = dir.absoluteFilePath(fileName);
        if(szFile.isEmpty()) continue;
        COperate* pOperate = m_pManager->LoadOperate(szFile);
        if(!pOperate) continue;

        QList<QStandardItem*> lstItem;
        lstItem = GetItem(pOperate, szFile);
        m_pModel->appendRow(lstItem);

        m_pManager->DeleteOperate(pOperate);
    }

    m_pStart->setEnabled(m_pModel->rowCount() > 0);
    m_pEditOperate->setEnabled(m_pModel->rowCount() > 0);
    m_pEdit->setEnabled(m_pModel->rowCount() > 0);
    m_pCopy->setEnabled(m_pModel->rowCount() > 0);
    m_pDelete->setEnabled(m_pModel->rowCount() > 0);
    m_pAddToFavorite->setEnabled(m_pModel->rowCount() > 0);

    //以下设置列宽函数必须要数据加载完成后使用,才能应用
    //See: https://blog.csdn.net/qq_40450386/article/details/86083759
    //m_pTableView->resizeColumnsToContents(); //设置所有列宽度自适应内容
    m_pTableView->resizeColumnToContents(ColumnNo::Name); //设置第0列宽度自适应内容
    m_pTableView->resizeColumnToContents(ColumnNo::Protocol); //设置第1列宽度自适应内容
    m_pTableView->resizeColumnToContents(ColumnNo::Type); //设置第1列宽度自适应内容
    m_pTableView->resizeColumnToContents(ColumnNo::Date); //设置第1列宽度自适应内容
    return;
}

QList<QStandardItem*> CFrmListRecent::GetItem(COperate* c, QString &szFile)
{
    QList<QStandardItem*> lstItem;
    QStandardItem* pName = new QStandardItem(c->Icon(), c->Name());
    pName->setToolTip(c->Description());
    lstItem << pName;
    QStandardItem* pProtocol = new QStandardItem(c->Protocol());
    lstItem << pProtocol;
    QStandardItem* pType = new QStandardItem(c->GetTypeName());
    lstItem << pType;
    //Date
    QFileInfo fi(szFile);
    lstItem << new QStandardItem(fi.lastModified().toString());
    QStandardItem* pId = new QStandardItem(c->Id());
    lstItem << pId;
    QStandardItem* pFile = new QStandardItem(szFile);
    lstItem << pFile;
    return lstItem;
}

int CFrmListRecent::InsertItem(COperate *c, QString& szFile)
{
    QList<QStandardItem*> lstItem;
    lstItem = GetItem(c, szFile);
    m_pModel->insertRow(0, lstItem);
    m_pTableView->selectRow(0);
    return 0;
}

void CFrmListRecent::slotStartByType()
{
    qDebug(log) << Q_FUNC_INFO;
    auto m = m_pMenuNew->actions();
    foreach(auto a, m) {
        a->deleteLater();
    }
    foreach (auto a, m_MenuStartByType) {
        a->deleteLater();
    }
    m_pMenuNew->clear();
    m_MenuStartByType.clear();
    m_pManager->EnumPlugins(this);
}

int CFrmListRecent::onProcess(const QString &id, CPlugin *pPlugin)
{
    // Connect menu and toolbar
    QMenu* m = m_pMenuNew;
    if(m_ParameterApp.GetStartByType()) {
        auto it = m_MenuStartByType.find(pPlugin->Type());
        if(it == m_MenuStartByType.end()) {
            m = new QMenu(pPlugin->TypeName(pPlugin->Type()), m_pMenuNew);
            m_MenuStartByType[pPlugin->Type()] = m;
            m_pMenuNew->addMenu(m);
        } else
            m = *it;
    }
    // Start menu and toolbar
    QAction* p = m_pMainWindow->GetStartAction(m, pPlugin);
    bool check = connect(p, SIGNAL(triggered()), this, SLOT(slotNew()));
    Q_ASSERT(check);
    return 0;
}

void CFrmListRecent::slotNew()
{
    QAction* pAction = dynamic_cast<QAction*>(this->sender());    
    COperate* pOperate = m_pManager->CreateOperate(pAction->data().toString());
    if(nullptr == pOperate) return;

    int nRet = pOperate->OpenDialogSettings(this);
    switch(nRet)
    {
    case QDialog::Rejected:
        break;
    case QDialog::Accepted:
    {
        QString szFile = pOperate->GetSettingsFile();
        QDir d;
        if(d.exists(szFile)) {
            QMessageBox::StandardButton r
                = QMessageBox::warning(
                    this, tr("Warning"),
                    tr("The file is exists. whether to overwrite it? File: %1").arg(szFile),
                    QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::No,
                    QMessageBox::StandardButton::No);
            if(QMessageBox::StandardButton::Ok == r)
            {
                d.remove(szFile);
                m_pManager->SaveOperate(pOperate);
            }
            break;
        }

        m_pManager->SaveOperate(pOperate);

        InsertItem(pOperate, szFile);

        break;
    }
    }

    m_pManager->DeleteOperate(pOperate);
}

void CFrmListRecent::slotEdit()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), ColumnNo::File)->text();
        COperate* pOperate = m_pManager->LoadOperate(szFile);
        int nRet = pOperate->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            break;
        case QDialog::Accepted:
            m_pManager->SaveOperate(pOperate);
            break;
        }
        m_pManager->DeleteOperate(pOperate);
    }
}

void CFrmListRecent::slotEditConnect()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), ColumnNo::File)->text();
        COperate* c = m_pManager->LoadOperate(szFile);
        int nRet = c->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            break;
        case QDialog::Accepted:
            m_pManager->SaveOperate(c);
            emit sigStart(szFile);
            break;
        }
        m_pManager->DeleteOperate(c);
    }
    if(!m_bDock)
        close();
}

void CFrmListRecent::slotCopy()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), ColumnNo::File)->text();
        COperate* pOperate = m_pManager->LoadOperate(szFile);

        do {
            bool bExit = true;
            bool bInsert = true;
            int nRet = pOperate->OpenDialogSettings(this);
            switch(nRet)
            {
            case QDialog::Rejected:
                break;
            case QDialog::Accepted:
            {
                szFile = pOperate->GetSettingsFile();
                QDir d(szFile);
                if(d.exists(szFile)) {
                    QMessageBox::StandardButton r
                        = QMessageBox::warning(
                            this,
                            tr("Warning"),
                            tr("The file is exists. whether to overwrite it?"
                               " If select No, please modify the name."),
                            QMessageBox::StandardButton::Ok
                                | QMessageBox::StandardButton::No
                                | QMessageBox::StandardButton::Cancel,
                            QMessageBox::StandardButton::No);
                    if(QMessageBox::StandardButton::No == r)
                    {
                        bExit = false;
                        break;
                    }
                    if(QMessageBox::StandardButton::Cancel == r)
                        break;
                    bInsert = false;
                }
                m_pManager->SaveOperate(pOperate);
                if(bInsert)
                    InsertItem(pOperate, szFile);
                break;
            }
            }

            if(bExit)
                break;
        } while(1);

        m_pManager->DeleteOperate(pOperate);
    }
}

void CFrmListRecent::slotDelete()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), ColumnNo::File)->text();
        QDir d(szFile);
        if(d.remove(szFile))
            m_pModel->removeRow(index.row());
    }
}

void CFrmListRecent::slotStart()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), ColumnNo::File)->text();
        emit sigStart(szFile);
    }
    if(!m_bDock) close();
}

void CFrmListRecent::slotDetail()
{
    if(m_pDetail->isChecked()) {
        m_pTableView->showColumn(ColumnNo::ID);
        m_pTableView->showColumn(ColumnNo::File);
    } else {
        m_pTableView->hideColumn(ColumnNo::ID);
        m_pTableView->hideColumn(ColumnNo::File);
    }
}

void CFrmListRecent::slotCustomContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    
    menu.addAction(m_pStart);
    menu.addAction(m_pStart);
    menu.addSeparator();
    menu.addMenu(m_pMenuNew);
    menu.addAction(m_pEdit);
    menu.addAction(m_pCopy);
    menu.addAction(m_pDelete);
    menu.addSeparator();
    menu.addAction(m_pAddToFavorite);
    if(m_bDock)
        menu.addAction(m_pRefresh);

    menu.exec(mapToGlobal(pos));
}

void CFrmListRecent::slotDoubleClicked(const QModelIndex& index)
{
    if(!index.isValid()) return;
    QString szFile = m_pModel->item(index.row(), ColumnNo::File)->text();
    emit sigStart(szFile);
    if(!m_bDock) close();
}

void CFrmListRecent::slotAddToFavorite()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        if(!index.isValid()) continue;
        auto item = m_pModel->item(index.row(), ColumnNo::Name);
        if(!item) continue;
        QString szName = item->text();
        QIcon icon = item->icon();
        QString szDescription = item->toolTip();
        QString szFile = m_pModel->item(index.row(), ColumnNo::File)->text();
        emit sigAddToFavorite(szName, szDescription, icon, szFile);
    }
}
