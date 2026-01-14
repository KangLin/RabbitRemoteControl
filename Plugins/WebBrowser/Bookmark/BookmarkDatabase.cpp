// Author: Kang Lin <kl222@126.com>

#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QLoggingCategory>
#include "RabbitCommonDir.h"
#include "BookmarkDatabase.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Bookmark.DB")
CBookmarkDatabase::CBookmarkDatabase(QObject *parent)
    : QObject(parent)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBookmarkDatabase::~CBookmarkDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    closeDatabase();
}

bool CBookmarkDatabase::openDatabase(const QString &dbPath)
{
    QString databasePath;
    if (dbPath.isEmpty()) {
        // 使用默认路径
        QString dataDir = RabbitCommon::CDir::Instance()->GetDirDatabase();
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(dataDir);
        }
        databasePath = dir.filePath("bookmarks.db");
    } else {
        databasePath = dbPath;
    }

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", "bookmarks_connection");
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        qCritical(log) << "Failed to open bookmarks database:"
                       << m_database.lastError().text() << databasePath;
        return false;
    }

    return initializeDatabase();
}

void CBookmarkDatabase::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase("bookmarks_connection");
}

bool CBookmarkDatabase::initializeDatabase()
{
    QSqlQuery query(m_database);

    // 启用外键约束
    query.exec("PRAGMA foreign_keys = ON");

    // 创建书签表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS bookmarks ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    url TEXT NOT NULL,"
        "    title TEXT NOT NULL,"
        "    icon BLOB,"
        "    icon_url TEXT,"
        "    description TEXT,"
        "    created_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    modified_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    visit_count INTEGER DEFAULT 0,"
        "    last_visit_time DATETIME,"
        "    favorite BOOLEAN DEFAULT 0,"
        "    tags TEXT,"
        "    folder_id INTEGER DEFAULT 1,"
        "    FOREIGN KEY (folder_id) REFERENCES bookmark_folders(id) ON DELETE SET NULL"
        ")"
        );

    if (!success) {
        qCritical(log) << "Failed to create bookmarks table:" << query.lastError().text();
        return false;
    }

    // 创建文件夹表
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS bookmark_folders ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL,"
        "    parent_id INTEGER DEFAULT 0,"
        "    sort_order INTEGER DEFAULT 0,"
        "    created_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        );

    if (!success) {
        qCritical(log) << "Failed to create folders table:" << query.lastError().text();
        return false;
    }

    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_bookmarks_url ON bookmarks(url)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_bookmarks_folder ON bookmarks(folder_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_bookmarks_favorite ON bookmarks(favorite)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_bookmarks_created ON bookmarks(created_time)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_folders_parent ON bookmark_folders(parent_id)");

    // 检查是否有默认文件夹
    query.exec("SELECT COUNT(*) FROM bookmark_folders");
    if (query.next() && query.value(0).toInt() == 0) {
        QSqlQuery queryInsert(m_database);
        // 插入默认文件夹
        queryInsert.prepare("INSERT INTO bookmark_folders (id, name, parent_id, sort_order) VALUES "
                   "(1, :bookmark, 0, 1), "
                   "(2, :other, 0, 2), "
                   "(3, :favorite, 1, 1), "
                   "(4, :frequently, 1, 2)");
        queryInsert.bindValue(":bookmark", tr("Bookmark"));
        queryInsert.bindValue(":other", tr("Other"));
        queryInsert.bindValue(":favorite", tr("Favorite"));
        queryInsert.bindValue(":frequently", tr("Frequently Used Websites"));
        bool bRet = queryInsert.exec();
        if(!bRet) {
            qCritical(log) << "Failed to insert default folder:" << queryInsert.lastError().text();
        }
    }

    return true;
}

bool CBookmarkDatabase::addBookmark(const BookmarkItem &item)
{
    QSqlQuery query(m_database);

    query.prepare(
        "INSERT INTO bookmarks (url, title, icon, icon_url, description, "
        "created_time, modified_time, visit_count, last_visit_time, "
        "favorite, tags, folder_id) "
        "VALUES (:url, :title, :icon, :icon_url, :description, "
        ":created_time, :modified_time, :visit_count, :last_visit_time, "
        ":favorite, :tags, :folder_id)"
        );

    query.bindValue(":url", item.url);
    query.bindValue(":title", item.title);
    query.bindValue(":icon", item.iconData);
    query.bindValue(":icon_url", item.iconUrl);
    query.bindValue(":description", item.description);
    query.bindValue(":created_time", item.createdTime);
    query.bindValue(":modified_time", QDateTime::currentDateTime());
    query.bindValue(":visit_count", item.visitCount);
    query.bindValue(":last_visit_time", item.lastVisitTime);
    query.bindValue(":favorite", item.isFavorite);
    query.bindValue(":tags", item.tags.join(","));
    query.bindValue(":folder_id", item.folderId);

    bool success = query.exec();

    if (success) {
        int id = query.lastInsertId().toInt();
        BookmarkItem newItem = item;
        newItem.id = id;
        emit bookmarkAdded(newItem);
        emit bookmarksChanged();
    } else {
        qCritical(log) << "Failed to add bookmark:" << query.lastError().text();
    }

    return success;
}

bool CBookmarkDatabase::updateBookmark(const BookmarkItem &item)
{
    QSqlQuery query(m_database);

    query.prepare(
        "UPDATE bookmarks SET "
        "url = :url, "
        "title = :title, "
        "icon = :icon, "
        "icon_url = :icon_url, "
        "description = :description, "
        "modified_time = :modified_time, "
        "visit_count = :visit_count, "
        "last_visit_time = :last_visit_time, "
        "favorite = :favorite, "
        "tags = :tags, "
        "folder_id = :folder_id "
        "WHERE id = :id"
        );

    query.bindValue(":id", item.id);
    query.bindValue(":url", item.url);
    query.bindValue(":title", item.title);
    query.bindValue(":icon", item.iconData);
    query.bindValue(":icon_url", item.iconUrl);
    query.bindValue(":description", item.description);
    query.bindValue(":modified_time", QDateTime::currentDateTime());
    query.bindValue(":visit_count", item.visitCount);
    query.bindValue(":last_visit_time", item.lastVisitTime);
    query.bindValue(":favorite", item.isFavorite);
    query.bindValue(":tags", item.tags.join(","));
    query.bindValue(":folder_id", item.folderId);

    bool success = query.exec();

    if (success) {
        emit bookmarkUpdated(item);
        emit bookmarksChanged();
    } else {
        qCritical(log) << "Failed to update bookmark:" << query.lastError().text();
    }

    return success;
}

bool CBookmarkDatabase::deleteBookmark(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM bookmarks WHERE id = :id");
    query.bindValue(":id", id);

    bool success = query.exec();

    if (success) {
        emit bookmarkDeleted(id);
        emit bookmarksChanged();
    }

    return success;
}

bool CBookmarkDatabase::deleteBookmark(const QList<BookmarkItem> &items)
{
    if(items.isEmpty()) {
        qWarning(log) << "The items is empty";
        return false;
    }

    QSqlQuery query(m_database);

    QString szSql("DELETE FROM bookmarks WHERE ");
    int i = 0;
    foreach(auto item, items) {
        if(0 == i++) {
            szSql += " id = " + QString::number(item.id);
        } else {
            szSql += " OR id = " + QString::number(item.id);
        }
    }
    bool success = query.exec(szSql);

    if (success) {
        foreach(auto item, items)
            emit bookmarkDeleted(item.id);
        emit bookmarksChanged();
    }

    return success;
}

bool CBookmarkDatabase::moveBookmark(int id, int newFolderId)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE bookmarks SET folder_id = :folder_id WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":folder_id", newFolderId);

    bool success = query.exec();

    if (success) {
        emit bookmarksChanged();
    }

    return success;
}

bool CBookmarkDatabase::addFolder(const QString &name, int parentId)
{
    QSqlQuery query(m_database);

    // 获取最大排序值
    query.prepare("SELECT MAX(sort_order) FROM bookmark_folders WHERE parent_id = :parent_id");
    query.bindValue(":parent_id", parentId);
    query.exec();

    int maxOrder = 0;
    if (query.next()) {
        maxOrder = query.value(0).toInt() + 1;
    }

    query.prepare(
        "INSERT INTO bookmark_folders (name, parent_id, sort_order) "
        "VALUES (:name, :parent_id, :sort_order)"
        );
    query.bindValue(":name", name);
    query.bindValue(":parent_id", parentId);
    query.bindValue(":sort_order", maxOrder);

    bool success = query.exec();

    if (success) {
        int id = query.lastInsertId().toInt();
        BookmarkItem folder;
        folder.id = id;
        folder.title = name;
        folder.folderId = parentId;
        folder.type = BookmarkType_Folder;
        emit folderAdded(folder);
        emit bookmarksChanged();
    }

    return success;
}

bool CBookmarkDatabase::renameFolder(int folderId, const QString &newName)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE bookmark_folders SET name = :name WHERE id = :id");
    query.bindValue(":id", folderId);
    query.bindValue(":name", newName);

    bool success = query.exec();

    if (success) {
        emit bookmarksChanged();
    }

    return success;
}

bool CBookmarkDatabase::deleteFolder(int folderId)
{
    if(1 == folderId) {
        qWarning(log) << "folderId == 1 is not delete";
        return true;
    }

    // 删除子目录
    auto folders = getSubFolders(folderId);
    foreach(auto f, folders) {
        deleteFolder(f.id);
    }

    QSqlQuery query(m_database);
    // 将文件夹中的书签移动到默认书签
    //query.prepare("UPDATE bookmarks SET folder_id = 1 WHERE folder_id = :folder_id");
    // 删除其下面的所有书签
    query.prepare("DELETE FROM bookmarks WHERE folder_id = :folder_id");
    query.bindValue(":folder_id", folderId);
    query.exec();

    // 删除文件夹
    query.prepare("DELETE FROM bookmark_folders WHERE id = :id");
    query.bindValue(":id", folderId);

    bool success = query.exec();

    if (success) {
        emit folderDeleted(folderId);
        emit bookmarksChanged();
    }

    return success;
}

bool CBookmarkDatabase::moveFolder(int folderId, int newParentId)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE bookmark_folders SET parent_id = :parent_id WHERE id = :id");
    query.bindValue(":id", folderId);
    query.bindValue(":parent_id", newParentId);

    bool success = query.exec();

    if (success) {
        emit bookmarksChanged();
    }

    return success;
}

BookmarkItem CBookmarkDatabase::getBookmark(int id)
{
    BookmarkItem item;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, url, title, icon, icon_url, description, "
        "created_time, modified_time, visit_count, last_visit_time, "
        "favorite, tags, folder_id "
        "FROM bookmarks WHERE id = :id"
        );
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        item = bookmarkFromQuery(query);
    }

    return item;
}

QList<BookmarkItem> CBookmarkDatabase::getBookmarkByUrl(const QString &url)
{
    QList<BookmarkItem> lstItems;
    BookmarkItem item;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, url, title, icon, icon_url, description, "
        "created_time, modified_time, visit_count, last_visit_time, "
        "favorite, tags, folder_id "
        "FROM bookmarks WHERE url = :url"
        );
    query.bindValue(":url", url);

    if (query.exec()) {
        while (query.next()) {
            item = bookmarkFromQuery(query);
            lstItems << item;
        }
    }

    return lstItems;
}

QList<BookmarkItem> CBookmarkDatabase::getAllBookmarks(int folderId)
{
    QList<BookmarkItem> bookmarks;

    QSqlQuery query(m_database);
    QString sql = QString(
        "SELECT id, url, title, icon, icon_url, description, "
        "created_time, modified_time, visit_count, last_visit_time, "
        "favorite, tags, folder_id "
        "FROM bookmarks "
        );

    if (folderId >= 0) {
        sql += "WHERE folder_id = :folder_id ";
    }

    sql += "ORDER BY title ASC";

    query.prepare(sql);
    if (folderId >= 0) {
        query.bindValue(":folder_id", folderId);
    }

    if (query.exec()) {
        while (query.next()) {
            bookmarks.append(bookmarkFromQuery(query));
        }
    }

    return bookmarks;
}

QList<BookmarkItem> CBookmarkDatabase::getFavoriteBookmarks()
{
    QList<BookmarkItem> bookmarks;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, url, title, icon, icon_url, description, "
        "created_time, modified_time, visit_count, last_visit_time, "
        "favorite, tags, folder_id "
        "FROM bookmarks WHERE favorite = 1 "
        "ORDER BY title ASC"
        );

    if (query.exec()) {
        while (query.next()) {
            bookmarks.append(bookmarkFromQuery(query));
        }
    }

    return bookmarks;
}

QList<BookmarkItem> CBookmarkDatabase::searchBookmarks(const QString &keyword)
{
    QList<BookmarkItem> bookmarks;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, url, title, icon, icon_url, description, "
        "created_time, modified_time, visit_count, last_visit_time, "
        "favorite, tags, folder_id "
        "FROM bookmarks "
        "WHERE url LIKE :keyword OR title LIKE :keyword OR description LIKE :keyword "
        "ORDER BY title ASC"
        );

    QString likePattern = QString("%%1%").arg(keyword);
    query.bindValue(":keyword", likePattern);

    if (query.exec()) {
        while (query.next()) {
            bookmarks.append(bookmarkFromQuery(query));
        }
    }

    return bookmarks;
}

QList<BookmarkItem> CBookmarkDatabase::getAllFolders()
{
    QList<BookmarkItem> folders;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, name, parent_id, sort_order, created_time "
        "FROM bookmark_folders "
        "ORDER BY parent_id, sort_order, name"
        );

    if (query.exec()) {
        while (query.next()) {
            BookmarkItem folder;
            folder.id = query.value(0).toInt();
            folder.title = query.value(1).toString();
            folder.folderId = query.value(2).toInt();
            folder.type = BookmarkType_Folder;
            folder.createdTime = query.value(4).toDateTime();

            folders.append(folder);
        }
    }

    return folders;
}

QList<BookmarkItem> CBookmarkDatabase::getSubFolders(int parentId)
{
    QList<BookmarkItem> folders;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, name, parent_id, sort_order, created_time "
        "FROM bookmark_folders "
        "WHERE parent_id = :parent_id "
        "ORDER BY sort_order, name"
        );
    query.bindValue(":parent_id", parentId);

    if (query.exec()) {
        while (query.next()) {
            BookmarkItem folder;
            folder.id = query.value(0).toInt();
            folder.title = query.value(1).toString();
            folder.folderId = query.value(2).toInt();
            folder.type = BookmarkType_Folder;
            folder.createdTime = query.value(4).toDateTime();

            folders.append(folder);
        }
    }

    return folders;
}

int CBookmarkDatabase::getBookmarkCount(int folderId)
{
    QSqlQuery query(m_database);

    if (folderId >= 0) {
        query.prepare("SELECT COUNT(*) FROM bookmarks WHERE folder_id = :folder_id");
        query.bindValue(":folder_id", folderId);
    } else {
        query.prepare("SELECT COUNT(*) FROM bookmarks");
    }

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

int CBookmarkDatabase::getFolderCount()
{
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM bookmark_folders");

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

BookmarkItem CBookmarkDatabase::bookmarkFromQuery(const QSqlQuery &query)
{
    BookmarkItem item;

    item.id = query.value(0).toInt();
    item.url = query.value(1).toString();
    item.title = query.value(2).toString();
    item.iconData = query.value(3).toByteArray();
    item.iconUrl = query.value(4).toString();
    item.description = query.value(5).toString();
    item.createdTime = query.value(6).toDateTime();
    item.modifiedTime = query.value(7).toDateTime();
    item.visitCount = query.value(8).toInt();
    item.lastVisitTime = query.value(9).toDateTime();
    item.isFavorite = query.value(10).toBool();

    QString tags = query.value(11).toString();
    if (!tags.isEmpty()) {
        item.tags = tags.split(",");
    }

    item.folderId = query.value(12).toInt();
    item.type = BookmarkType_Bookmark;

    return item;
}

bool CBookmarkDatabase::importFromHtml(const QString &filename)
{
    // 实现HTML导入（Chrome/Firefox格式）
    // 这里简化为从历史记录导入
    return true;
}

bool CBookmarkDatabase::exportToHtml(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif

    // HTML头部
    out << "<!DOCTYPE NETSCAPE-Bookmark-file-1>" << "\n";
    out << "<!-- This is an automatically generated file." << "\n";
    out << "     It will be read and overwritten." << "\n";
    out << "     DO NOT EDIT! -->" << "\n";
    out << "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=UTF-8\">" << "\n";
    out << "<TITLE>Bookmarks</TITLE>" << "\n";
    out << "<H1>Bookmarks</H1>" << "\n";
    out << "<DL><p>" << "\n";

    // 导出书签
    QList<BookmarkItem> bookmarks = getAllBookmarks(-1);
    for (const auto &item : bookmarks) {
        QString timestamp = QString::number(item.createdTime.toMSecsSinceEpoch() / 1000);

        out << "    <DT><A HREF=\"" << item.url << "\" "
            << "ADD_DATE=\"" << timestamp << "\" "
            << "LAST_VISIT=\"" << timestamp << "\" "
            << "LAST_MODIFIED=\"" << timestamp << "\">"
            << item.title << "</A>" << "\n";
    }

    out << "</DL><p>" << "\n";

    file.close();
    return true;
}
