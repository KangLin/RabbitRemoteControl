// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QInputDialog>
#include <QMenu>
#include <QDesktopServices>
#include <QLoggingCategory>
#include "FrmFileTransfer.h"
#include "ui_FrmFileTransfer.h"

static Q_LOGGING_CATEGORY(log, "FileTransfer.Widget")
CFrmFileTransfer::CFrmFileTransfer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmFileTransfer)
    , m_pModelLocalDir(new QFileSystemModel(this))
    , m_pModelLocalFile(new QFileSystemModel(this))
    , m_pListFileModel(new CListFileModel(this))
{
    bool check = false;
    ui->setupUi(this);

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
    // 注意：必须在 setModel 之后才会生效
    ui->treeLocal->header()->hideSection(1);
    ui->treeLocal->header()->hideSection(2);
    ui->treeLocal->header()->hideSection(3);

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

    ui->treeRemote->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeRemote->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabRemote->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tabRemote->setSelectionBehavior(QAbstractItemView::SelectRows);
    
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
    delete ui;
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

void CFrmFileTransfer::on_cbLocal_editTextChanged(const QString &text)
{
    qDebug(log) << Q_FUNC_INFO << text;
    auto idx = m_pModelLocalDir->index(text);
    if(!idx.isValid()) return;
    if(text.length() > 1 && (text.right(1) == '/' || text.right(1) == '\\')) return;
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
    QString szName = QInputDialog::getText(this, tr("New folder"), tr("Folder name:"));
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
    }
    menu.exec(ui->tabLocal->viewport()->mapToGlobal(pos));
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

void CFrmFileTransfer::on_cbRemote_editTextChanged(const QString &arg1)
{
    
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
        menu.addAction(tr("Copy url to clipboard"),
                       this, SLOT(slotTreeRemoteCopyToClipboard()));
    }
    menu.exec(ui->treeRemote->viewport()->mapToGlobal(pos));
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
