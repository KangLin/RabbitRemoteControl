// Author: Kang Lin <kl222@126.com>

#include <QMenu>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QLoggingCategory>

#include "FrmBookmark.h"
#include "ui_FrmBookmark.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Bookmark")
CFrmBookmark::CFrmBookmark(CBookmarkDatabase *db, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmBookmark)
    , m_pDatabase(db)
    , m_pTreeView(nullptr)
    , m_pSearchEdit(nullptr)
    , m_pToolBar(nullptr)
    , m_pModel(new QStandardItemModel(this))
{
    ui->setupUi(this);
    setWindowTitle(tr("Bookmark"));

    setupUI();
    loadBookmarks();
}

CFrmBookmark::~CFrmBookmark()
{
    delete ui;
}

void CFrmBookmark::setupUI()
{
    bool check = false;
    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
    if(!pMainLayout) return;

    // 工具栏
    setupToolBar();
    pMainLayout->addWidget(m_pToolBar);

    // 搜索框
    QHBoxLayout *pSearchLayout = new QHBoxLayout;
    if(pSearchLayout) {
        pSearchLayout->addWidget(new QLabel(tr("Search:")));

        m_pSearchEdit = new QLineEdit(this);
        if(m_pSearchEdit) {
            m_pSearchEdit->setPlaceholderText(tr("Input keyword to search boolmark ......"));
            m_pSearchEdit->setClearButtonEnabled(true);
            check = connect(m_pSearchEdit, &QLineEdit::textChanged,
                    this, &CFrmBookmark::onSearchTextChanged);
            Q_ASSERT(check);
            pSearchLayout->addWidget(m_pSearchEdit);
        }
        pMainLayout->addLayout(pSearchLayout);
    }

    // 书签树
    setupTreeView();
    pMainLayout->addWidget(m_pTreeView);

    // 按钮
    QHBoxLayout *pButtonLayout = new QHBoxLayout;
    if(pButtonLayout) {
        pButtonLayout->addStretch();

        QPushButton *closeButton = new QPushButton(tr("Close"), this);
        connect(closeButton, &QPushButton::clicked, this, &CFrmBookmark::close);
        pButtonLayout->addWidget(closeButton);

        pMainLayout->addLayout(pButtonLayout);
    }
    setLayout(pMainLayout);
}

void CFrmBookmark::setupToolBar()
{
    bool check = false;
    m_pToolBar = new QToolBar(this);
    if(!m_pToolBar) return;

    // 添加书签
    QAction *addAction = m_pToolBar->addAction(QIcon::fromTheme("add"), tr("Add bookmark"));
    if(addAction) {
        check = connect(addAction, &QAction::triggered, this, &CFrmBookmark::onAddBookmark);
        Q_ASSERT(check);
    }

    // 添加文件夹
    QAction *addFolderAction = m_pToolBar->addAction(QIcon::fromTheme("folder-new"), tr("Add folder"));
    if(addFolderAction) {
        check = connect(addFolderAction, &QAction::triggered, this, &CFrmBookmark::onAddFolder);
        Q_ASSERT(check);
    }

    m_pToolBar->addSeparator();

    // 编辑
    QAction *editAction = m_pToolBar->addAction(QIcon::fromTheme("edit"), tr("Edit"));
    if(editAction) {
        check = connect(editAction, &QAction::triggered, this, &CFrmBookmark::onEditBookmark);
        Q_ASSERT(check);
    }

    // 删除
    QAction *pDeleteAction = m_pToolBar->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"));
    if(pDeleteAction) {
        check = connect(pDeleteAction, &QAction::triggered, this, &CFrmBookmark::onDeleteBookmark);
        Q_ASSERT(check);
    }

    // 设为最爱
    QAction *favoriteAction = m_pToolBar->addAction(QIcon::fromTheme("favorites"), tr("Favorite"));
    if(favoriteAction) {
        check = connect(favoriteAction, &QAction::triggered, this, &CFrmBookmark::onSetFavorite);
        Q_ASSERT(check);
    }

    m_pToolBar->addSeparator();

    // 导入
    QAction *importAction = m_pToolBar->addAction(QIcon::fromTheme("import"), tr("Import"));
    if(importAction) {
        check = connect(importAction, &QAction::triggered, this, &CFrmBookmark::onImportBookmarks);
        Q_ASSERT(check);
    }

    // 导出
    QAction *exportAction = m_pToolBar->addAction(QIcon::fromTheme("export"), tr("Export"));
    if(exportAction) {
        check = connect(exportAction, &QAction::triggered, this, &CFrmBookmark::onExportBookmarks);
        Q_ASSERT(check);
    }

    // 刷新
    QAction *refreshAction = m_pToolBar->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"));
    if(refreshAction) {
        check = connect(refreshAction, &QAction::triggered, this, &CFrmBookmark::refresh);
        Q_ASSERT(check);
    }
}

void CFrmBookmark::setupTreeView()
{
    bool check = false;
    m_pTreeView = new QTreeView(this);
    if(!m_pTreeView) return;
    m_pTreeView->setHeaderHidden(true);
    m_pTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 连接信号
    check = connect(m_pTreeView, &QTreeView::doubleClicked,
            this, &CFrmBookmark::onTreeViewDoubleClicked);
    Q_ASSERT(check);
    check = connect(m_pTreeView, &QTreeView::customContextMenuRequested,
            this, &CFrmBookmark::onCustomContextMenu);
    Q_ASSERT(check);

    if(m_pModel) {
        m_pTreeView->setModel(m_pModel);
        // 设置列
        m_pModel->setColumnCount(1);
    }
    if(m_pTreeView->header())
        m_pTreeView->header()->setStretchLastSection(true);  // 最后一列自适应
}

void CFrmBookmark::loadBookmarks()
{
    if(!m_pDatabase || !m_pModel) return;
    m_pModel->clear();
    m_folderItems.clear();

    // 设置表头
    m_pModel->setHorizontalHeaderLabels(QStringList() << tr("Title"));

    // 加载文件夹结构
    QList<BookmarkItem> folders = m_pDatabase->getAllFolders();

    // 创建根节点
    QStandardItem *rootItem = m_pModel->invisibleRootItem();

    // 先添加顶级文件夹
    for (const auto &folder : folders) {
        if (folder.folderId == 0) {  // 顶级文件夹
            QStandardItem *pFolderItem = new QStandardItem(folder.getIcon(), folder.title);
            if(!pFolderItem) continue;
            pFolderItem->setData(folder.id, ID);
            pFolderItem->setData(BookmarkType_Folder, Type);
            rootItem->appendRow(pFolderItem);
            m_folderItems[folder.id] = pFolderItem;
            continue;
        }

        auto it = m_folderItems.find(folder.folderId);
        if(m_folderItems.end() == it) {
            qWarning(log) << "The parent of folder is not find:" << folder.folderId;
            continue;
        }
        QStandardItem *pFolderItem = new QStandardItem(folder.getIcon(), folder.title);
        if(!pFolderItem) continue;
        pFolderItem->setData(folder.id, ID);
        pFolderItem->setData(BookmarkType_Folder, Type);
        (*it)->appendRow(pFolderItem);
        m_folderItems[folder.id] = pFolderItem;
    }

    // 添加书签到对应的文件夹
    QList<BookmarkItem> bookmarks = m_pDatabase->getAllBookmarks(-1);
    for (const auto &bookmark : bookmarks) {
        QStandardItem *pParentItem = nullptr;

        if (bookmark.folderId > 0 && m_folderItems.contains(bookmark.folderId)) {
            pParentItem = m_folderItems[bookmark.folderId];
        } else {
            pParentItem = rootItem;
        }

        QStandardItem *bookmarkItem = new QStandardItem(bookmark.getIcon(), bookmark.title);
        bookmarkItem->setData(bookmark.id, ID);
        bookmarkItem->setData(BookmarkType_Bookmark, Type);
        bookmarkItem->setData(bookmark.url, Url);

        if (bookmark.isFavorite) {
            bookmarkItem->setIcon(QIcon::fromTheme("favorites"));
        }

        if(pParentItem)
            pParentItem->appendRow(bookmarkItem);
    }

    // 展开所有节点
    m_pTreeView->expandAll();
}

void CFrmBookmark::onAddBookmark()
{
    bool ok;
    QString url = QInputDialog::getText(this, tr("Add bookmark"),
                                        tr("Url:"), QLineEdit::Normal,
                                        "http://", &ok);
    if (!ok || url.isEmpty()) return;

    QString title = QInputDialog::getText(this, tr("Add bookmark"),
                                          tr("Title:"), QLineEdit::Normal,
                                          "", &ok);
    if (!ok) return;

    BookmarkItem item;
    item.url = url;
    item.title = title.isEmpty() ? url : title;
    item.createdTime = QDateTime::currentDateTime();

    // 获取选中的文件夹
    QModelIndex currentIndex = m_pTreeView->currentIndex();
    if (currentIndex.isValid()) {
        int type = currentIndex.data(Type).toInt();
        if (BookmarkType_Folder == type) {
            item.folderId = currentIndex.data(Qt::UserRole).toInt();
        }
    }

    if (m_pDatabase->addBookmark(item)) {
        refresh();
    }
}

void CFrmBookmark::onAddFolder()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add folder"),
                                         tr("Folder name:"), QLineEdit::Normal,
                                         "", &ok);
    if (!ok || name.isEmpty()) return;

    int parentId = 0;
    QModelIndex currentIndex = m_pTreeView->currentIndex();
    if (currentIndex.isValid()) {
        int type = currentIndex.data(Type).toInt();
        if (BookmarkType_Folder == type) {
            parentId = currentIndex.data(Qt::UserRole).toInt();
        }
    }

    if (m_pDatabase->addFolder(name, parentId)) {
        refresh();
    }
}

void CFrmBookmark::onEditBookmark()
{
    QModelIndex index = m_pTreeView->currentIndex();
    if (!index.isValid()) return;

    int type = index.data(Type).toInt();
    int id = index.data(Qt::UserRole).toInt();

    if (BookmarkType_Bookmark == type) {
        BookmarkItem item = m_pDatabase->getBookmark(id);
        if (item.id == 0) return;

        bool ok;
        QString title = QInputDialog::getText(this, tr("Edit bookmark"),
                                              tr("Title:"), QLineEdit::Normal,
                                              item.title, &ok);
        if (!ok) return;

        QString url = QInputDialog::getText(this, tr("Add bookmark"),
                                            tr("Url:"), QLineEdit::Normal,
                                            item.url, &ok);
        if (!ok) return;

        item.title = title;
        item.url = url;

        if (m_pDatabase->updateBookmark(item)) {
            refresh();
        }
    } else if (BookmarkType_Folder == type) {
        QString oldName = index.data(Qt::DisplayRole).toString();

        bool ok;
        QString newName = QInputDialog::getText(this, tr("Rename folder"),
                                                tr("Folder name:"), QLineEdit::Normal,
                                                oldName, &ok);
        if (!ok || newName.isEmpty()) return;

        if (m_pDatabase->renameFolder(id, newName)) {
            refresh();
        }
    }
}

void CFrmBookmark::onDeleteBookmark()
{
    QModelIndex index = m_pTreeView->currentIndex();
    if (!index.isValid()) return;

    int type = index.data(Type).toInt();
    QString name = index.data(Qt::DisplayRole).toString();
    int id = index.data(Qt::UserRole).toInt();
    if(1 == id && BookmarkType_Folder == type) {
        QMessageBox::warning(this, tr("Warning"), tr("The folder \"%1\" is not delete").arg(name));
        return;
    }

    QString message;
    if (BookmarkType_Bookmark == type) {
        message = tr("Are you sure you want to delete the bookmark \"%1\"?").arg(name);
    } else if (BookmarkType_Folder == type) {
        message = tr("Are you sure you want to delete the folder \"%1\"?\n"
                     "The bookmarks inside the folder will be moved to the root directory.").arg(name);
    } else {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Confirm deletion"), message,
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (BookmarkType_Bookmark == type) {
            m_pDatabase->deleteBookmark(id);
        } else if (BookmarkType_Folder == type) {
            m_pDatabase->deleteFolder(id);
        }
        refresh();
    }
}

void CFrmBookmark::onSetFavorite()
{
    QModelIndex index = m_pTreeView->currentIndex();
    if (!index.isValid()) return;

    int type = index.data(Type).toInt();
    if (type != BookmarkType_Bookmark) return;

    int id = index.data(ID).toInt();
    BookmarkItem item = m_pDatabase->getBookmark(id);
    if (item.id == 0) return;

    item.isFavorite = !item.isFavorite;

    if (m_pDatabase->updateBookmark(item)) {
        refresh();
    }
}

void CFrmBookmark::onImportBookmarks()
{
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Import bookmarks"), QString(), tr("HTML(*.html);; All files (*.*)"));

    if (!filename.isEmpty()) {
        if (m_pDatabase->importFromHtml(filename)) {
            QMessageBox::information(this, tr("Success"), tr("Bookmarks imported successfully"));
            refresh();
        } else {
            QMessageBox::warning(this, tr("Failure"), tr("Failed to import bookmark"));
        }
    }
}

void CFrmBookmark::onExportBookmarks()
{
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Export bookmarks"), QString(), tr("HTML (*.html);; All files (*.*)"));

    if (!filename.isEmpty()) {
        if (m_pDatabase->exportToHtml(filename)) {
            QMessageBox::information(this, tr("Success"), tr("Bookmarks exported successfully"));
        } else {
            QMessageBox::warning(this, tr("Failure"), tr("Failed to export bookmark"));
        }
    }
}

void CFrmBookmark::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        loadBookmarks();
        return;
    }

    if(!m_pModel) return;

    m_pModel->clear();
    m_folderItems.clear();

    QList<BookmarkItem> bookmarks = m_pDatabase->searchBookmarks(text);
    QStandardItem *pRootItem = m_pModel->invisibleRootItem();
    if(!pRootItem) return;

    for (const auto &bookmark : bookmarks) {
        QStandardItem *item = new QStandardItem(bookmark.getIcon(), bookmark.title);
        item->setData(bookmark.id, ID);
        item->setData(BookmarkType_Bookmark, Type);
        item->setData(bookmark.url, Url);

        // 显示路径
        QString path = bookmark.title;
        if (bookmark.folderId > 0) {
            BookmarkItem folder = m_pDatabase->getBookmark(bookmark.folderId);
            if (folder.id > 0) {
                path = QString("%1 / %2").arg(folder.title).arg(bookmark.title);
            }
        }

        item->setText(path);

        pRootItem->appendRow(item);
    }
}

void CFrmBookmark::onTreeViewDoubleClicked(const QModelIndex &index)
{
    int type = index.data(Type).toInt();
    if (BookmarkType_Bookmark == type) {
        QString url = index.data(Url).toString();
        if (!url.isEmpty()) {
            emit openUrlRequested(url);
            close();  // 关闭对话框
        }
    }
}

void CFrmBookmark::onCustomContextMenu(const QPoint &pos)
{
    if(!m_pTreeView) return;
    QModelIndex index = m_pTreeView->indexAt(pos);
    if (!index.isValid()) return;

    QMenu menu(this);

    int id = index.data(ID).toInt();
    int type = index.data(Type).toInt();

    if (BookmarkType_Bookmark == type) {
        menu.addAction(QIcon::fromTheme("document-open"), tr("Open"), this, [this, index]() {
            QString url = index.data(Url).toString();
            if (!url.isEmpty()) {
                emit openUrlRequested(url);
            }
        });

        menu.addAction(QIcon::fromTheme("edit"), tr("Edit"), this, &CFrmBookmark::onEditBookmark);
        menu.addSeparator();

        QAction *favoriteAction = menu.addAction(QIcon::fromTheme("favorites"), tr("Favorite"));
        connect(favoriteAction, &QAction::triggered, this, &CFrmBookmark::onSetFavorite);

        menu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"), this, &CFrmBookmark::onDeleteBookmark);
    } else if (BookmarkType_Folder == type) {
        menu.addAction(QIcon::fromTheme("Add"), tr("Add bookmark"), this, &CFrmBookmark::onAddBookmark);
        menu.addAction(QIcon::fromTheme("folder-new"), tr("Add folder"), this, &CFrmBookmark::onAddFolder);
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme("edit"), tr("Edit"), this, &CFrmBookmark::onEditBookmark);
        if(1 != id)
            menu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"), this, &CFrmBookmark::onDeleteBookmark);
    }

    menu.exec(m_pTreeView->viewport()->mapToGlobal(pos));
}

void CFrmBookmark::refresh()
{
    loadBookmarks();
}
