// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QMessageBox>
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

    m_pModelLocalDir = new QFileSystemModel(ui->treeLocal);
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

    m_pModelLocalFile = new QFileSystemModel(ui->tabLocal);
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

    m_pModelRemoteDir = new CRemoteFileSystemModel(ui->treeRemote, CRemoteFileSystem::TYPE::DIRS);
    SetRemoteConnect(m_pModelRemoteDir);
    ui->treeRemote->setModel(m_pModelRemoteDir);
    QModelIndex index = m_pModelRemoteDir->SetRootPath("/");
    ui->treeRemote->setRootIndex(index);
    ui->treeRemote->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeRemote->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeRemote->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeRemote->header()->hide();
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::Type);
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::Size);
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::LastModified);
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::Permission);
    ui->treeRemote->header()->hideSection((int)CRemoteFileSystem::ColumnValue::Owner);

    m_pModelRemoteFile = new CRemoteFileSystemModel(ui->tabRemote, CRemoteFileSystem::TYPE::FILE);
    SetRemoteConnect(m_pModelRemoteFile);
    ui->tabRemote->setModel(m_pModelRemoteFile);
    ui->tabRemote->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tabRemote->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tabRemote->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabRemote->verticalHeader()->hide();
    // ui->tabRemote->horizontalHeader()->setSectionResizeMode(
    //     QHeaderView::ResizeToContents);

    m_pListFileModel = new CListFileModel(ui->tabList);
    ui->tabList->setModel(m_pListFileModel);
    ui->tabList->verticalHeader()->hide();
    ui->tabList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tabList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tabList->setColumnHidden((int)CFileTransfer::ColumnValue::Explanation, true);
    ui->tabList->setColumnHidden((int)CFileTransfer::ColumnValue::Time, true);
    ui->tabFail->setModel(m_pListFileModel);
    ui->tabFail->verticalHeader()->hide();
    ui->tabFail->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabFail->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tabFail->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tabFail->setColumnHidden((int)CFileTransfer::ColumnValue::State, true);
    ui->tabFail->setColumnHidden((int)CFileTransfer::ColumnValue::Speed, true);
    ui->tabSuccess->setModel(m_pListFileModel);
    ui->tabSuccess->verticalHeader()->hide();
    ui->tabSuccess->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tabSuccess->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tabSuccess->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tabSuccess->setColumnHidden((int)CFileTransfer::ColumnValue::State, true);
    ui->tabSuccess->setColumnHidden((int)CFileTransfer::ColumnValue::Speed, true);
    ui->tabSuccess->setColumnHidden((int)CFileTransfer::ColumnValue::Explanation, true);
}

CFrmFileTransfer::~CFrmFileTransfer()
{
    qDebug(log) << Q_FUNC_INFO;
    delete ui;
}

void CFrmFileTransfer::SetRemoteConnect(CRemoteFileSystemModel *pRfs)
{
    bool check = connect(pRfs, SIGNAL(sigGetDir(CRemoteFileSystem*)),
                    this, SIGNAL(sigGetDir(CRemoteFileSystem*)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigGetDir(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> > , bool)),
                    pRfs, SLOT(slotGetDir(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> > , bool)));
    Q_ASSERT(check);
    check = connect(pRfs, SIGNAL(sigRemoveDir(const QString&)),
                    this, SIGNAL(sigRemoveDir(const QString&)));
    Q_ASSERT(check);
    check = connect(pRfs, SIGNAL(sigRemoveFile(const QString&)),
                    this, SIGNAL(sigRemoveFile(const QString&)));
    Q_ASSERT(check);
    check = connect(pRfs, SIGNAL(sigRename(const QString&, const QString&)),
                    this, SIGNAL(sigRename(const QString&, const QString&)));
    Q_ASSERT(check);
    check = connect(pRfs, SIGNAL(sigMakeDir(const QString&)),
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
    if(m_pModelLocalDir)
        return m_pModelLocalDir->rootPath();
    return QString();
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
    //qDebug(log) << Q_FUNC_INFO << szPath;
    if(!m_pModelLocalDir) return;
    auto idx = m_pModelLocalDir->index(szPath);
    if(!idx.isValid()) return;
    if(szPath.length() > 1 && (szPath.right(1) == '/' || szPath.right(1) == '\\')) return;
    ui->treeLocal->setCurrentIndex(idx);
    slotTreeLocalClicked(idx);
}

void CFrmFileTransfer::on_treeLocal_customContextMenuRequested(const QPoint &pos)
{
    //qDebug(log) << Q_FUNC_INFO;
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
        menu.addAction(QIcon::fromTheme("rename"), tr("Rename"),
                       this, SLOT(slotTreeLocalRename()));
        menu.addAction(QIcon::fromTheme("edit-copy"), tr("Copy path to clipboard"),
                       this, SLOT(slotTreeLocalCopyToClipboard()));
    }
    menu.exec(ui->treeLocal->viewport()->mapToGlobal(pos));
}

void CFrmFileTransfer::slotTreeLocalOpen()
{
    if(!m_pModelLocalDir) return;
    auto idx = ui->treeLocal->currentIndex();
    auto szPath = m_pModelLocalDir->filePath(idx);
    QDesktopServices::openUrl(QUrl(szPath));
}

void CFrmFileTransfer::slotTreeLocalNew()
{
    if(!m_pModelLocalDir) return;
    QString szName = QInputDialog::getText(
        this, tr("New folder"), tr("Folder name:"));
    if(szName.isEmpty()) return;
    auto idx = ui->treeLocal->currentIndex();
    m_pModelLocalDir->mkdir(idx, szName);
}

void CFrmFileTransfer::slotTreeLocalDelete()
{
    if(!m_pModelLocalDir) return;
    auto idx = ui->treeLocal->currentIndex();
    m_pModelLocalDir->remove(idx);
}

void CFrmFileTransfer::slotTreeLocalRename()
{
    ui->treeLocal->edit(ui->treeLocal->currentIndex());
}

void CFrmFileTransfer::slotTreeLocalUpload()
{
    qDebug(log) << Q_FUNC_INFO << "Complete it!";
    // TODO: implemented
}

int CFrmFileTransfer::EnumLocalDirectory(QDir d, const QString& szRemoteDir)
{
    foreach (auto f, d.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
        QString szLocal = d.absoluteFilePath(f);
        if(szLocal.isEmpty()) {
            qDebug(log) << "The select is empty";
            continue;
        }
        QFileInfo fi(szLocal);
        if(!fi.exists()) {
            QMessageBox::critical(this, tr("Error"),
                                  tr("The file is not exists:") + szLocal);
            continue;
        }
        QString szRemoteFile = szRemoteDir;
        if(szRemoteDir.right(1) == "/")
            szRemoteFile += fi.fileName();
        else
            szRemoteFile += "/" + fi.fileName();
        QSharedPointer<CFileTransfer> fileTransfer(new CFileTransfer(
            szLocal, szRemoteFile,
            CFileTransfer::Direction::Upload));
        fileTransfer->SetFileSize(fi.size());
        m_pListFileModel->AddFileTransfer(fileTransfer);
    }
    foreach(auto folder, d.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir dir(d.absoluteFilePath(folder));
        EnumLocalDirectory(dir, szRemoteDir + "/" + folder);
    }
    return 0;
}

void CFrmFileTransfer::slotTreeLocalAddToList()
{
    QString szRemote = ui->cbRemote->currentText();
    if(szRemote.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please select remote directory"));
        return;
    }
    auto idx = ui->treeLocal->currentIndex();
    if(!idx.isValid()) return;
    QString szPath = m_pModelLocalDir->filePath(idx);
    if(!m_pModelLocalDir->isDir(idx) || szPath.isEmpty()) return;
    QFileInfo fi(szPath);
    EnumLocalDirectory(QDir(szPath), szRemote + "/" + fi.fileName());
}

void CFrmFileTransfer::slotTreeLocalCopyToClipboard()
{
    if(!m_pModelLocalDir) return;
    auto idx = ui->treeLocal->currentIndex();
    QString szPath = m_pModelLocalDir->filePath(idx);
    if(szPath.isEmpty()) return;
    QClipboard* pClipboard = QApplication::clipboard();
    pClipboard->setText(szPath);
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
        menu.addAction(QIcon::fromTheme("rename"), tr("Rename"),
                       this, SLOT(slotTabLocalRename()));
        menu.addAction(QIcon::fromTheme("edit-copy"), tr("Copy path to clipboard"),
                       this, SLOT(slotTabLocalCopyToClipboard()));
    }
    menu.exec(ui->tabLocal->viewport()->mapToGlobal(pos));
}

void CFrmFileTransfer::slotTabLocalCopyToClipboard()
{
    if(!m_pModelLocalDir) return;
    auto idx = ui->tabLocal->currentIndex();
    if(!idx.isValid()) return;
    QString szPath = m_pModelLocalDir->filePath(idx);
    if(szPath.isEmpty()) return;
    QClipboard* pClipboard = QApplication::clipboard();
    pClipboard->setText(szPath);
}

void CFrmFileTransfer::slotTabLocalUpload()
{
    if(!m_pModelLocalDir) return;
    auto idx = ui->tabLocal->currentIndex();
    if(!idx.isValid()) return;
    QString szPath;
    szPath = m_pModelLocalDir->filePath(idx);
    if(szPath.isEmpty()) return;
}

void CFrmFileTransfer::slotTabLocalAddToList()
{
    QString szRemote = ui->cbRemote->currentText();
    if(szRemote.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please select remote directory"));
        return;
    }
    auto indexes = ui->tabLocal->selectionModel()->selectedRows();
    foreach(auto idx, indexes) {
        QString szLocal = m_pModelLocalFile->filePath(idx);
        if(szLocal.isEmpty()) {
            qDebug(log) << "The select is empty:" << idx;
            continue;
        }
        QFileInfo fi(szLocal);
        if(!fi.exists()) {
            QMessageBox::critical(this, tr("Error"), tr("The file is not exists:") + szLocal);
            continue;
        }
        QString szRemoteFile = szRemote;
        if(szRemote.right(1) == "/")
            szRemoteFile += fi.fileName();
        else
            szRemoteFile += "/" + fi.fileName();
        QSharedPointer<CFileTransfer> f(new CFileTransfer(szLocal, szRemoteFile,
            CFileTransfer::Direction::Upload));
        f->SetFileSize(fi.size());
        f->SetLocalPermission(m_pModelLocalFile->permissions(idx));
        m_pListFileModel->AddFileTransfer(f);
    }
}

void CFrmFileTransfer::slotTabLocalOpen()
{
    auto idx = ui->tabLocal->currentIndex();
    if(!idx.isValid()) return;
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
    if(idx.isValid())
        m_pModelLocalDir->remove(idx);
}

void CFrmFileTransfer::slotTabLocalRename()
{
    auto idx = ui->tabLocal->currentIndex();
    if(idx.isValid())
        ui->tabLocal->edit(idx);
}

void CFrmFileTransfer::on_cbRemote_editTextChanged(const QString &szPath)
{
    qDebug(log) << Q_FUNC_INFO << szPath;
    if(szPath.isEmpty()) return;
    if(!m_pModelRemoteDir) return;
    QModelIndex index = m_pModelRemoteDir->index(szPath);
    if(index.isValid()) {
        on_treeRemote_clicked(index);
        return;
    }
    
    auto idx = m_pModelRemoteDir->SetRootPath(szPath);
    ui->treeRemote->setRootIndex(idx);
}

// void CFrmFileTransfer::on_cbRemote_currentTextChanged(const QString &szPath)
// {
//     qDebug(log) << Q_FUNC_INFO << szPath;
//     if(szPath.length() > 1 && (szPath.right(1) == '/' || szPath.right(1) == '\\')) return;
//     if(m_pModelRemoteFile) {
//         auto idx = m_pModelRemoteFile->SetRootPath(szPath);
//         ui->tabRemote->setRootIndex(idx);
//     }
// }

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
    if(m_pModelRemoteFile) {
        auto idx = m_pModelRemoteFile->SetRootPath(pRemoteFileSystem->GetPath());
        ui->tabRemote->setRootIndex(idx);
    }
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
    // if(idx.isValid()) {
    //     menu.addAction(QIcon::fromTheme("emblem-downloads"), tr("Download"),
    //                    this, SLOT(slotTreeRemoteDownload()));
    //     menu.addAction(QIcon::fromTheme("list-add"), tr("Add to list"),
    //                    this, SLOT(slotTreeRemoteAddToList()));
    // }
    menu.addAction(QIcon::fromTheme("document-new"), tr("New"),
                   this, SLOT(slotTreeRemoteNew()));
    if(idx.isValid()) {
        menu.addAction(QIcon::fromTheme("remove"), tr("Delete"),
                       this, SLOT(slotTreeRemoteDelete()));
        menu.addAction(QIcon::fromTheme("rename"), tr("Rename"),
                       this, SLOT(slotTreeRemoteRename()));
        menu.addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"),
                       this, SLOT(slotTreeRemoteRefresh()));
        menu.addAction(QIcon::fromTheme("edit-copy"), tr("Copy url to clipboard"),
                       this, SLOT(slotTreeRemoteCopyToClipboard()));
    }
    menu.exec(ui->treeRemote->viewport()->mapToGlobal(pos));
}

void CFrmFileTransfer::slotTreeRemoteDownload()
{
    qDebug(log) << Q_FUNC_INFO;
}

int CFrmFileTransfer::EnumRemoteDirectory(CRemoteFileSystem *pRfs,
                                          const QString &szLocalDir)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!pRfs || !pRfs->IsDir()) return -1;

    for(int index = 0; index < pRfs->ChildCount(); index++) {
        //TODO: Complete it
    }
    return 0;
}

void CFrmFileTransfer::slotTreeRemoteAddToList()
{
    QString szLocal = ui->cbLocal->currentText();
    if(szLocal.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please select local directory"));
        return;
    }
    auto idx = ui->treeRemote->currentIndex();
    if(!idx.isValid()) return;
    auto p = m_pModelRemoteDir->GetRemoteFileSystemFromIndex(idx);
    if(p && !(p->GetType() & CRemoteFileSystem::TYPE::FILE))
        EnumRemoteDirectory(p, szLocal);
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
            m_pModelRemoteDir->fetchMore(idx);
        }
    }
}

void CFrmFileTransfer::slotTreeRemoteRename()
{
    auto idx = ui->treeRemote->currentIndex();
    if(!idx.isValid()) return;
    ui->treeRemote->edit(idx);
}

void CFrmFileTransfer::slotTreeRemoteCopyToClipboard()
{
    auto idx = ui->treeRemote->currentIndex();
    if(idx.isValid()) {
        auto p = m_pModelRemoteDir->GetRemoteFileSystemFromIndex(idx);
        if(p) {
            QString szPath = p->GetPath();
            emit sigCopyUrlToClipboard(szPath);
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
        menu.addAction(QIcon::fromTheme("rename"), tr("Rename"),
                       this, SLOT(slotTabRemoteRename()));
        menu.addAction(QIcon::fromTheme("edit-copy"), tr("Copy url to clipboard"),
                       this, SLOT(slotTabRemoteCopyToClipboard()));
    }
    menu.exec(ui->tabRemote->viewport()->mapToGlobal(pos));
}

void CFrmFileTransfer::slotTabRemoteDownload()
{
}

void CFrmFileTransfer::slotTabRemoteAddToList()
{
    QString szLocal = ui->cbLocal->currentText();
    if(szLocal.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please select local directory"));
        return;
    }
    auto indexes = ui->tabRemote->selectionModel()->selectedRows();
    foreach(auto idx, indexes) {
        auto p = m_pModelRemoteFile->GetRemoteFileSystemFromIndex(idx);
        QString szRemote = p->GetPath();
        if(szRemote.isEmpty()) {
            qDebug(log) << "The select is empty:" << idx;
            continue;
        }
        QString szLocalFile = szLocal;
        if(szLocalFile.right(1) == "/" || szLocalFile.right(1) == "\\")
            szLocalFile += p->GetName();
        else
            szLocalFile += QDir::separator() + p->GetName();
        QSharedPointer<CFileTransfer> f(
            new CFileTransfer(szLocalFile,
                              szRemote, CFileTransfer::Direction::Download));
        f->SetFileSize(p->GetSize());
        f->SetRemotePermission(p->GetPermissions());
        m_pListFileModel->AddFileTransfer(f);
    }
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
{
    auto idx = ui->tabRemote->currentIndex();
    if(idx.isValid()) {
        auto p = m_pModelRemoteFile->GetRemoteFileSystemFromIndex(idx);
        if(p) {
            QString szPath = p->GetPath();
            emit sigCopyUrlToClipboard(szPath);
        }
    }
}

void CFrmFileTransfer::on_tabList_customContextMenuRequested(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    auto idx = ui->tabList->currentIndex();
    QMenu menu;
    if(idx.isValid()) {
        auto f = m_pListFileModel->GetFileTransfer(idx);
        if(f) {
            if(((int)f->GetState() & (int)CFileTransfer::State::Process))
                menu.addAction(QIcon::fromTheme("media-playback-stop"),
                               tr("Stop"), this, SLOT(slotProcessFileTransfer()));
            else if(f->GetState() != CFileTransfer::State::Finish)
                menu.addAction(QIcon::fromTheme("media-playback-start"),
                               tr("Start"), this, SLOT(slotProcessFileTransfer()));
        }
        menu.addAction(QIcon::fromTheme("remove"), tr("Delete"),
                       this, SLOT(slotTabListDelete()));
    }
    menu.exec(ui->tabList->viewport()->mapToGlobal(pos));
}

void CFrmFileTransfer::slotTabListDelete()
{
    auto indexes = ui->tabList->selectionModel()->selectedRows();
    // 倒序排序
    std::sort(indexes.begin(), indexes.end(),
              [](const QModelIndex &a, const QModelIndex &b) {
                  return a.row() > b.row();
              });
    foreach(const QModelIndex &idx, indexes) {
        auto f = m_pListFileModel->GetFileTransfer(idx);
        if(f) {
            if((int)f->GetState() & (int)CFileTransfer::State::Process) {
                QMessageBox::critical(this, tr("Error"), tr("Please stop the file transfer first"));
                return;
            }
        }
        m_pListFileModel->removeRow(idx.row(), idx.parent());
    }
    return;
}

void CFrmFileTransfer::slotProcessFileTransfer()
{
    auto idx = ui->tabList->currentIndex();
    if(!idx.isValid()) return;
    auto f = m_pListFileModel->GetFileTransfer(idx);
    if(!f) return;
    if((int)f->GetState() & (int)CFileTransfer::State::Process) {
        emit sigStopFileTransfer(f);
    }
    if((int)f->GetState() & (int)CFileTransfer::State::CanStart) {
        f->slotSetstate(CFileTransfer::State::Opening);
        emit sigStartFileTransfer(f);
    }
}

void CFrmFileTransfer::slotFileTransferUpdate(QSharedPointer<CFileTransfer> f)
{
    if(!f) return;
    m_pListFileModel->UpdateFileTransfer(f);
}

int CFrmFileTransfer::Load(QSettings &set)
{
    m_pListFileModel->Load(set);
    return 0;
}

int CFrmFileTransfer::Save(QSettings &set)
{
    if(m_pListFileModel)
        m_pListFileModel->Save(set);
    return 0;
}
