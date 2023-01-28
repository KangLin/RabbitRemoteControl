#include "FrmOpenConnect.h"
#include "ui_FrmOpenConnect.h"
#include "RabbitCommonDir.h"
#include <QDateTime>
#include <QStandardItem>
#include <QDir>
#include <QHeaderView>
#include <QMenu>

CFrmOpenConnect::CFrmOpenConnect(CClient* pClient, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmOpenConnect),
    m_pToolBar(nullptr),
    m_pModel(nullptr),
    m_pClient(pClient),
    m_nFileRow(0)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    m_pToolBar = new QToolBar(this);
    m_pToolBar->addAction(QIcon::fromTheme("network-wired"), tr("Connect"),
                          this, SLOT(slotConnect()));
    m_pToolBar->addAction(/*QIcon::fromTheme("network-wired"), */tr("Edit and Connect"),
                          this, SLOT(slotEditConnect()));
    m_pToolBar->addAction(/*QIcon::fromTheme("edit-copy"),*/ tr("Edit"),
                          this, SLOT(slotEdit()));
    m_pToolBar->addAction(QIcon::fromTheme("edit-copy"), tr("Copy"),
                          this, SLOT(slotCopy()));
    m_pToolBar->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"),
                          this, SLOT(slotDelete()));
    m_pToolBar->addAction(QIcon::fromTheme("window-close"), tr("Close"),
                          this, SLOT(slotCancel()));
    ui->gridLayout->addWidget(m_pToolBar);

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
    ui->gridLayout->addWidget(m_pTableView);;
    
    m_pModel = new QStandardItemModel(m_pTableView);
    m_pTableView->setModel(m_pModel);
    m_pTableView->verticalHeader()->hide();
    m_pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
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

    LoadFiles();
    
    //必须在 setModel 后,才能应用
    /*第二个参数可以为：
    QHeaderView::Interactive     ：0 用户可设置，也可被程序设置成默认大小
    QHeaderView::Fixed           ：2 用户不可更改列宽
    QHeaderView::Stretch         ：1 根据空间，自动改变列宽，用户与程序不能改变列宽
    QHeaderView::ResizeToContents：3 根据内容改变列宽，用户与程序不能改变列宽
    */
    m_pTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //以下设置列宽函数必须要数据加载完成后使用,才能应用
    //See: https://blog.csdn.net/qq_40450386/article/details/86083759
    //m_pTableView->resizeColumnsToContents(); //设置所有列宽度自适应内容
    //m_pTableView->resizeColumnToContents(0); //设置第0列宽度自适应内容
    //m_pTableView->resizeColumnToContents(2); //设置第1列宽度自适应内容
}

CFrmOpenConnect::~CFrmOpenConnect()
{
    delete ui;
}

int CFrmOpenConnect::LoadFiles()
{
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
    return 0;
}

void CFrmOpenConnect::slotCancel()
{
    close();
}

void CFrmOpenConnect::slotEdit()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        CConnecter* p = m_pClient->LoadConnecter(szFile);
        int nRet = p->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            break;
        case QDialog::Accepted:
            m_pClient->SaveConnecter(szFile, p);
            break;
        }
        p->deleteLater();
    }
}

void CFrmOpenConnect::slotEditConnect()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        CConnecter* p = m_pClient->LoadConnecter(szFile);
        int nRet = p->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            break;
        case QDialog::Accepted:
            m_pClient->SaveConnecter(szFile, p);
            emit sigConnect(szFile);
            break;
        }
        p->deleteLater();
    }
    close();
}

void CFrmOpenConnect::slotCopy()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        CConnecter* p = m_pClient->LoadConnecter(szFile);
        int nRet = p->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            break;
        case QDialog::Accepted:
            szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                        + QDir::separator()
                        + p->Id()
                        + ".rrc";
            QDir d(szFile);
            if(d.exists(szFile)) {
                QMessageBox::warning(this, "Replace",
                    "File of connecter is exists. please modify the name of connecter");
            } else {
                m_pClient->SaveConnecter(szFile, p);
            }
            break;
        }
        p->deleteLater();
    }
    LoadFiles();
}

void CFrmOpenConnect::slotDelete()
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

void CFrmOpenConnect::on_pbAdd_clicked()
{
    LoadFiles();
}

void CFrmOpenConnect::slotConnect()
{
    QItemSelectionModel* pSelect = m_pTableView->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
        emit sigConnect(szFile);
    }
    close();
}

void CFrmOpenConnect::slotCustomContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    
    menu.addAction(tr("Connect"), this, SLOT(slotConnect()));
    menu.addAction(tr("Edit and Connect"), this, SLOT(slotEditConnect()));
    menu.addAction(tr("Edit"), this, SLOT(slotEdit()));
    menu.addAction(tr("Copy"), this, SLOT(slotCopy()));
    menu.addAction(tr("Delete"), this, SLOT(slotDelete()));
    
    menu.exec(mapToGlobal(pos));
}

void CFrmOpenConnect::slotDoubleClicked(const QModelIndex& index)
{
    QString szFile = m_pModel->item(index.row(), m_nFileRow)->text();
    emit sigConnect(szFile);
    close();
}
