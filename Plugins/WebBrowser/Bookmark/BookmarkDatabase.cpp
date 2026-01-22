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

#include "BookmarkDatabase.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Bookmark.DB")
static CBookmarkDatabase* g_pDatabase = nullptr;
CBookmarkDatabase* CBookmarkDatabase::Instance(const QSqlDatabase &database)
{
    if(!g_pDatabase) {
        g_pDatabase = new CBookmarkDatabase();
        if(g_pDatabase) {
            g_pDatabase->SetDatabase(database);
            g_pDatabase->OnInitializeDatabase();
        }
    }
    return g_pDatabase;
}

CBookmarkDatabase* CBookmarkDatabase::Instance(const QString &szFile)
{
    if(!g_pDatabase) {
        g_pDatabase = new CBookmarkDatabase();
        if(g_pDatabase) {
            bool bRet = g_pDatabase->OpenDatabase("bookmarks_connect", szFile);
            if(!bRet) {
                delete g_pDatabase;
                g_pDatabase = nullptr;
            }
        }
    }
    return g_pDatabase;
}

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
    m_UrlDB.SetDatabase(GetDatabase());
    m_UrlDB.OnInitializeDatabase();
    m_TreeDB.SetDatabase(GetDatabase());
    m_TreeDB.OnInitializeDatabase();

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

bool CBookmarkDatabase::addFolder(const QString &name, int parentId)
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
    BookmarkItem item;

    auto leaf = m_TreeDB.GetLeaf(id);
    if(leaf.GetId() == 0) return item;
    item = TreeToBookmark(leaf);
    return item;
}

QList<BookmarkItem> CBookmarkDatabase::getBookmarkByUrl(const QString &url)
{
    QList<BookmarkItem> lstItems;
    BookmarkItem item;

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

TreeItem CBookmarkDatabase::BookmarkToTree(const BookmarkItem& tree, bool setKey)
{
    TreeItem item;
    TreeItem::TYPE type;
    type = tree.type == BookmarkType_Folder ? TreeItem::Node: TreeItem::Leaf;
    item.SetType(type);
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
    BookmarkItem item;
    CDatabaseUrl::UrlItem url = m_UrlDB.GetItem(tree.GetKey());
    item = TreeToBookmark(tree, url);
    return item;
}

BookmarkItem CBookmarkDatabase::TreeToBookmark(
    const TreeItem& tree, const CDatabaseUrl::UrlItem& url)
{
    BookmarkItem item;
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
