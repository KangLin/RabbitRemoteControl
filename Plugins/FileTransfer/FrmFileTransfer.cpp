// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QInputDialog>
#include <QMenu>
#include <QUrl>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QDesktopServices>
#include <QLoggingCategory>
#include "FrmFileTransfer.h"
#include "ui_FrmFileTransfer.h"

static Q_LOGGING_CATEGORY(log, "FileTransfer.Widget")
CFrmFileTransfer::CFrmFileTransfer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmFileTransfer)
    , m_pModelLocalDir(nullptr)
    , m_pModelLocalFile(nullptr)
    , m_pModelRemoteDir(nullptr)
    , m_pModelRemoteFile(nullptr)
    , m_pListFileModel(nullptr)
{
    bool check = false;
    ui->setupUi(this);

    m_pModelLocalDir = new QFileSystemModel(this);
    m_pModelLocalDir->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    m_pModelLocalDir->setReadOnly(false);
    ui->treeLocal->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeLocal->setModel(m_pModelLocalDir);
    ui->treeLocal->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeLocal->setSelectionBehavior(QAbstractItemView::SelectRows);
    check = connect(ui->treeLocal, &QTreeView::clicked,
                    this, &CFrmFileTransfer::slotTreeLocalClicked);
    Q_ASSERT(check);
    SetLocalRoot(QString());
    ui->treeLocal->setHeaderHidden(true);
    ui->treeLocal->header()->hide();
    // 注意：必须在 setModel 之后才会生效
    ui->treeLocal->header()->hideSection(1);
    ui->treeLocal->header()->hideSection(2);
    ui->treeLocal->header()->hideSection(3);

    m_pModelLocalFile = new QFileSystemModel(this);
    m_pModelLocalFile->setFilter(QDir::Files);
    m_pModelLocalFile->setReadOnly(false);
    ui->tabLocal->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tabLocal->setModel(m_pModelLocalFile);
    ui->tabLocal->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tabLocal->setShowGrid(false);
    ui->tabLocal->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabLocal->verticalHeader()->hide();
    ui->tabLocal->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);

    m_pModelRemoteDir = new CRemoteFileSystemModel(this);
    SetRemoteConnecter(m_pModelRemoteDir);
    m_pModelRemoteDir->SetRootPath("/");
    //m_pModelRemoteDir->SetFilter((CRemoteFileSystem::TYPES)(CRemoteFileSystem::TYPE::DIR) | CRemoteFileSystem::TYPE::DRIVE);
    ui->treeRemote->setModel(m_pModelRemoteDir);
    ui->treeRemote->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeRemote->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeRemote->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeRemote->header()->hide();
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::Type);
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::Size);
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::LastModified);
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::Permission);
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::Owner);
    
    m_pModelRemoteFile = new CRemoteFileSystemModel(this);
    SetRemoteConnecter(m_pModelRemoteFile);
    m_pModelRemoteFile->SetRootPath("/");
    //m_pModelRemoteFile->SetFilter(CRemoteFileSystem::TYPE::FILE);
    ui->tabRemote->setModel(m_pModelRemoteFile);
    ui->tabRemote->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tabRemote->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tabRemote->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabRemote->verticalHeader()->hide();
    // ui->tabRemote->horizontalHeader()->setSectionResizeMode(
    //     QHeaderView::ResizeToContents);

    m_pListFileModel = new CListFileModel(this);
    ui->tabList->setModel(m_pListFileModel);
    ui->tabList->setColumnHidden((int)CListFileModel::ColumnValue::Explanation, true);
    ui->tabList->setColumnHidden((int)CListFileModel::ColumnValue::Time, true);
    ui->tabFail->setModel(m_pListFileModel);
    ui->tabFail->setColumnHidden((int)CListFileModel::ColumnValue::State, true);
    ui->tabFail->setColumnHidden((int)CListFileModel::ColumnValue::Speed, true);
    ui->tabSuccess->setModel(m_pListFileModel);
    ui->tabSuccess->setColumnHidden((int)CListFileModel::ColumnValue::State, true);
    ui->tabSuccess->setColumnHidden((int)CListFileModel::ColumnValue::Speed, true);
    ui->tabSuccess->setColumnHidden((int)CListFileModel::ColumnValue::Explanation, true);
}

CFrmFileTransfer::~CFrmFileTransfer()
{
    qDebug(log) << Q_FUNC_INFO;
    delete ui;
}

void CFrmFileTransfer::SetRemoteConnecter(CRemoteFileSystemModel *p)
{
    bool check = connect(p, SIGNAL(sigGetDir(CRemoteFileSystem*)),
                    this, SIGNAL(sigGetDir(CRemoteFileSystem*)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigGetDir(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> > , bool)),
                    p, SLOT(slotGetDir(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> > , bool)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigRemoveDir(const QString&)),
                    this, SIGNAL(sigRemoveDir(const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigRemoveFile(const QString&)),
                    this, SIGNAL(sigRemoveFile(const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigRename(const QString&, const QString&)),
                    this, SIGNAL(sigRename(const QString&, const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigMakeDir(const QString&)),
                    this, SIGNAL(sigMakeDir(const QString&)));
    Q_ASSERT(check);
}

int CFrmFileTransfer::SetLocalRoot(const QString &root)
{
    auto index = m_pModelLocalDir->setRootPath(root);
    ui->treeLocal->setRootIndex(index);
    slotTreeLocalClicked(index);
    return 0;
}

QString CFrmFileTransfer::GetLocalRoot() const
{
    return m_pModelLocalDir->rootPath();
}

void CFrmFileTransfer::slotTreeLocalClicked(const QModelIndex &index)
{
    QString szPath = m_pModelLocalDir->filePath(index);
    if(szPath.isEmpty()) return;
    if(-1 == ui->cbLocal->findText(szPath))
        ui->cbLocal->addItem(szPath);
    ui->cbLocal->setCurrentText(szPath);
    QModelIndex idx = m_pModelLocalFile->setRootPath(szPath);
    ui->tabLocal->setRootIndex(idx);
}

void CFrmFileTransfer::on_cbLocal_editTextChanged(const QString &szPath)
{
    qDebug(log) << Q_FUNC_INFO << szPath;
    auto idx = m_pModelLocalDir->index(szPath);
    if(!idx.isValid()) return;
    if(szPath.length() > 1 && (szPath.right(1) == '/' || szPath.right(1) == '\\')) return;
    ui->treeLocal->setCurrentIndex(idx);
    slotTreeLocalClicked(idx);
}

void CFrmFileTransfer::on_treeLocal_customContextMenuRequested(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    auto idx = ui->treeLocal->currentIndex();
    QMenu menu;
    if(idx.isValid()) {
        menu.addAction(QIcon::fromTheme("go-up"), tr("Upload"),
                       this, SLOT(slotTreeLocalUpload()));
        menu.addAction(QIcon::fromTheme("list-add"), tr("Add to list"),
                       this, SLOT(slotTreeLocalAddToList()));
        menu.addAction(QIcon::fromTheme("folder-open"), tr("Open"),
                       this, SLOT(slotTreeLocalOpen()));
    }
    menu.addAction(QIcon::fromTheme("document-new"), tr("New"),
                   this, SLOT(slotTreeLocalNew()));
    if(idx.isValid()) {
        menu.addAction(QIcon::fromTheme("remove"), tr("Delete"),
                       this, SLOT(slotTreeLocalDelete()));
        menu.addAction(tr("Rename"),
                       this, SLOT(slotTreeLocalRename()));
        menu.addAction(tr("Copy path to clipboard"),
                       this, SLOT(slotTreeLocalCopyToClipboard()));
    }
    menu.exec(ui->treeLocal->viewport()->mapToGlobal(pos));
}

void CFrmFileTransfer::slotTreeLocalOpen()
{
    auto idx = ui->treeLocal->currentIndex();
    auto szPath = m_pModelLocalDir->filePath(idx);
    QDesktopServices::openUrl(QUrl(szPath));
}

void CFrmFileTransfer::slotTreeLocalNew()
{
    QString szName = QInputDialog::getText(
        this, tr("New folder"), tr("Folder name:"));
    if(szName.isEmpty()) return;
    auto idx = ui->treeLocal->currentIndex();
    m_pModelLocalDir->mkdir(idx, szName);
}

void CFrmFileTransfer::slotTreeLocalDelete()
{
    auto idx = ui->treeLocal->currentIndex();
    m_pModelLocalDir->remove(idx);
}

void CFrmFileTransfer::slotTreeLocalRename()
{
    ui->treeLocal->edit(ui->treeLocal->currentIndex());
}

void CFrmFileTransfer::slotTreeLocalUpload()
{
    // TODO: implemented
}

void CFrmFileTransfer::slotTreeLocalAddToList()
{
    // TODO: implemented
}

void CFrmFileTransfer::slotTreeLocalCopyToClipboard()
{
    auto idx = ui->treeLocal->currentIndex();
    QString szPath = m_pModelLocalDir->filePath(idx);
    if(szPath.isEmpty()) return;
    QClipboard* pClipboard = QApplication::clipboard();
    pClipboard->setText(szPath);
    // QMimeData* d = new QMimeData();
    // d->setUrls(QList<QUrl>() << QUrl::fromLocalFile(szPath));
    // pClipboard->setMimeData(d);
}

void CFrmFileTransfer::on_tabLocal_customContextMenuRequested(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    auto idx = ui->tabLocal->currentIndex();
    QMenu menu;
    if(idx.isValid()) {
        menu.addAction(QIcon::fromTheme("go-up"), tr("Upload"),
                       this, SLOT(slotTabLocalUpload()));
        menu.addAction(QIcon::fromTheme("list-add"), tr("Add to list"),
                       this, SLOT(slotTabLocalAddToList()));
        menu.addAction(QIcon::fromTheme("file-open"), tr("Open"),
                       this, SLOT(slotTabLocalOpen()));
        menu.addAction(QIcon::fromTheme("file-edit"), tr("Edit"),
                       this, SLOT(slotTabLocalEdit()));
        menu.addAction(QIcon::fromTheme("remove"), tr("Delete"),
                       this, SLOT(slotTabLocalDelete()));
        menu.addAction(tr("Rename"),
                       this, SLOT(slotTabLocalRename()));
        menu.addAction(tr("Copy path to clipboard"),
                       this, SLOT(slotTabLocalCopyToClipboard()));
    }
    menu.exec(ui->tabLocal->viewport()->mapToGlobal(pos));
}

void CFrmFileTransfer::slotTabLocalCopyToClipboard()
{
    auto idx = ui->tabLocal->currentIndex();
    QString szPath = m_pModelLocalDir->filePath(idx);
    if(szPath.isEmpty()) return;
    QClipboard* pClipboard = QApplication::clipboard();
    pClipboard->setText(szPath);
    // QMimeData* d = new QMimeData();
    // d->setUrls(QList<QUrl>() << QUrl::fromLocalFile(szPath));
    // pClipboard->setMimeData(d);
}

void CFrmFileTransfer::slotTabLocalUpload()
{
}

void CFrmFileTransfer::slotTabLocalAddToList()
{
}

void CFrmFileTransfer::slotTabLocalOpen()
{
    auto idx = ui->tabLocal->currentIndex();
    auto szPath = m_pModelLocalFile->filePath(idx);
    QDesktopServices::openUrl(QUrl(szPath));
}

void CFrmFileTransfer::slotTabLocalEdit()
{
    // TODO: implemented
    qDebug(log) << Q_FUNC_INFO << "Not implemented";
}

void CFrmFileTransfer::slotTabLocalDelete()
{
    auto idx = ui->tabLocal->currentIndex();
    m_pModelLocalDir->remove(idx);
}

void CFrmFileTransfer::slotTabLocalRename()
{
    ui->tabLocal->edit(ui->tabLocal->currentIndex());
}

void CFrmFileTransfer::on_cbRemote_editTextChanged(const QString &szPath)
{
    qDebug(log) << Q_FUNC_INFO << szPath;
    // TODO: Not implemented
    QModelIndex idx;
    if(!idx.isValid()) return;
    if(szPath.length() > 1 && (szPath.right(1) == '/' || szPath.right(1) == '\\')) return;
    ui->treeRemote->setCurrentIndex(idx);
    on_treeRemote_clicked(idx);
}

void CFrmFileTransfer::on_cbRemote_currentIndexChanged(int index)
{
    QModelIndex idx = ui->cbRemote->itemData(index).value<QModelIndex>();
    if(idx.isValid()) {
        ui->treeRemote->setCurrentIndex(idx);
        on_treeRemote_clicked(idx);
        return;
    }
    QString szPath = ui->cbRemote->itemText(index);
    on_cbRemote_editTextChanged(szPath);
}

void CFrmFileTransfer::on_treeRemote_clicked(const QModelIndex &index)
{
    CRemoteFileSystem* pRemoteFileSystem = m_pModelRemoteDir->GetRemoteFileSystemFromIndex(index);
    if(!pRemoteFileSystem) return;
    QString szPath = pRemoteFileSystem->GetPath();
    if(szPath.isEmpty()) return;
    qDebug(log) << Q_FUNC_INFO << szPath;
    m_pModelRemoteFile->SetRootPath(pRemoteFileSystem->GetPath());
    if(-1 == ui->cbRemote->findText(szPath)) {
        ui->cbRemote->addItem(szPath, index);
    }
    ui->cbRemote->setCurrentText(szPath);
}

void CFrmFileTransfer::on_treeRemote_doubleClicked(const QModelIndex &index)
{
    qDebug(log) << Q_FUNC_INFO << index;
    if(index.isValid())
        ui->treeRemote->expand(index);
}

void CFrmFileTransfer::on_treeRemote_customContextMenuRequested(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    auto idx = ui->treeRemote->currentIndex();
    QMenu menu;
    if(idx.isValid()) {
        menu.addAction(QIcon::fromTheme("emblem-downloads"), tr("Download"),
                       this, SLOT(slotTreeRemoteDownload()));
        menu.addAction(QIcon::fromTheme("list-add"), tr("Add to list"),
                       this, SLOT(slotTreeRemoteAddToList()));
    }
    menu.addAction(QIcon::fromTheme("document-new"), tr("New"),
                   this, SLOT(slotTreeRemoteNew()));
    if(idx.isValid()) {
        menu.addAction(QIcon::fromTheme("remove"), tr("Delete"),
                       this, SLOT(slotTreeRemoteDelete()));
        menu.addAction(tr("Rename"),
                       this, SLOT(slotTreeRemoteRename()));
        menu.addAction(QIcon::fromTheme("refresh"), tr("Refresh"),
                       this, SLOT(slotTreeRemoteRefresh()));
        menu.addAction(tr("Copy url to clipboard"),
                       this, SLOT(slotTreeRemoteCopyToClipboard()));
    }
    menu.exec(ui->treeRemote->viewport()->mapToGlobal(pos));
}

void CFrmFileTransfer::slotTreeRemoteDownload()
{
}

void CFrmFileTransfer::slotTreeRemoteAddToList()
{
}

void CFrmFileTransfer::slotTreeRemoteNew()
{
    QString szName = QInputDialog::getText(
        this, tr("New folder"), tr("Folder name:"));
    if(szName.isEmpty()) return;
    auto idx = ui->treeRemote->currentIndex();
    if(idx.isValid())
        m_pModelRemoteDir->CreateDir(idx, szName);
}

void CFrmFileTransfer::slotTreeRemoteDelete()
{
    auto idx = ui->treeRemote->currentIndex();
    if(idx.isValid())
        m_pModelRemoteDir->RemoveDir(idx);
}

void CFrmFileTransfer::slotTreeRemoteRefresh()
{
    auto idx = ui->treeRemote->currentIndex();
    if(idx.isValid()) {
        auto p = m_pModelRemoteDir->GetRemoteFileSystemFromIndex(idx);
        if(p && !p->GetPath().isEmpty()) {
            p->SetState(CRemoteFileSystem::State::No);
            ui->treeRemote->expand(idx);
        }
    }
}

void CFrmFileTransfer::slotTreeRemoteRename()
{
    ui->treeRemote->edit(ui->treeRemote->currentIndex());
}

void CFrmFileTransfer::slotTreeRemoteCopyToClipboard()
{
    auto idx = ui->treeRemote->currentIndex();
    if(idx.isValid()) {
        auto p = m_pModelRemoteDir->GetRemoteFileSystemFromIndex(idx);
        if(p) {
            QClipboard* pClipboard = QApplication::clipboard();
            //TODO: add protocol + host
            QString u = p->GetPath();
            QUrl url(u);
            pClipboard->setText(p->GetPath());
            //pClipboard->mimeData()->setUrls(QList<QUrl>() << url);
        }
    }
}

void CFrmFileTransfer::on_tabRemote_customContextMenuRequested(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    auto idx = ui->tabRemote->currentIndex();
    QMenu menu;
    if(idx.isValid()) {
        menu.addAction(QIcon::fromTheme("emblem-downloads"), tr("Download"),
                       this, SLOT(slotTabRemoteDownload()));
        menu.addAction(QIcon::fromTheme("list-add"), tr("Add to list"),
                       this, SLOT(slotTabRemoteAddToList()));
    }
    menu.addAction(QIcon::fromTheme("document-new"), tr("New"),
                   this, SLOT(slotTabRemoteNew()));
    if(idx.isValid()) {
        menu.addAction(QIcon::fromTheme("remove"), tr("Delete"),
                       this, SLOT(slotTabRemoteDelete()));
        menu.addAction(tr("Rename"),
                       this, SLOT(slotTabRemoteRename()));
        menu.addAction(tr("Copy url to clipboard"),
                       this, SLOT(slotTabRemoteCopyToClipboard()));
    }
    menu.exec(ui->tabRemote->viewport()->mapToGlobal(pos));
}

void CFrmFileTransfer::slotTabRemoteDownload()
{
}

void CFrmFileTransfer::slotTabRemoteAddToList()
{
}

void CFrmFileTransfer::slotTabRemoteNew()
{
}

void CFrmFileTransfer::slotTabRemoteDelete()
{
}

void CFrmFileTransfer::slotTabRemoteRename()
{}

void CFrmFileTransfer::slotTabRemoteCopyToClipboard()
{}
