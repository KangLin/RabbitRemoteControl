#include "FrmListRecentConnects.h"
#include "RabbitCommonDir.h"
#include <QGridLayout>
#include <QDateTime>
#include <QStandardItem>
#include <QDir>
#include <QHeaderView>
#include <QMenu>

CFrmListRecentConnects::CFrmListRecentConnects(CClient* pClient, bool bDock, QWidget *parent) :
    QWidget(parent),
    m_pToolBar(nullptr),
    m_ptbConnect(nullptr),
    m_pMenuNew(nullptr),
    m_pModel(nullptr),
    m_pClient(pClient),
    m_nFileRow(0),
    m_bDock(bDock),
    m_pDockTitleBar(nullptr)
{
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_DeleteOnClose);
    setLayout(new QGridLayout(this));
    setWindowTitle(tr("List recent connections"));

    m_pToolBar = new QToolBar(this);
    m_pConnect = m_pToolBar->addAction(
        QIcon::fromTheme("network-wired"), tr("Connect"),
        this, SLOT(slotConnect()));
    m_pConnect->setStatusTip(tr("Connect"));
    m_pConnect->setToolTip(tr("Connect"));
    m_pEditConnect = m_pToolBar->addAction(
        QIcon::fromTheme("edit-connect"), tr("Edit and Connect"),
        this, SLOT(slotEditConnect()));
    m_pEditConnect->setStatusTip(tr("Edit and Connect"));
    m_pEditConnect->setToolTip(tr("Edit and Connect"));
    m_pToolBar->addSeparator();

    m_ptbConnect = new QToolButton(m_pToolBar);
    m_ptbConnect->setFocusPolicy(Qt::NoFocus);
    m_ptbConnect->setPopupMode(QToolButton::InstantPopup);
    //m_ptbConnect->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_pMenuNew = new QMenu(tr("New"), this);
    m_pMenuNew->setIcon(QIcon::fromTheme("add"));
    m_ptbConnect->setMenu(m_pMenuNew);
    m_ptbConnect->setIcon(m_pMenuNew->icon());
    m_ptbConnect->setText(tr("New"));
    m_ptbConnect->setToolTip(tr("New"));
    m_ptbConnect->setStatusTip(tr("New"));
    m_pToolBar->addWidget(m_ptbConnect);
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
        
        pMenu->addAction(m_pConnect);
        pMenu->addAction(m_pEditConnect);
        pMenu->addMenu(m_pMenuNew);
        pMenu->addAction(m_pEdit);
        pMenu->addAction(m_pCopy);
        pMenu->addAction(m_pDelete);
        pMenu->addAction(m_pDetail);
        pMenu->addAction(m_pRefresh);
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
    layout()->addWidget(m_pTableView);;

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
        QHeaderView::ResizeToContents);
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

CFrmListRecentConnects::~CFrmListRecentConnects()
{
}

void CFrmListRecentConnects::slotLoadFiles()
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
        m_pConnect->setEnabled(true);
        m_pEditConnect->setEnabled(true);
        m_pEdit->setEnabled(true);
        m_pCopy->setEnabled(true);
        m_pDelete->setEnabled(true);
    } else {
        m_pConnect->setEnabled(false);
        m_pEditConnect->setEnabled(false);
        m_pEdit->setEnabled(false);
        m_pCopy->setEnabled(false);
        m_pDelete->setEnabled(false);
    }

    return;
}

int CFrmListRecentConnects::InsertItem(CConnecter *c, QString& szFile)
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

int CFrmListRecentConnects::onProcess(const QString &id, CPluginClient *pPlug)
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

void CFrmListRecentConnects::slotNew()
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
                    tr("File of connecter is exists. whether to overwrite it? File: %1").arg(szFile),
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

void CFrmListRecentConnects::slotEdit()
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

void CFrmListRecentConnects::slotEditConnect()
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

void CFrmListRecentConnects::slotCopy()
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
                            tr("File of connecter is exists. whether to overwrite it? "
                               "If select No, please modify the name of connecter"),
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

void CFrmListRecentConnects::slotDelete()
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

void CFrmListRecentConnects::slotConnect()
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

void CFrmListRecentConnects::slotDetail()
{
    if(m_pDetail->isChecked()) {
        m_pTableView->showColumn(3);
        m_pTableView->showColumn(m_nFileRow);
    } else {
        m_pTableView->hideColumn(3);
        m_pTableView->hideColumn(m_nFileRow);
    }
}

void CFrmListRecentConnects::slotCustomContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    
    menu.addAction(QIcon::fromTheme("network-wired"),
                   tr("Connect"), this, SLOT(slotConnect()));
    menu.addAction(tr("Edit and Connect"), this, SLOT(slotEditConnect()));
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

void CFrmListRecentConnects::slotDoubleClicked(const QModelIndex& index)
{
    QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
    emit sigConnect(szFile);
    if(!m_bDock) close();
}
