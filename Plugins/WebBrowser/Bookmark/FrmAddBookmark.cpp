// Author: Kang Lin <kl222@126.com>

#include <QMenu>
#include <QLoggingCategory>
#include <QInputDialog>
#include "FrmAddBookmark.h"
#include "ui_FrmAddBookmark.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Bookmark.Add")
CFrmAddBookmark::CFrmAddBookmark(const QString &szTitle, const QUrl &url,
                                 const QIcon &icon, CParameterWebBrowser* pPara,
                                 CBookmarkDatabase *db, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmAddBookmark)
    , m_szTitle(szTitle)
    , m_Url(url)
    , m_Icon(icon)
    , m_pPara(pPara)
    , m_pDatabase(db)
    , m_pModelTree(nullptr)
{
    ui->setupUi(this);
    ui->pbDelete->setVisible(false);

    setWindowIcon(QIcon::fromTheme("user-bookmarks"));

    if(m_szTitle.isEmpty())
        m_szTitle = m_Url.toString();

    ui->leTitle->setText(m_szTitle);

    if(m_pPara) {
        resize(m_pPara->GetWindowSize());
        ui->cbSave->setChecked(m_pPara->GetBookmarkShowEditor());
    }

    ui->tvFolder->setHeaderHidden(true);
    ui->tvFolder->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tvFolder->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pModelTree = new QStandardItemModel(ui->tvFolder);
    if(m_pModelTree) {
        ui->tvFolder->setModel(m_pModelTree);
        m_pModelTree->setColumnCount(1);
    }

    bool check = false;
    check = connect(ui->tvFolder, SIGNAL(customContextMenuRequested(QPoint)),
                    this, SLOT(onCustomContextMenu(QPoint)));
    Q_ASSERT(check);
    check = connect(m_pDatabase, &CBookmarkDatabase::folderAdded,
                         this, [&](const BookmarkItem &folder){
        loadFolder(folder.id);
    });
    Q_ASSERT(check);
    loadFolder();
}

CFrmAddBookmark::~CFrmAddBookmark()
{
    delete ui;
}

void CFrmAddBookmark::on_pbAdd_clicked()
{
    if(!m_pPara || !m_pDatabase) return;

    m_pPara->SetBookmarkShowEditor(ui->cbSave->isChecked());

    auto index = ui->tvFolder->currentIndex();
    if(index.isValid()) {
        int id = index.data((int)Role::ID).toInt();
        m_pPara->SetBookmarkCurrentFolder(id);
        auto items = m_pDatabase->getBookmarkByUrl(m_Url.toString());
        if(!items.isEmpty()) {
            auto item = items[0];
            item.title = ui->leTitle->text();
            item.url = m_Url.toString();
            item.folderId = id;
            m_pDatabase->updateBookmark(item);
        } else {
            BookmarkItem item;
            item.title = ui->leTitle->text();
            item.url = m_Url.toString();
            item.folderId = id;
            m_pDatabase->addBookmark(item);
        }
    }

    close();
}

void CFrmAddBookmark::on_pbCancel_clicked()
{
    close();
}

void CFrmAddBookmark::on_pbDelete_clicked()
{
    auto items = m_pDatabase->getBookmarkByUrl(m_Url.toString());
    m_pDatabase->deleteBookmark(items);
    ui->pbDelete->setVisible(false);
    loadFolder();
}

void CFrmAddBookmark::loadFolder(int nCurrent)
{
    if(!m_pDatabase || !m_pModelTree || !m_pPara) return;

    m_pModelTree->clear();
    m_folderItems.clear();

    // 设置表头
    m_pModelTree->setHorizontalHeaderLabels(QStringList() << tr("Title"));

    QStandardItem* pCurrentItem = nullptr;
    // 加载文件夹结构
    QList<BookmarkItem> folders = m_pDatabase->getAllFolders();

    // 创建根节点
    QStandardItem *rootItem = m_pModelTree->invisibleRootItem();

    if(0 == nCurrent) {
        nCurrent = m_pPara->GetBookmarkCurrentFolder();
        auto item = m_pDatabase->getBookmarkByUrl(m_Url.toString());
        if(!item.isEmpty()) {
            nCurrent = item[0].folderId;
            ui->pbDelete->setVisible(true);
            ui->pbDelete->setText(tr("Delete %1 bookmarks").arg(item.count()));
        }
    }

    // 先添加顶级文件夹
    for (const auto &folder : folders) {
        if (folder.folderId == 0) {  // 顶级文件夹
            QStandardItem *pFolderItem = new QStandardItem(folder.getIcon(), folder.title);
            if(!pFolderItem) continue;
            pFolderItem->setData(folder.id, (int)Role::ID);
            pFolderItem->setData(BookmarkType_Folder, (int)Role::Type);
            rootItem->appendRow(pFolderItem);
            m_folderItems[folder.id] = pFolderItem;
            if(nCurrent == folder.id)
                pCurrentItem = pFolderItem;
            continue;
        }

        auto it = m_folderItems.find(folder.folderId);
        if(m_folderItems.end() == it) {
            qWarning(log) << "The parent of folder is not find:" << folder.folderId;
            continue;
        }
        QStandardItem *pFolderItem = new QStandardItem(folder.getIcon(), folder.title);
        if(!pFolderItem) continue;
        pFolderItem->setData(folder.id, (int)Role::ID);
        pFolderItem->setData(BookmarkType_Folder, (int)Role::Type);
        (*it)->appendRow(pFolderItem);
        m_folderItems[folder.id] = pFolderItem;
        if(nCurrent == folder.id)
            pCurrentItem = pFolderItem;
    }

    // Set current index
    if(pCurrentItem) {
        auto index = m_pModelTree->indexFromItem(pCurrentItem);
        ui->tvFolder->setCurrentIndex(index);
    }

    ui->tvFolder->expandAll();
}

void CFrmAddBookmark::on_pbNewFolder_clicked()
{
    auto index = ui->tvFolder->currentIndex();
    if(!index.isValid()) return;
    QString szParent = index.data().toString();
    int parentId = index.data((int)Role::ID).toInt();
    QString szFolder = QInputDialog::getText(
        this, tr("New folder:"), tr("Create new folder in folder \"%1\"").arg(szParent));
    if(szFolder.isEmpty()) return;
    m_pDatabase->addFolder(szFolder, parentId);
}

void CFrmAddBookmark::onCustomContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->tvFolder->indexAt(pos);
    if (!index.isValid()) return;

    QMenu menu(this);

    menu.addAction(QIcon::fromTheme("folder-new"), tr("Add folder"),
                   this, &CFrmAddBookmark::on_pbNewFolder_clicked);

    menu.exec(ui->tvFolder->viewport()->mapToGlobal(pos));
}
