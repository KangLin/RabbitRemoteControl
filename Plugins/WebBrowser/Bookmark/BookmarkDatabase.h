// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDomElement>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QIcon>
#include <QList>
#include <QDateTime>
#include "DatabaseTree.h"
#include "DatabaseUrl.h"

enum BookmarkType {
    BookmarkType_Bookmark,
    BookmarkType_Folder,
    BookmarkType_Separator
};

struct BookmarkItem {
    int id;
    QString url;
    QString title;
    QIcon icon;
    QDateTime createdTime;
    QDateTime modifiedTime;
    QDateTime lastVisitTime;

    int folderId;
    BookmarkType type;

    BookmarkItem()
        : id(0)
        , folderId(1) // 指向默认收藏夹
        , type(BookmarkType_Bookmark)
    {}

    bool isFolder() const { return type == BookmarkType_Folder; }
    bool isSeparator() const { return type == BookmarkType_Separator; }
    bool isBookmark() const { return type == BookmarkType_Bookmark; }

    QIcon getIcon() const {
        if (!icon.isNull()) {
            return icon;
        }

        // 根据URL返回默认图标
        if (url.startsWith("https://")) {
            return QIcon::fromTheme("security-high");
        } else if (url.startsWith("http://")) {
            return QIcon::fromTheme("security-low");
        } else if (isFolder()) {
            return QIcon::fromTheme("folder");
        }

        return QIcon::fromTheme("user-bookmarks");
    }

};

class CBookmarkDatabase : public CDatabase
{
    Q_OBJECT
public:
    static CBookmarkDatabase* Instance(const QSqlDatabase& database);
    static CBookmarkDatabase* Instance(const QString& szFile = QString());

    // 书签操作
    int addBookmark(const BookmarkItem &item);
    bool updateBookmark(const BookmarkItem &item);
    bool deleteBookmark(int id);
    bool deleteBookmark(const QList<BookmarkItem>& items);
    bool moveBookmark(int id, int newFolderId);

    // 文件夹操作
    int addFolder(const QString &name, int parentId = 0);
    bool renameFolder(int folderId, const QString &newName);
    bool deleteFolder(int folderId);
    bool moveFolder(int folderId, int newParentId);

    // 查询操作
    BookmarkItem getBookmark(int id);
    QList<BookmarkItem> getBookmarkByUrl(const QString &url);
    QList<BookmarkItem> getAllBookmarks(int folderId = 0);
    QList<BookmarkItem> searchBookmarks(const QString &keyword);

    // 文件夹查询
    QList<BookmarkItem> getAllFolders();
    QList<BookmarkItem> getSubFolders(int folderId);

    // 导入导出
    bool importFromHtml(const QString &filename);
    bool exportToHtml(const QString &filename);
    bool importFromBrowser(const QString &browserName);

    TreeItem BookmarkToTree(const BookmarkItem& tree, bool setKey = false);
    BookmarkItem TreeToBookmark(const TreeItem& tree);
    BookmarkItem TreeToBookmark(const TreeItem& item, const CDatabaseUrl::UrlItem& url);

signals:
    void bookmarksChanged();
    void bookmarkAdded(const BookmarkItem &item);
    void bookmarkUpdated(const BookmarkItem &item);
    void bookmarkDeleted(int id);
    void folderAdded(const BookmarkItem &folder);
    void folderUpdated(const BookmarkItem &folder);
    void folderDeleted(int folderId);

private:
    explicit CBookmarkDatabase(QObject *parent = nullptr);
    ~CBookmarkDatabase();
    bool OnInitializeDatabase() override;

    void buildBookmarkDocument(QDomDocument &doc);
    void buildBookmarkTree(
        QDomDocument &doc, QDomElement &parentElement, int folderId);
    QDomElement createBookmarkDomElement(
        QDomDocument &doc, const BookmarkItem &bookmark);
    int parseHtmlBookmarks(const QString &htmlContent);
    int parseBookmarkList(const QDomElement &dlElement,
                          const QString &currentPath,
                          QMap<QString, int> &folderMap);
    int parseDtElement(const QDomElement &dtElement,
                       const QString &currentPath,
                       QMap<QString, int> &folderMap);
    int importBookmark(const QDomElement &aElement,
                       const QString &folderPath, QMap<QString, int> &folderMap);
    QString importFolder(const QDomElement &h3Element,
                         const QString &parentPath,
                         QMap<QString, int> &folderMap);
    int getOrCreateFolder(const QString &folderPath, int parentFolderId);
    int getOrCreateFolder(const QString &folderPath, QMap<QString, int> &folderMap);
    QDateTime parseTimestamp(const QString &timestampStr);
    QDomElement findFirstElement(const QDomElement &parent, const QString &tagName);

    CDatabaseTree m_TreeDB;
    CDatabaseUrl m_UrlDB;
};

