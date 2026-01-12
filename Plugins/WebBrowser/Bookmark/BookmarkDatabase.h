// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QIcon>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

enum BookmarkType {
    BookmarkType_Bookmark,
    BookmarkType_Folder,
    BookmarkType_Separator
};

struct BookmarkItem {
    int id;
    QString url;
    QString title;
    QByteArray iconData;
    QString iconUrl;
    QString description;
    QDateTime createdTime;
    QDateTime modifiedTime;
    QDateTime lastVisitTime;
    int visitCount;
    bool isFavorite;
    QStringList tags;
    int folderId;
    BookmarkType type;

    // 运行时属性
    mutable QIcon cachedIcon;
    QList<BookmarkItem> children;

    BookmarkItem()
        : id(0)
        , visitCount(0)
        , isFavorite(false)
        , folderId(1) // 指向默认收藏夹
        , type(BookmarkType_Bookmark)
    {}

    bool isFolder() const { return type == BookmarkType_Folder; }
    bool isSeparator() const { return type == BookmarkType_Separator; }
    bool isBookmark() const { return type == BookmarkType_Bookmark; }

    QIcon getIcon() const {
        if (!cachedIcon.isNull()) {
            return cachedIcon;
        }

        if (!iconData.isEmpty()) {
            QPixmap pixmap;
            pixmap.loadFromData(iconData);
            if (!pixmap.isNull()) {
                cachedIcon = QIcon(pixmap);
                return cachedIcon;
            }
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

    void setIconFromData(const QByteArray &data) {
        iconData = data;
        cachedIcon = QIcon();  // 清除缓存
    }
};

class CBookmarkDatabase : public QObject
{
    Q_OBJECT
public:
    explicit CBookmarkDatabase(QObject *parent = nullptr);
    ~CBookmarkDatabase();

    bool openDatabase(const QString &dbPath = QString());
    void closeDatabase();

    // 书签操作
    bool addBookmark(const BookmarkItem &item);
    bool updateBookmark(const BookmarkItem &item);
    bool deleteBookmark(int id);
    bool moveBookmark(int id, int newFolderId);

    // 文件夹操作
    bool addFolder(const QString &name, int parentId = 0);
    bool renameFolder(int folderId, const QString &newName);
    bool deleteFolder(int folderId);
    bool moveFolder(int folderId, int newParentId);

    // 查询操作
    BookmarkItem getBookmark(int id);
    BookmarkItem getBookmarkByUrl(const QString &url);
    QList<BookmarkItem> getAllBookmarks(int folderId = 0);
    QList<BookmarkItem> getFavoriteBookmarks();
    QList<BookmarkItem> searchBookmarks(const QString &keyword);

    // 文件夹查询
    QList<BookmarkItem> getAllFolders();
    QList<BookmarkItem> getSubFolders(int parentId);

    // 统计
    int getBookmarkCount(int folderId = 0);
    int getFolderCount();

    // 导入导出
    bool importFromHtml(const QString &filename);
    bool exportToHtml(const QString &filename);
    bool importFromBrowser(const QString &browserName);

signals:
    void bookmarksChanged();
    void bookmarkAdded(const BookmarkItem &item);
    void bookmarkUpdated(const BookmarkItem &item);
    void bookmarkDeleted(int id);
    void folderAdded(const BookmarkItem &folder);
    void folderUpdated(const BookmarkItem &folder);
    void folderDeleted(int folderId);

private:
    bool initializeDatabase();
    BookmarkItem bookmarkFromQuery(const QSqlQuery &query);

private:
    QSqlDatabase m_database;
};
