#include "FrmListConnects.h"
#include "RabbitCommonDir.h"
#include <QGridLayout>
#include <QDateTime>
#include <QStandardItem>
#include <QDir>
#include <QHeaderView>
#include <QMenu>

CFrmListConnects::CFrmListConnects(CClient* pClient, bool bClose, QWidget *parent) :
    QWidget(parent),
    m_pToolBar(nullptr),
    m_ptbConnect(nullptr),
    m_pMenuNew(nullptr),
    m_pModel(nullptr),
    m_pClient(pClient),
    m_nFileRow(0),
    m_bClose(bClose)
{
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_DeleteOnClose);
    setLayout(new QGridLayout(this));
    setWindowTitle(tr("List connections"));

    m_pToolBar = new QToolBar(this);
    m_pToolBar->addAction(QIcon::fromTheme("network-wired"), tr("Connect"),
                          this, SLOT(slotConnect()));
    m_pToolBar->addAction(/*QIcon::fromTheme("network-wired"), */tr("Edit and Connect"),
                          this, SLOT(slotEditConnect()));
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
    m_pToolBar->addAction(QIcon::fromTheme("edit"), tr("Edit"),
                          this, SLOT(slotEdit()));
    m_pToolBar->addAction(QIcon::fromTheme("edit-copy"), tr("Copy"),
                          this, SLOT(slotCopy()));
    m_pToolBar->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"),
                          this, SLOT(slotDelete()));
    m_pToolBar->addSeparator();   
    if(m_bClose)
        m_pToolBar->addAction(QIcon::fromTheme("window-close"), tr("Close"),
                          this, SLOT(close()));
    else
        m_pToolBar->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"),
                          this, SLOT(slotLoadFiles()));

    layout()->addWidget(m_pToolBar);

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
#ifndef DEBUG
    m_pTableView->hideColumn(3);
    m_pTableView->hideColumn(m_nFileRow);
#endif
    
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

CFrmListConnects::~CFrmListConnects()
{
}

void CFrmListConnects::slotLoadFiles()
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
        lstItem << new QStandardItem(fi.lastModified().toString());
        QStandardItem* pId = new QStandardItem(c->Id());
        lstItem << pId;
        QStandardItem* pFile = new QStandardItem(szFile);
        lstItem << pFile;
        m_pModel->appendRow(lstItem);

        c->deleteLater();
    }
    return;
}

int CFrmListConnects::InsertItem(CConnecter *c, QString& szFile)
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

int CFrmListConnects::onProcess(const QString &id, CPluginClient *pPlug)
{
    // Connect menu and toolbar
    QAction* pAction = m_pMenuNew->addAction(pPlug->Protocol()
                                       + ": " + pPlug->DisplayName(),
                                       this, SLOT(slotNew()));
    pAction->setToolTip(pPlug->Description());
    pAction->setStatusTip(pPlug->Description());
    pAction->setData(id);
    pAction->setIcon(pPlug->Icon());
    return 0;
}

void CFrmListConnects::slotNew()
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
        QString szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                + QDir::separator()
                + c->Id()
                + ".rrc";
        QDir d;
        if(d.exists(szFile)) {
            QMessageBox::StandardButton r
                    = QMessageBox::warning(this, tr("Warning"),
              tr("File of connecter is exists. whether to overwrite it? File: %1").arg(szFile),
              QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::No,
                                               QMessageBox::StandardButton::No);
            if(QMessageBox::StandardButton::Ok == r)
            {
                d.remove(szFile);
                m_pClient->SaveConnecter(szFile, c);
            }
            break;
        }

        m_pClient->SaveConnecter(szFile, c);
        
        InsertItem(c, szFile);

        break;
    }
    }

    c->deleteLater();
}

void CFrmListConnects::slotEdit()
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
            m_pClient->SaveConnecter(szFile, c);
            break;
        }
        c->deleteLater();
    }
}

void CFrmListConnects::slotEditConnect()
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
            m_pClient->SaveConnecter(szFile, c);
            emit sigConnect(szFile);
            break;
        }
        c->deleteLater();
    }
    if(m_bClose)
        close();
}

void CFrmListConnects::slotCopy()
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
                szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                        + QDir::separator()
                        + c->Id()
                        + ".rrc";
                QDir d(szFile);
                if(d.exists(szFile)) {
                    QMessageBox::StandardButton r = QMessageBox::warning(this,
                        tr("Warning"),
                        tr("File of connecter is exists. whether to overwrite it? "
                        "If select No, please modify the name of connecter"), 
                        QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::No | QMessageBox::StandardButton::Cancel,
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
                m_pClient->SaveConnecter(szFile, c);
                if(bInsert)
                    InsertItem(c, szFile);
                break;
            }
            }

            if(bExit)
                break;
        } while(1);

        c->deleteLater();
    }
}

void CFrmListConnects::slotDelete()
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

void CFrmListConnects::slotConnect()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        emit sigConnect(szFile);
    }
    if(m_bClose) close();
}

void CFrmListConnects::slotCustomContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    
    menu.addAction(QIcon::fromTheme("network-wired"), tr("Connect"), this, SLOT(slotConnect()));
    menu.addAction(tr("Edit and Connect"), this, SLOT(slotEditConnect()));
    menu.addSeparator();
    menu.addMenu(m_pMenuNew);
    menu.addAction(tr("Edit"), this, SLOT(slotEdit()));
    menu.addAction(QIcon::fromTheme("edit-copy"), tr("Copy"), this, SLOT(slotCopy()));
    menu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"), this, SLOT(slotDelete()));
    menu.addSeparator();
    menu.addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"), this, SLOT(slotLoadFiles()));

    menu.exec(mapToGlobal(pos));
}

void CFrmListConnects::slotDoubleClicked(const QModelIndex& index)
{
    QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
    emit sigConnect(szFile);
    if(m_bClose) close();
}
