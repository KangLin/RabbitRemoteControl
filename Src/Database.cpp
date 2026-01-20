// Author: Kang Lin <kl222@126.com>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QLoggingCategory>
#include "RabbitCommonDir.h"
#include "IconUtils.h"
#include "Database.h"

static Q_LOGGING_CATEGORY(log, "DB")
CDatabase::CDatabase(QObject *parent)
    : QObject{parent}
{
    qDebug(log) << Q_FUNC_INFO;
    m_szConnectName = "connect";
}

CDatabase::~CDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    closeDatabase();
}

void CDatabase::setDatabase(QSqlDatabase db)
{
    m_database = db;
}

QSqlDatabase CDatabase::getDatabase() const
{
    return m_database;
}

bool CDatabase::openDatabase(const QString &connectionName, const QString &dbPath)
{
    QString databasePath;
    if (dbPath.isEmpty()) {
        // 使用默认路径
        QString dataDir = RabbitCommon::CDir::Instance()->GetDirUserDatabase();
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(dataDir);
        }
        databasePath = dir.filePath("database.db");
    } else {
        databasePath = dbPath;
    }

    if(isOpen())
        closeDatabase();

    if(!connectionName.isEmpty())
        m_szConnectName = connectionName;

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", m_szConnectName);
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        qCritical(log) << "Failed to open database:" << m_szConnectName
                       << m_database.lastError().text();
        return false;
    }

    qInfo(log) << "Open database connect:" << m_szConnectName << "File:" << databasePath;
    return onInitializeDatabase();
}

bool CDatabase::isOpen() const
{
    return m_database.isOpen();
}

void CDatabase::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase(m_szConnectName);
}

CDatabaseIcon::CDatabaseIcon(QObject *parent) : CDatabase(parent)
{
    m_szConnectName = "icon_connect";
}

bool CDatabaseIcon::onInitializeDatabase()
{
    QSqlQuery query(m_database);

    // Create icon table
    bool success = query.exec(
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
    query.exec("CREATE INDEX IF NOT EXISTS idx_icon_name ON icon(name)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_icon_hash ON icon(hash)");
    return true;
}

int CDatabaseIcon::getIcon(const QIcon &icon)
{
    bool bRet = false;
    QSqlQuery query(m_database);
    QString szName = icon.name();
    if(szName.isEmpty()) {
        // Check hash and data
        QByteArray data = RabbitCommon::CIconUtils::iconToByteArray(icon);
        QString szHash = RabbitCommon::CIconUtils::hashIconData(data);
        query.prepare("SELECT id, data FROM icon WHERE hash=:hash");
        query.bindValue(":hash", szHash);
        bRet = query.exec();
        if(!bRet) {
            qCritical(log) << "Failed to select icon hash:"
                           << szHash << query.lastError().text();
            return 0;
        }
        while (query.next()) {
            // check a same data
            if(data == query.value(1).toByteArray()) {
                return query.value(0).toInt();
            }
        }

        query.prepare(
            "INSERT INTO icon (hash, data) "
            "VALUES (:hash, :data)"
            );
        query.bindValue(":hash", szHash);
        query.bindValue(":data", data);
        bRet = query.exec();
        if(!bRet) {
            qCritical(log) << "Failed to insert icon hash:"
                           << szHash << query.lastError().text();
            return 0;
        }
        return query.lastInsertId().toInt();
    }

    // Check name
    query.prepare("SELECT id FROM icon WHERE name=:name");
    query.bindValue(":name", szName);
    bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to select icon name:"
                       << szName << query.lastError().text();
        return 0;
    }
    if(query.next()) {
        return query.value(0).toInt();
    }

    query.prepare("INSERT INTO icon (name) VALUES (:name)");
    query.bindValue(":name", szName);
    bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to insert icon name:"
                       << szName << query.lastError().text();
        return 0;
    }
    return query.lastInsertId().toInt();
}

QIcon CDatabaseIcon::getIcon(int id)
{
    QIcon icon;
    QSqlQuery query(m_database);
    query.prepare(
        "SELECT name, data FROM icon "
        "WHERE id=:id "
        );
    query.bindValue(":id", id);
    bool bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to delete icon id:"
                       << id << query.lastError().text();
        return icon;
    }

    if (query.next()) {
        QString szName = query.value(0).toString();
        if(!szName.isEmpty()) {
            return QIcon::fromTheme(szName);
        }
        QByteArray ba = query.value(1).toByteArray();
        return RabbitCommon::CIconUtils::byteArrayToIcon(ba);
    }
    return icon;
}

CDatabaseFolder::CDatabaseFolder(QObject *parent) : CDatabase(parent)
{
    m_szConnectName = "folder_connect";
}

bool CDatabaseFolder::onInitializeDatabase()
{
    QSqlQuery query(m_database);

    // 启用外键约束
    query.exec("PRAGMA foreign_keys = ON");

    // 创建文件夹表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS folders ("
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
    query.exec("CREATE INDEX IF NOT EXISTS idx_folders_parent ON folders(parent_id)");

    return true;
}


bool CDatabaseFolder::addFolder(const QString &name, int parentId)
{
    QSqlQuery query(m_database);

    // 获取最大排序值
    query.prepare("SELECT MAX(sort_order) FROM folders WHERE parent_id = :parent_id");
    query.bindValue(":parent_id", parentId);
    query.exec();

    int maxOrder = 0;
    if (query.next()) {
        maxOrder = query.value(0).toInt() + 1;
    }

    query.prepare(
        "INSERT INTO folders (name, parent_id, sort_order) "
        "VALUES (:name, :parent_id, :sort_order)"
        );
    query.bindValue(":name", name);
    query.bindValue(":parent_id", parentId);
    query.bindValue(":sort_order", maxOrder);

    bool success = query.exec();

    if (success)
        emit sigChanged();
    else
        qCritical(log) << "Failed to add folders:" << query.lastError().text();

    return success;
}

bool CDatabaseFolder::renameFolder(int folderId, const QString &newName)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE folders SET name = :name WHERE id = :id");
    query.bindValue(":id", folderId);
    query.bindValue(":name", newName);

    bool success = query.exec();

    if (success)
        emit sigChanged();
    else
        qCritical(log) << "Failed to rename folders:" << query.lastError().text();

    return success;
}

bool CDatabaseFolder::deleteFolder(int folderId)
{
    // 删除子目录
    auto folders = getSubFolders(folderId);
    foreach(auto f, folders) {
        deleteFolder(f.id);
    }

    // 删除其下面的所有条目
    onDeleteItems();

    // 删除文件夹
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM folders WHERE id = :id");
    query.bindValue(":id", folderId);

    bool success = query.exec();

    if (success)
        emit sigChanged();
    else
        qCritical(log) << "Failed to delete folders:" << query.lastError().text();

    return success;
}

bool CDatabaseFolder::moveFolder(int folderId, int newParentId)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE folders SET parent_id = :parent_id WHERE id = :id");
    query.bindValue(":id", folderId);
    query.bindValue(":parent_id", newParentId);

    bool success = query.exec();

    if (success)
        emit sigChanged();
    else
        qCritical(log) << "Failed to move folders:" << query.lastError().text();

    return success;
}


QList<CDatabaseFolder::FolderItem> CDatabaseFolder::getAllFolders()
{
    QList<CDatabaseFolder::FolderItem> folders;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, name, parent_id, sort_order, created_time "
        "FROM folders "
        "ORDER BY parent_id, sort_order, name"
        );

    if (query.exec()) {
        while (query.next()) {
            CDatabaseFolder::FolderItem folder;
            folder.id = query.value(0).toInt();
            folder.szName = query.value(1).toString();
            folder.parentId = query.value(2).toInt();
            folder.sortOrder = query.value(3).toInt();
            folder.createTime = query.value(4).toDateTime();

            folders.append(folder);
        }
    }

    return folders;
}

QList<CDatabaseFolder::FolderItem> CDatabaseFolder::getSubFolders(int parentId)
{
    QList<CDatabaseFolder::FolderItem> folders;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, name, parent_id, sort_order, created_time "
        "FROM folders "
        "WHERE parent_id = :parent_id "
        "ORDER BY sort_order, name"
        );
    query.bindValue(":parent_id", parentId);

    if (query.exec()) {
        while (query.next()) {
            CDatabaseFolder::FolderItem folder;
            folder.id = query.value(0).toInt();
            folder.szName = query.value(1).toString();
            folder.parentId = query.value(2).toInt();
            folder.sortOrder = query.value(3).toInt();
            folder.createTime = query.value(4).toDateTime();

            folders.append(folder);
        }
    }

    return folders;
}
