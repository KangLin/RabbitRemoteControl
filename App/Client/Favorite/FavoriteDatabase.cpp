// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "RabbitCommonDir.h"
#include "FavoriteDatabase.h"

static Q_LOGGING_CATEGORY(log, "App.Favorite.Db")
CFavoriteDatabase::CFavoriteDatabase(QObject *parent)
    : QObject{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CFavoriteDatabase::~CFavoriteDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    closeDatabase();
}

bool CFavoriteDatabase::openDatabase(const QString &dbPath)
{
    QString databasePath;
    if (dbPath.isEmpty()) {
        // 使用默认路径
        QString dataDir = RabbitCommon::CDir::Instance()->GetDirUserDatabase();
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(dataDir);
        }
        databasePath = dir.filePath("Favorite.db");
    } else {
        databasePath = dbPath;
    }

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", "favorite_connection");
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        qCritical(log) << "Failed to open database:"
                       << m_database.lastError().text();
        return false;
    }

    qInfo(log) << "Open database:" << databasePath;
    return initializeDatabase();
}

bool CFavoriteDatabase::isOpen()
{
    return m_database.isOpen();
}

void CFavoriteDatabase::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase("favorite_connection");
}

bool CFavoriteDatabase::initializeDatabase()
{
    QSqlQuery query(m_database);

    // 启用外键约束
    query.exec("PRAGMA foreign_keys = ON");

    // 创建书签表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS favorite ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL,"
        "    icon INTEGER DEFAULT 0,"
        "    file TEXT NOT NULL,"
        "    description TEXT,"
        "    time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    tags TEXT,"
        "    parent_id INTEGER DEFAULT 0,"
        "    FOREIGN KEY (parent_id) REFERENCES favorite_folders(id) ON DELETE SET NULL"
        ")"
        );

    if (!success) {
        qCritical(log) << "Failed to create favorite table:" << query.lastError().text();
        return false;
    }

    // 创建文件夹表
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS favorite_folders ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL,"
        "    parent_id INTEGER DEFAULT 0,"
        "    sort_order INTEGER DEFAULT 0,"
        "    time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        );

    if (!success) {
        qCritical(log) << "Failed to create favorite_folders table:" << query.lastError().text();
        return false;
    }

    // Create icon table
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS icon ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT UNIQUE,"       // Icon name. see QIcon::name()
        "    hash TEXT,"              // Icon hash value
        "    data BLOB"               // Icon binary data
        ")"
        );
    if (!success) {
        qCritical(log) << "Failed to create icon table:" << query.lastError().text();
        return false;
    }

    query.exec("CREATE INDEX IF NOT EXISTS idx_favorite_parent ON favorite(parent_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_favorite_time ON favorite(time)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_favorite_folders_parent ON favorite_folders(parent_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_icon_name ON icon(name)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_icon_hash ON icon(hash)");

    // 检查是否有默认文件夹
    query.exec("SELECT COUNT(*) FROM favorite_folders");
    if (query.next() && query.value(0).toInt() == 0) {
        QSqlQuery queryInsert(m_database);
        // 插入默认文件夹
        queryInsert.prepare("INSERT INTO favorite_folders (id, name, parent_id, sort_order) VALUES "
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
            qCritical(log) << "Failed to insert default favorite folder:"
                           << queryInsert.lastError().text();
        }
    }

    return true;
}

bool CFavoriteDatabase::AddFavorite(const Item &item)
{
    QSqlQuery query(m_database);

    query.prepare(
        "INSERT INTO favorite (name, file, description, time, parent_id)"
        "VALUES (:name, :file, :description, :time, :parent_id)"
        );
    query.bindValue(":name", item.szName);
    query.bindValue(":file", item.szFile);
    query.bindValue(":description", item.szDescription);
    query.bindValue(":time", item.time);
    query.bindValue(":parent_id", item.parentId);

    bool success = query.exec();
    if (success) {
        emit sigChanged();
    } else {
        qCritical(log) << "Failed to add bookmark:" << query.lastError().text();
    }

    return success;
}

bool CFavoriteDatabase::deleteFavorite(int id)
{
    if(0 >= id) return false;
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM favorite WHERE id=:id");
    query.bindValue(":id", id);
    bool success = query.exec();
    if (success) {
        emit sigChanged();
    } else
        qCritical(log) << "Failed to delete favorite id:"
                       << id << query.lastError().text();

    return success;
}

bool CFavoriteDatabase::moveFavorite(int id, int newParentId)
{
    if(0 >= id) return false;
    QSqlQuery query(m_database);
    query.prepare(
        "UPDATE recent SET "
        "parent_id=:parent_id "
        "WHERE id=:id"
        );
    query.bindValue(":id", id);
    query.bindValue(":parent_id", newParentId);
    bool success = query.exec();
    if (success) {
        emit sigChanged();
    } else
        qCritical(log) << "Failed to move favorite id:"
                       << id << "to" << newParentId << query.lastError().text();

    return success;
}

bool CFavoriteDatabase::addFolder(const QString &name, int parentId)
{
    QSqlQuery query(m_database);

    // 获取最大排序值
    query.prepare("SELECT MAX(sort_order) FROM favorite_folders WHERE parent_id = :parent_id");
    query.bindValue(":parent_id", parentId);
    query.exec();

    int maxOrder = 0;
    if (query.next()) {
        maxOrder = query.value(0).toInt() + 1;
    }

    query.prepare(
        "INSERT INTO favorite_folders (name, parent_id, sort_order) "
        "VALUES (:name, :parent_id, :sort_order)"
        );
    query.bindValue(":name", name);
    query.bindValue(":parent_id", parentId);
    query.bindValue(":sort_order", maxOrder);

    bool success = query.exec();

    if (success) {
        emit sigChanged();
    } else
        qCritical(log) << "Failed to add folder name:" << name;

    return success;
}

bool CFavoriteDatabase::renameFolder(int folderId, const QString &newName)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE favorite_folders SET name = :name WHERE id = :id");
    query.bindValue(":id", folderId);
    query.bindValue(":name", newName);

    bool success = query.exec();

    if (success) {
        emit sigChanged();
    }

    return success;
}

bool CFavoriteDatabase::deleteFolder(int folderId)
{
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
    query.prepare("DELETE FROM favorite_folders WHERE id = :id");
    query.bindValue(":id", folderId);

    bool success = query.exec();

    if (success) {
        emit sigChanged();
    }

    return success;
}

bool CFavoriteDatabase::moveFolder(int folderId, int newParentId)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE favorite_folders SET parent_id = :parent_id WHERE id = :id");
    query.bindValue(":id", folderId);
    query.bindValue(":parent_id", newParentId);

    bool success = query.exec();

    if (success) {
        emit sigChanged();
    }

    return success;
}

QList<CFavoriteDatabase::Item> CFavoriteDatabase::getAllFolders()
{
    QList<Item> folders;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, name, parent_id, sort_order, time "
        "FROM favorite_folders "
        "ORDER BY parent_id, sort_order, name"
        );

    if (query.exec()) {
        while (query.next()) {
            Item folder;
            folder.id = query.value(0).toInt();
            folder.szName = query.value(1).toString();
            folder.parentId = query.value(2).toInt();
            folder.type = Type_Folder;
            folder.time = query.value(4).toDateTime();

            folders.append(folder);
        }
    }

    return folders;
}

QList<CFavoriteDatabase::Item> CFavoriteDatabase::getSubFolders(int parentId)
{
    QList<Item> folders;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, name, parent_id, sort_order, time "
        "FROM favorite_folders "
        "WHERE parent_id = :parent_id "
        "ORDER BY sort_order, name"
        );
    query.bindValue(":parent_id", parentId);

    if (query.exec()) {
        while (query.next()) {
            Item folder;
            folder.id = query.value(0).toInt();
            folder.szName = query.value(1).toString();
            folder.parentId = query.value(2).toInt();
            folder.type = Type_Folder;
            folder.time = query.value(4).toDateTime();

            folders.append(folder);
        }
    }

    return folders;
}
