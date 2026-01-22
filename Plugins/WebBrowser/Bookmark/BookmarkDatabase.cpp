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
#include "RabbitCommonDir.h"
#include "BookmarkDatabase.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Bookmark.DB")
CBookmarkDatabase* CBookmarkDatabase::Instance(const QString &szPath)
{
    static CBookmarkDatabase* p = nullptr;
    if(!p) {
        p = new CBookmarkDatabase();
        if(p) {
            bool bRet = p->OpenDatabase("bookmarks_connect", szPath);
            if(!bRet) {
                delete p;
                p = nullptr;
            }
        }
    }
    return p;
}

CBookmarkDatabase::CBookmarkDatabase(QObject *parent)
    : CDatabase(parent)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBookmarkDatabase::~CBookmarkDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
}

bool CBookmarkDatabase::OnInitializeDatabase()
{
    QSqlQuery query(GetDatabase());

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
        qCritical(log) << "Failed to create bookmark_folders table:" << query.lastError().text();
        return false;
    }

    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_bookmarks_url ON bookmarks(url)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_bookmarks_folder ON bookmarks(folder_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_bookmarks_favorite ON bookmarks(favorite)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_bookmarks_created ON bookmarks(created_time)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_bookmarks_folders_parent ON bookmark_folders(parent_id)");

    // 检查是否有默认文件夹
    query.exec("SELECT COUNT(*) FROM bookmark_folders");
    if (query.next() && query.value(0).toInt() == 0) {
        QSqlQuery queryInsert(GetDatabase());
        // 插入默认文件夹
        queryInsert.prepare("INSERT INTO bookmark_folders (id, name, parent_id, sort_order) VALUES "
                   "(1, :bookmark, 0, 1), "
                   "(2, :other, 0, 2), "
                   "(3, :favorite, 1, 1), "
                   "(4, :frequently, 1, 2)");
        queryInsert.bindValue(":bookmark", tr("Bookmarks"));
        queryInsert.bindValue(":other", tr("Other"));
        queryInsert.bindValue(":favorite", tr("Favorites"));
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
    QSqlQuery query(GetDatabase());

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
    QSqlQuery query(GetDatabase());

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
        qCritical(log) << "Failed to update bookmark:" << query.lastError().text()
                       << item.id << item.title << item.url << item.folderId;
    }

    return success;
}

bool CBookmarkDatabase::deleteBookmark(int id)
{
    QSqlQuery query(GetDatabase());
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

    QSqlQuery query(GetDatabase());

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
    QSqlQuery query(GetDatabase());
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
    QSqlQuery query(GetDatabase());

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
    QSqlQuery query(GetDatabase());
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

    QSqlQuery query(GetDatabase());
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
    QSqlQuery query(GetDatabase());
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

    QSqlQuery query(GetDatabase());
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

    QSqlQuery query(GetDatabase());
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

    QSqlQuery query(GetDatabase());
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

    QSqlQuery query(GetDatabase());
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

    QSqlQuery query(GetDatabase());
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

    QSqlQuery query(GetDatabase());
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

    QSqlQuery query(GetDatabase());
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
    QSqlQuery query(GetDatabase());

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
    QSqlQuery query(GetDatabase());
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

bool CBookmarkDatabase::exportToHtml(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QDomDocument doc;

    // 构建完整文档
    buildBookmarkDocument(doc);

    // 写入文件
    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out << doc.toString(4);

    file.close();
    return true;
}

void CBookmarkDatabase::buildBookmarkDocument(QDomDocument &doc)
{
    // XML 声明
    QDomProcessingInstruction xmlDecl = doc.createProcessingInstruction(
        "xml", "version=\"1.0\" encoding=\"UTF-8\""
        );
    doc.appendChild(xmlDecl);

    // 注释
    QDomComment comment = doc.createComment(
        " This is an automatically generated file by Rabbit Remote Control.\n"
        "     It will be read and overwritten.\n"
        "     DO NOT EDIT! "
        );
    doc.appendChild(comment);

    // DOCTYPE
    QDomDocumentType doctype = doc.implementation().createDocumentType(
        "NETSCAPE-Bookmark-file-1",
        QString(),
        QString()
        );
    doc.appendChild(doctype);

    // HTML 根元素
    QDomElement htmlElement = doc.createElement("HTML");
    doc.appendChild(htmlElement);

    // HEAD
    QDomElement headElement = doc.createElement("HEAD");
    htmlElement.appendChild(headElement);

    // META
    QDomElement metaElement = doc.createElement("META");
    metaElement.setAttribute("HTTP-EQUIV", "Content-Type");
    metaElement.setAttribute("CONTENT", "text/html; charset=UTF-8");
    headElement.appendChild(metaElement);

    // TITLE
    QDomElement titleElement = doc.createElement("TITLE");
    QDomText titleText = doc.createTextNode("Rabbit Remote Control - Browser: Bookmarks");
    titleElement.appendChild(titleText);
    headElement.appendChild(titleElement);

    // BODY
    QDomElement bodyElement = doc.createElement("BODY");
    htmlElement.appendChild(bodyElement);

    // H1
    QDomElement h1Element = doc.createElement("H1");
    QDomText h1Text = doc.createTextNode("Bookmarks");
    h1Element.appendChild(h1Text);
    bodyElement.appendChild(h1Element);

    // 主 DL
    QDomElement dlElement = doc.createElement("DL");
    bodyElement.appendChild(dlElement);

    // 构建书签树
    buildBookmarkTree(doc, dlElement, 0);
}

void CBookmarkDatabase::buildBookmarkTree(QDomDocument &doc,
                                          QDomElement &parentElement,
                                          int folderId)
{
    // 获取该文件夹下的书签
    QList<BookmarkItem> bookmarks = getAllBookmarks(folderId);

    for (const auto &bookmark : bookmarks) {
        QDomElement dtElement = doc.createElement("DT");
        parentElement.appendChild(dtElement);

        QDomElement aElement = createBookmarkDomElement(doc, bookmark);
        dtElement.appendChild(aElement);
    }

    // 获取子文件夹
    QList<BookmarkItem> subFolders = getSubFolders(folderId);

    for (const auto &folder : subFolders) {
        // 创建文件夹
        QDomElement dtElement = doc.createElement("DT");
        parentElement.appendChild(dtElement);

        // 文件夹标题
        QDomElement h3Element = doc.createElement("H3");

        // 文件夹属性
        if (folder.createdTime.isValid()) {
            qint64 timestamp = folder.createdTime.toSecsSinceEpoch();
            h3Element.setAttribute("ADD_DATE", QString::number(timestamp));
        } else {
            h3Element.setAttribute("ADD_DATE", "0");
        }

        // 文件夹名称
        QDomText folderNameText = doc.createTextNode(folder.title);
        h3Element.appendChild(folderNameText);

        dtElement.appendChild(h3Element);

        // 创建子 DL
        QDomElement childDlElement = doc.createElement("DL");
        dtElement.appendChild(childDlElement);

        // 递归处理子文件夹
        buildBookmarkTree(doc, childDlElement, folder.id);
    }
}

QDomElement CBookmarkDatabase::createBookmarkDomElement(
    QDomDocument &doc, const BookmarkItem &bookmark)
{
    QDomElement aElement = doc.createElement("A");

    // 必需属性
    aElement.setAttribute("HREF", bookmark.url);

    // 时间属性
    QDateTime defaultTime = QDateTime::currentDateTime();

    qint64 addDate = bookmark.createdTime.isValid() ?
                         bookmark.createdTime.toSecsSinceEpoch() :
                         defaultTime.toSecsSinceEpoch();
    qint64 lastVisit = bookmark.lastVisitTime.isValid() ?
                           bookmark.lastVisitTime.toSecsSinceEpoch() : 0;
    qint64 lastModified = bookmark.modifiedTime.isValid() ?
                              bookmark.modifiedTime.toSecsSinceEpoch() :
                              addDate;

    aElement.setAttribute("ADD_DATE", QString::number(addDate));
    aElement.setAttribute("LAST_VISIT_TIME", QString::number(lastVisit));
    aElement.setAttribute("MODIFIED_TIME", QString::number(lastModified));

    // 可选属性
    if (!bookmark.iconUrl.isEmpty()) {
        aElement.setAttribute("ICON", bookmark.iconUrl);
    }

    if (!bookmark.description.isEmpty()) {
        aElement.setAttribute("DESCRIPTION", bookmark.description);
    }

    if (!bookmark.tags.isEmpty()) {
        aElement.setAttribute("TAGS", bookmark.tags.join(","));
    }

    // 标题
    QString displayTitle = bookmark.title.isEmpty() ? bookmark.url : bookmark.title;
    QDomText titleText = doc.createTextNode(displayTitle);
    aElement.appendChild(titleText);

    return aElement;
}

bool CBookmarkDatabase::importFromHtml(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for import:" << filename;
        return false;
    }

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    QString content = in.readAll();
    file.close();

    qDebug(log) << "Importing HTML file:" << filename
                << "File size:" << content.size() << "bytes";

    // 检查文件格式
    if (!content.contains("<!DOCTYPE", Qt::CaseInsensitive) ||
        !content.contains("NETSCAPE-Bookmark-file-1", Qt::CaseInsensitive)) {
        qWarning() << "Not a valid Netscape bookmark file:" << filename;
        QMessageBox::warning(nullptr, tr("Format error"),
                             tr("This is not a valid bookmark file format.\n"
                                "Please select the HTML bookmark file exported from your browser."));
        return false;
    }

    // 开始事务
    GetDatabase().transaction();

    try {
        int importedCount = parseHtmlBookmarks(content);

        if (!GetDatabase().commit()) {
            throw QString("Failed to commit transaction: %1").arg(GetDatabase().lastError().text());
        }

        emit bookmarksChanged();

        qDebug(log) << "Successfully imported" << importedCount << "bookmarks";
        return true;

    } catch (const QString &error) {
        GetDatabase().rollback();
        qWarning(log) << "Import failed:" << error;
        return false;
    }
}

int CBookmarkDatabase::parseHtmlBookmarks(const QString &htmlContent)
{
    int importedCount = 0;

    // 解析 HTML 书签文件
    QDomDocument doc;
    if (!doc.setContent(htmlContent)) {
        throw QString("Invalid HTML format");
    }

    // 查找根节点
    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        throw QString("No root element found");
    }

    // 查找 BODY 标签（书签列表）
    QDomElement bodyElement = findFirstElement(root, "BODY");
    if (bodyElement.isNull()) {
        throw QString("No body node found");
    }

    // 查找 DL 标签（书签列表）
    QDomElement dlElement = findFirstElement(bodyElement, "DL");
    if (dlElement.isNull()) {
        throw QString("No bookmarks list found");
    }

    // 递归解析书签
    QMap<QString, int> folderMap;  // 文件夹路径 -> 文件夹ID
    folderMap["/"] = 0;  // 根目录

    importedCount = parseBookmarkList(dlElement, "/", folderMap);

    return importedCount;
}

int CBookmarkDatabase::parseBookmarkList(const QDomElement &dlElement,
                                         const QString &currentPath,
                                         QMap<QString, int> &folderMap)
{
    int importedCount = 0;
    QDomNode child = dlElement.firstChild();

    while (!child.isNull()) {
        QDomElement element = child.toElement();

        if (!element.isNull()) {
            QString tagName = element.tagName().toUpper();

            if (tagName == "DT") {
                // 处理 DT 节点
                importedCount += parseDtElement(element, currentPath, folderMap);
            } else if (tagName == "DL") {
                // 嵌套的 DL 标签，继续解析
                importedCount += parseBookmarkList(element, currentPath, folderMap);
            }
        }

        child = child.nextSibling();
    }

    return importedCount;
}

int CBookmarkDatabase::parseDtElement(const QDomElement &dtElement,
                                      const QString &currentPath,
                                      QMap<QString, int> &folderMap)
{
    int importedCount = 0;

    QString parentPath = currentPath;

    // 查找子节点
    QDomNode child = dtElement.firstChild();
    while (!child.isNull()) {
        QDomElement element = child.toElement();

        if (!element.isNull()) {
            QString tagName = element.tagName().toUpper();

            if (tagName == "A") {
                // 书签链接
                importedCount += importBookmark(element, currentPath, folderMap);
            } else if (tagName == "H3") {
                // 文件夹
                parentPath = importFolder(element, currentPath, folderMap);
            } else if (tagName == "DL") {
                // 嵌套的书签列表
                importedCount += parseBookmarkList(element, parentPath, folderMap);
            }
        }

        child = child.nextSibling();
    }

    return importedCount;
}

int CBookmarkDatabase::importBookmark(const QDomElement &aElement,
                                      const QString &folderPath,
                                      QMap<QString, int> &folderMap)
{
    // 获取链接属性
    QString url = aElement.attribute("HREF");
    QString title = aElement.text();

    if (url.isEmpty() || title.isEmpty()) {
        return 0;
    }

    // 获取时间戳
    QString addDateStr = aElement.attribute("ADD_DATE");
    QString lastVisitStr = aElement.attribute("LAST_VISIT");
    QString lastModifiedStr = aElement.attribute("LAST_MODIFIED");

    QDateTime addDate = parseTimestamp(addDateStr);
    QDateTime lastVisit = parseTimestamp(lastVisitStr);
    QDateTime lastModified = parseTimestamp(lastModifiedStr);

    // 创建书签对象
    BookmarkItem item;
    item.url = url;
    item.title = title;
    item.createdTime = addDate.isValid() ? addDate : QDateTime::currentDateTime();
    item.lastVisitTime = lastVisit;
    item.modifiedTime = lastModified.isValid() ? lastModified : item.createdTime;

    // 获取图标（如果有）
    QString iconUrl = aElement.attribute("ICON");
    if (!iconUrl.isEmpty()) {
        item.iconUrl = iconUrl;

        // 尝试获取ICON_URI属性
        QString iconUri = aElement.attribute("ICON_URI");
        if (!iconUri.isEmpty()) {
            item.iconUrl = iconUri;
        }
    }

    // 获取标签（如果有）
    QString tags = aElement.attribute("TAGS");
    if (!tags.isEmpty()) {
        item.tags = tags.split(",");
    }

    // 查找或创建文件夹
    int folderId = getOrCreateFolder(folderPath, folderMap);
    item.folderId = folderId;

    // 检查是否已存在
    auto lstExisting = getBookmarkByUrl(url);
    if (lstExisting.isEmpty()) {
        // 插入新书签
        if (addBookmark(item)) {
            qDebug(log) << "Imported bookmark:" << title << folderPath << "parentID:" << folderId;
            return 1;
        }
    } else {
        foreach(auto exist, lstExisting) {
            // 更新现有书签
            exist.title = item.title;
            exist.iconUrl = item.iconUrl;
            exist.tags = item.tags;
            exist.folderId = item.folderId;
            exist.lastVisitTime = item.lastVisitTime;
            exist.modifiedTime = QDateTime::currentDateTime();

            if (updateBookmark(exist)) {
                qDebug(log) << "Updated existing bookmark:" << title << folderPath << "parentID:" << folderId;
            }
        }

        return lstExisting.count();
    }

    return 0;
}

QString CBookmarkDatabase::importFolder(const QDomElement &h3Element,
                                    const QString &parentPath,
                                    QMap<QString, int> &folderMap)
{
    QString folderName = h3Element.text();
    if (folderName.isEmpty()) {
        return 0;
    }

    // 构建完整路径
    QString folderPath = parentPath;
    if (!parentPath.endsWith("/")) {
        folderPath += "/";
    }
    folderPath += folderName;

    // 查找父文件夹ID
    int parentFolderId = folderMap.value(parentPath, 0);

    // 创建文件夹
    int folderId = getOrCreateFolder(folderPath, parentFolderId);
    folderMap[folderPath] = folderId;

    qDebug(log) << "Imported folder:" << folderName << "Path:" << folderPath << "ID:" << folderId;

    return folderPath;
}

int CBookmarkDatabase::getOrCreateFolder(const QString &folderPath, int parentFolderId)
{
    if (folderPath.isEmpty() || folderPath == "/") {
        return 0;  // 根目录
    }

    // 分割路径
    QStringList pathParts = folderPath.split("/", Qt::SkipEmptyParts);
    if (pathParts.isEmpty()) {
        return 0;
    }

    QString folderName = pathParts.last();

    // 检查文件夹是否已存在
    QSqlQuery query(GetDatabase());
    query.prepare("SELECT id FROM bookmark_folders WHERE name = :name AND parent_id = :parent_id");
    query.bindValue(":name", folderName);
    query.bindValue(":parent_id", parentFolderId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    // 创建新文件夹
    query.prepare(
        "INSERT INTO bookmark_folders (name, parent_id, created_time) "
        "VALUES (:name, :parent_id, :created_time)"
        );
    query.bindValue(":name", folderName);
    query.bindValue(":parent_id", parentFolderId);
    query.bindValue(":created_time", QDateTime::currentDateTime());

    if (query.exec()) {
        return query.lastInsertId().toInt();
    }

    qCritical(log) << "Failed to create folder:" << folderName << query.lastError().text();
    return 0;  // 失败时返回根目录
}

int CBookmarkDatabase::getOrCreateFolder(const QString &folderPath,
                                         QMap<QString, int> &folderMap)
{
    if (folderPath.isEmpty() || folderPath == "/") {
        return 0;
    }

    if(folderMap.contains(folderPath))
        return folderMap[folderPath];

    QStringList pathParts = folderPath.split("/", Qt::SkipEmptyParts);
    int currentParentId = 0;

    // 逐级创建文件夹
    for (const QString &part : pathParts) {
        if (part.isEmpty()) continue;

        currentParentId = getOrCreateFolder(part, currentParentId);
    }

    return currentParentId;
}

QDateTime CBookmarkDatabase::parseTimestamp(const QString &timestampStr)
{
    if (timestampStr.isEmpty()) {
        return QDateTime();
    }

    bool ok;
    qint64 timestamp = timestampStr.toLongLong(&ok);

    if (!ok) {
        return QDateTime();
    }

    // 处理不同精度的时间戳
    if (timestamp > 10000000000) {
        // 毫秒时间戳
        return QDateTime::fromMSecsSinceEpoch(timestamp);
    } else {
        // 秒时间戳
        return QDateTime::fromSecsSinceEpoch(timestamp);
    }
}

QDomElement CBookmarkDatabase::findFirstElement(const QDomElement &parent, const QString &tagName)
{
    QDomNode child = parent.firstChild();

    while (!child.isNull()) {
        QDomElement element = child.toElement();

        if (!element.isNull() && element.tagName().toUpper() == tagName.toUpper()) {
            return element;
        }

        child = child.nextSibling();
    }

    return QDomElement();
}
