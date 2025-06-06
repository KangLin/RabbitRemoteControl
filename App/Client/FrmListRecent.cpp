#include "FrmListRecent.h"
#include "RabbitCommonDir.h"
#include <QVBoxLayout>
#include <QDateTime>
#include <QStandardItem>
#include <QDir>
#include <QHeaderView>
#include <QMenu>

CFrmListRecent::CFrmListRecent(
    CClient* pClient,
    CParameterApp &parameterApp, bool bDock, QWidget *parent) :
    QWidget(parent),
    m_ParameterApp(parameterApp),
    m_pToolBar(nullptr),
    m_ptbOperate(nullptr),
    m_pMenuNew(nullptr),
    m_pModel(nullptr),
    m_pClient(pClient),
    m_nFileRow(0),
    m_bDock(bDock),
    m_pDockTitleBar(nullptr)
{
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_DeleteOnClose);
    setLayout(new QVBoxLayout(this));
    setWindowTitle(tr("List recent"));

    m_pToolBar = new QToolBar(this);

    m_pStart = m_pToolBar->addAction(
        QIcon::fromTheme("media-playback-start"), tr("Start"),
        this, SLOT(slotConnect()));
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
    //m_ptbConnect->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_pMenuNew = new QMenu(tr("New"), this);
    m_pMenuNew->setIcon(QIcon::fromTheme("add"));
    m_ptbOperate->setMenu(m_pMenuNew);
    m_ptbOperate->setIcon(m_pMenuNew->icon());
    m_ptbOperate->setText(tr("New"));
    m_ptbOperate->setToolTip(tr("New"));
    m_ptbOperate->setStatusTip(tr("New"));
    m_pToolBar->addWidget(m_ptbOperate);
    m_pClient->EnumPlugins(this);
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

    Q_ASSERT(m_pClient);
    m_pTableView = new QTableView(this);
    m_pTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    bool check = connect(m_pTableView,
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
    m_pModel->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
    m_pModel->setHorizontalHeaderItem(1, new QStandardItem(tr("Protocol")));
    m_pModel->setHorizontalHeaderItem(2, new QStandardItem(tr("Date")));
    m_pModel->setHorizontalHeaderItem(3, new QStandardItem(tr("ID")));
    m_nFileRow = 4;
    m_pModel->setHorizontalHeaderItem(m_nFileRow, new QStandardItem(tr("File")));
    if(!m_pDetail->isChecked()) {
        m_pTableView->hideColumn(3);
        m_pTableView->hideColumn(m_nFileRow);
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
        CConnecter* c = m_pClient->LoadConnecter(szFile);
        if(!c) continue;

        QList<QStandardItem*> lstItem;
        QStandardItem* pName = new QStandardItem(c->Icon(), c->Name());
        pName->setToolTip(c->Description());
        lstItem << pName;
        QStandardItem* pProtocol = new QStandardItem(c->Protocol());
        lstItem << pProtocol;
        QFileInfo fi(szFile);
        lstItem << new QStandardItem(fi.lastModified().toLocalTime().toString());
        QStandardItem* pId = new QStandardItem(c->Id());
        lstItem << pId;
        QStandardItem* pFile = new QStandardItem(szFile);
        lstItem << pFile;
        m_pModel->appendRow(lstItem);

        m_pClient->DeleteConnecter(c);
    }

    if(m_pModel->rowCount() > 0)
    {
        m_pStart->setEnabled(true);
        m_pEditOperate->setEnabled(true);
        m_pEdit->setEnabled(true);
        m_pCopy->setEnabled(true);
        m_pDelete->setEnabled(true);
    } else {
        m_pStart->setEnabled(false);
        m_pEditOperate->setEnabled(false);
        m_pEdit->setEnabled(false);
        m_pCopy->setEnabled(false);
        m_pDelete->setEnabled(false);
    }

    //以下设置列宽函数必须要数据加载完成后使用,才能应用
    //See: https://blog.csdn.net/qq_40450386/article/details/86083759
    //m_pTableView->resizeColumnsToContents(); //设置所有列宽度自适应内容
    m_pTableView->resizeColumnToContents(0); //设置第0列宽度自适应内容
    m_pTableView->resizeColumnToContents(1); //设置第1列宽度自适应内容
    m_pTableView->resizeColumnToContents(2); //设置第1列宽度自适应内容
    return;
}

int CFrmListRecent::InsertItem(CConnecter *c, QString& szFile)
{
    QList<QStandardItem*> lstItem;
    QStandardItem* pName = new QStandardItem(c->Icon(), c->Name());
    pName->setToolTip(c->Description());
    lstItem << pName;
    QStandardItem* pProtocol = new QStandardItem(c->Protocol());
    lstItem << pProtocol;
    QFileInfo fi(szFile);
    lstItem << new QStandardItem(fi.lastModified().toString());
    QStandardItem* pId = new QStandardItem(c->Id());
    lstItem << pId;
    QStandardItem* pFile = new QStandardItem(szFile);
    lstItem << pFile;
    m_pModel->insertRow(0, lstItem);
    m_pTableView->selectRow(0);
    return 0;
}

int CFrmListRecent::onProcess(const QString &id, CPluginClient *pPlug)
{
    // Connect menu and toolbar
    QAction* pAction = m_pMenuNew->addAction(
        pPlug->Protocol() + ": " + pPlug->DisplayName(),
        this, SLOT(slotNew()));
    pAction->setToolTip(pPlug->Description());
    pAction->setStatusTip(pPlug->Description());
    pAction->setData(id);
    pAction->setIcon(pPlug->Icon());
    return 0;
}

void CFrmListRecent::slotNew()
{
    QAction* pAction = dynamic_cast<QAction*>(this->sender());    
    CConnecter* c = m_pClient->CreateConnecter(pAction->data().toString());
    if(nullptr == c) return;

    int nRet = c->OpenDialogSettings(this);
    switch(nRet)
    {
    case QDialog::Rejected:
        break;
    case QDialog::Accepted:
    {
        QString szFile = c->GetSettingsFile();
        QDir d;
        if(d.exists(szFile)) {
            QMessageBox::StandardButton r
                = QMessageBox::warning(
                    this, tr("Warning"),
                    tr("File of operate is exists. whether to overwrite it? File: %1").arg(szFile),
                    QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::No,
                    QMessageBox::StandardButton::No);
            if(QMessageBox::StandardButton::Ok == r)
            {
                d.remove(szFile);
                m_pClient->SaveConnecter(c);
            }
            break;
        }

        m_pClient->SaveConnecter(c);
        
        InsertItem(c, szFile);

        break;
    }
    }

    m_pClient->DeleteConnecter(c);
}

void CFrmListRecent::slotEdit()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        CConnecter* c = m_pClient->LoadConnecter(szFile);
        int nRet = c->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            break;
        case QDialog::Accepted:
            m_pClient->SaveConnecter(c);
            break;
        }
        m_pClient->DeleteConnecter(c);
    }
}

void CFrmListRecent::slotEditConnect()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        CConnecter* c = m_pClient->LoadConnecter(szFile);
        int nRet = c->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            break;
        case QDialog::Accepted:
            m_pClient->SaveConnecter(c);
            emit sigConnect(szFile);
            break;
        }
        m_pClient->DeleteConnecter(c);
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
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        CConnecter* c = m_pClient->LoadConnecter(szFile);

        do {
            bool bExit = true;
            bool bInsert = true;
            int nRet = c->OpenDialogSettings(this);
            switch(nRet)
            {
            case QDialog::Rejected:
                break;
            case QDialog::Accepted:
            {
                szFile = c->GetSettingsFile();
                QDir d(szFile);
                if(d.exists(szFile)) {
                    QMessageBox::StandardButton r
                        = QMessageBox::warning(
                            this,
                            tr("Warning"),
                            tr("File of operate is exists. whether to overwrite it? "
                               "If select No, please modify the name of operate"),
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
                m_pClient->SaveConnecter(c);
                if(bInsert)
                    InsertItem(c, szFile);
                break;
            }
            }

            if(bExit)
                break;
        } while(1);

        m_pClient->DeleteConnecter(c);
    }
}

void CFrmListRecent::slotDelete()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        QDir d(szFile);
        if(d.remove(szFile))
            m_pModel->removeRow(index.row());
    }
}

void CFrmListRecent::slotConnect()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        emit sigConnect(szFile);
    }
    if(!m_bDock) close();
}

void CFrmListRecent::slotDetail()
{
    if(m_pDetail->isChecked()) {
        m_pTableView->showColumn(3);
        m_pTableView->showColumn(m_nFileRow);
    } else {
        m_pTableView->hideColumn(3);
        m_pTableView->hideColumn(m_nFileRow);
    }
}

void CFrmListRecent::slotCustomContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    
    menu.addAction(QIcon::fromTheme("media-playback-start"),
                   tr("Start"), this, SLOT(slotConnect()));
    menu.addAction(tr("Edit and Start"), this, SLOT(slotEditConnect()));
    menu.addSeparator();
    menu.addMenu(m_pMenuNew);
    menu.addAction(tr("Edit"), this, SLOT(slotEdit()));
    menu.addAction(QIcon::fromTheme("edit-copy"), tr("Copy"),
                   this, SLOT(slotCopy()));
    menu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"),
                   this, SLOT(slotDelete()));
    menu.addSeparator();
    menu.addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"),
                   this, SLOT(slotLoadFiles()));

    menu.exec(mapToGlobal(pos));
}

void CFrmListRecent::slotDoubleClicked(const QModelIndex& index)
{
    QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
    emit sigConnect(szFile);
    if(!m_bDock) close();
}
