// Author: Kang Lin <kl222@126.com>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QLoggingCategory>
#include <QJsonArray>

#include "BookmarkDatabase.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Bookmark.DB")

CBookmarkDatabase::CBookmarkDatabase(QObject *parent)
    : CDatabase(parent)
    , m_TreeDB("bookmarks")
{
    qDebug(log) << Q_FUNC_INFO;
}

CBookmarkDatabase::~CBookmarkDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
}

bool CBookmarkDatabase::OnInitializeDatabase()
{
    bool bRet = false;
    bRet = m_UrlDB.SetDatabase(GetDatabase(), m_pPara);
    if(!bRet) return false;
    bRet = m_TreeDB.SetDatabase(GetDatabase(), m_pPara);
    if(!bRet) return false;

    if(m_TreeDB.GetNodeCount() == 0) {
        m_TreeDB.AddNode(tr("Bookmarks"), 0);
        m_TreeDB.AddNode(tr("Other"), 0);
        m_TreeDB.AddNode(tr("Favorites"), 1);
        m_TreeDB.AddNode(tr("Frequently Used Websites"), 1);
    }
    return true;
}

int CBookmarkDatabase::addBookmark(const BookmarkItem &item)
{
    int urlId = m_UrlDB.AddUrl(item.url, item.title, item.icon);
    auto tree = BookmarkToTree(item);
    tree.SetKey(urlId);
    return m_TreeDB.Add(tree);
}

bool CBookmarkDatabase::updateBookmark(const BookmarkItem &item)
{
    bool success = m_UrlDB.UpdateUrl(item.url, item.title, item.icon);
    if(!success) {
        qCritical(log) << "Failed to update bookmark" << item.url;
        return false;
    }
    TreeItem tree = BookmarkToTree(item, true);
    success = m_TreeDB.Update(tree);
    if(!success) {
        qCritical(log) << "Failed to update bookmark" << item.url;
    }
    return success;
}

bool CBookmarkDatabase::deleteBookmark(int id)
{
    return m_TreeDB.Delete(id);
}

bool CBookmarkDatabase::deleteBookmark(const QList<BookmarkItem> &items)
{
    if(items.isEmpty()) {
        qWarning(log) << "The items is empty";
        return false;
    }
    QList<int> lstItems;
    foreach(auto item, items) {
        lstItems.push_back(item.id);
    }
    return m_TreeDB.Delete(lstItems);
}

bool CBookmarkDatabase::moveBookmark(int id, int newFolderId)
{
    return m_TreeDB.Move(id, newFolderId);
}

int CBookmarkDatabase::addFolder(const QString &name, int parentId)
{
    return m_TreeDB.AddNode(name, parentId);
}

bool CBookmarkDatabase::renameFolder(int folderId, const QString &newName)
{
    return m_TreeDB.RenameNode(folderId, newName);
}

bool CBookmarkDatabase::deleteFolder(int folderId)
{
    return m_TreeDB.DeleteNode(folderId);
}

bool CBookmarkDatabase::moveFolder(int folderId, int newParentId)
{
    return m_TreeDB.MoveNode(folderId, newParentId);
}

BookmarkItem CBookmarkDatabase::getBookmark(int id)
{
    BookmarkItem item(BookmarkType_Bookmark);

    auto leaf = m_TreeDB.GetLeaf(id);
    if(leaf.GetId() == 0) return item;
    item = TreeToBookmark(leaf);
    return item;
}

QList<BookmarkItem> CBookmarkDatabase::getBookmarkByUrl(const QString &url)
{
    QList<BookmarkItem> lstItems;
    BookmarkItem item(BookmarkType_Bookmark);

    auto urlItem = m_UrlDB.GetItem(url);
    auto leaves = m_TreeDB.GetLeavesByKey(urlItem.id);
    foreach(auto leaf, leaves) {
        item = TreeToBookmark(leaf, urlItem);
        lstItems.push_back(item);
    }
    return lstItems;
}

QList<BookmarkItem> CBookmarkDatabase::getAllBookmarks(int folderId)
{
    QList<BookmarkItem> bookmarks;

    auto leaves = m_TreeDB.GetLeaves(folderId);
    foreach(auto leaf, leaves) {
        auto item = TreeToBookmark(leaf);
        bookmarks << item;
    }
    return bookmarks;
}

QList<BookmarkItem> CBookmarkDatabase::searchBookmarks(const QString &keyword)
{
    QList<BookmarkItem> bookmarks;

    auto urlIds = m_UrlDB.Search(keyword);
    foreach(auto urlItem, urlIds) {
        if(0 == urlItem.id)
            continue;
        auto trees = m_TreeDB.GetLeavesByKey(urlItem.id);
        foreach(auto t, trees) {
            auto item = TreeToBookmark(t, urlItem);
            if(0 == item.id)
                continue;
            bookmarks << item;
        }
    }

    return bookmarks;
}

QList<BookmarkItem> CBookmarkDatabase::getAllFolders()
{
    QList<BookmarkItem> folders;

    auto items = m_TreeDB.GetAllNodes();
    foreach(auto it, items) {
        auto book = TreeToBookmark(it);
        folders << book;
    }
    return folders;
}

QList<BookmarkItem> CBookmarkDatabase::getSubFolders(int folderId)
{
    QList<BookmarkItem> folders;

    auto items = m_TreeDB.GetSubNodes(folderId);
    foreach(auto it, items) {
        auto book = TreeToBookmark(it);
        folders << book;
    }
    return folders;
}

TreeItem CBookmarkDatabase::BookmarkToTree(const BookmarkItem& tree, bool setKey)
{
    TreeItem::TYPE type;
    type = tree.type == BookmarkType_Folder ? TreeItem::Node: TreeItem::Leaf;
    TreeItem item(type);   
    item.SetId(tree.id);
    item.SetParentId(tree.folderId);
    item.SetCreateTime(tree.createdTime);
    item.SetModifyTime(tree.modifiedTime);
    item.SetLastVisitTime(tree.lastVisitTime);
    if(setKey && BookmarkType_Bookmark == tree.type && !tree.url.isEmpty()) {
        int id = m_UrlDB.GetId(tree.url);
        item.SetKey(id);
    }
    return item;
}

BookmarkItem CBookmarkDatabase::TreeToBookmark(const TreeItem& tree)
{
    BookmarkItem item(BookmarkType_Bookmark);
    CDatabaseUrl::UrlItem url = m_UrlDB.GetItem(tree.GetKey());
    item = TreeToBookmark(tree, url);
    return item;
}

BookmarkItem CBookmarkDatabase::TreeToBookmark(
    const TreeItem& tree, const CDatabaseUrl::UrlItem& url)
{
    BookmarkItem item(BookmarkType_Bookmark);
    item.id = tree.GetId();
    item.folderId = tree.GetParentId();
    item.createdTime = tree.GetCreateTime();
    item.modifiedTime = tree.GetModifyTime();
    item.lastVisitTime = tree.GetLastVisitTime();

    if(tree.IsNode()) {
        item.type = BookmarkType_Folder;
        item.title = tree.GetName();
    } else {
        item.type = BookmarkType_Bookmark;
        item.title = url.szTitle;
        item.url = url.szUrl;
        item.icon = url.icon;
    }
    return item;
}

bool CBookmarkDatabase::ExportToJson(QJsonObject &obj)
{
    bool bRet = true;
    QJsonArray root;
    bRet = ExportToJson(0, root);
    if(bRet)
        obj.insert("browser_bookmark", root);
    return bRet;
}

bool CBookmarkDatabase::ImportFromJson(const QJsonObject &obj)
{
    bool bRet = false;
    QJsonArray bookmark = obj["browser_bookmark"].toArray();
    if(bookmark.isEmpty()) {
        SetError(tr("The file format is error. Json without \"browser_bookmark\""));
        qCritical(log) << GetError();
        return false;
    }

    bRet = ImportFromJson(0, bookmark);
    return bRet;
}

bool CBookmarkDatabase::ImportFromJson(int parentId, const QJsonArray &obj)
{
    for(auto it = obj.begin(); it != obj.end(); it++) {
        QJsonObject itemObj = it->toObject();
        if(itemObj.isEmpty()) continue;
        if(BookmarkType::BookmarkType_Folder == itemObj["type"].toInt()) {
            QString szName = itemObj["name"].toString();
            int id = addFolder(szName, parentId);
            QJsonArray items = itemObj[szName].toArray();
            if(items.isEmpty()) continue;
            ImportFromJson(id, items);
            continue;
        }

        BookmarkItem item(BookmarkType::BookmarkType_Bookmark);
        item.title = itemObj["title"].toString();
        item.url = itemObj["url"].toString();
        item.folderId = parentId;

        QIcon icon;
        bool bRet = CDatabaseIcon::ImportIconFromJson(itemObj, icon);
        if(!bRet) continue;
        item.icon =icon;

        addBookmark(item);
    }
    
    return true;
}

bool CBookmarkDatabase::ExportToJson(int parentId, QJsonArray &obj)
{
    auto items = getSubFolders(parentId);
    foreach(auto item, items) {
        QJsonObject oItem;
        if(item.isFolder()) {
            QJsonArray aItem;
            bool bRet = ExportToJson(item.id, aItem);
            if(!bRet) continue;

            oItem.insert("type", item.type);
            oItem.insert("name", item.title);
            oItem.insert(item.title, aItem);
        } else {
            Q_ASSERT_X(false, "ExportBookmark", "The item is not node");
            continue;
        }
        
        obj.append(oItem);    
    }

    items = getAllBookmarks(parentId);
    foreach(auto item, items) {
        QJsonObject oItem;
        if(item.isBookmark()) {
            oItem.insert("type", item.type);
            oItem.insert("title", item.title);
            oItem.insert("url", item.url);
            // Icon
            bool bRet = CDatabaseIcon::ExportIconToJson(item.getIcon(), oItem);
            if(!bRet) continue;
        } else {
            Q_ASSERT_X(false, "ExportBookmark", "The item is not bookmark");
            continue;
        }
        obj.append(oItem);    
    }
    return true;
}
