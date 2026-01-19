// Author: Kang Lin <kl222@126.com>

#include <QCoreApplication>
#include <QIcon>
#include <QLoggingCategory>
#include "RabbitCommonDir.h"
#include "IconUtils.h"
#include "RecentDatabase.h"

static Q_LOGGING_CATEGORY(log, "App.Recent.Db")
CRecentDatabase::CRecentDatabase(QObject *parent)
    : QObject{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CRecentDatabase::~CRecentDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    closeDatabase();
}

bool CRecentDatabase::openDatabase(const QString &dbPath)
{
    QString databasePath;
    if (dbPath.isEmpty()) {
        // 使用默认路径
        QString dataDir = RabbitCommon::CDir::Instance()->GetDirUserDatabase();
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(dataDir);
        }
        databasePath = dir.filePath("Recent.db");
    } else {
        databasePath = dbPath;
    }

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", "recent_connection");
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        qCritical(log) << "Failed to open database:"
                       << m_database.lastError().text()
                       << ";Library paths:" << QCoreApplication::libraryPaths();
        return false;
    }

    return initializeDatabase();
}

bool CRecentDatabase::isOpen()
{
    return m_database.isOpen();
}

void CRecentDatabase::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase("recent_connection");
}

bool CRecentDatabase::initializeDatabase()
{
    QSqlQuery query(m_database);

    // Create recent table
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS recent ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    operate_id TEXT NO NULL,"
        "    icon INTEGER DEFAULT 0,"
        "    name TEXT NOT NULL,"
        "    protocol TEXT,"
        "    type TEXT,"
        "    file TEXT UNIQUE NOT NULL,"
        "    time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    description TEXT"
        ")"
        );

    if (!success) {
        qCritical(log) << "Failed to create recent table:" << query.lastError().text();
        return false;
    }

    // Create index
    query.exec("CREATE INDEX IF NOT EXISTS idx_recent_file ON recent(file)");

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
    query.exec("CREATE INDEX IF NOT EXISTS idx_recent_hash ON recent(hash)");
    return true;
}

int CRecentDatabase::addRecent(const Item &item)
{
    QSqlQuery query(m_database);
    if(item.szFile.isEmpty())
        return -1;
    query.prepare(
        "SELECT id FROM recent "
        "WHERE file=:file"
        );
    query.bindValue(":file", item.szFile);
    if (query.exec() && query.next()) {
        // 更新现有记录
        int id = query.value(0).toInt();

        query.prepare(
            "UPDATE recent SET "
            "name=:name, "
            "time=:time, "
            "description=:description "
            "WHERE id=:id"
            );
        query.bindValue(":name", item.szName);
        query.bindValue(":time", item.time);
        query.bindValue(":description", item.szDescription);
        query.bindValue(":id", id);
    } else {
        query.prepare(
            "INSERT INTO recent (operate_id, icon, name, protocol, type, file, time, description)"
            "VALUES (:operate_id, :icon, :name, :protocol, :type, :file, :time, :description)"
            );
        query.bindValue(":operate_id", item.szOperateId);
        query.bindValue(":icon", getIcon(item.icon));
        query.bindValue(":name", item.szName);
        query.bindValue(":protocol", item.szProtocol);
        query.bindValue(":type", item.szType);
        query.bindValue(":file", item.szFile);
        query.bindValue(":time", item.time);
        query.bindValue(":description", item.szDescription);
    }
    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to add recent:" << query.lastError().text();
        return 0;
    }

    qDebug(log) << "Insert recent id:" << query.lastInsertId().toInt();
    emit sigChanged();
    return query.lastInsertId().toInt();
}

bool CRecentDatabase::updateRecent(
    const QString &szFile, const QString& szName, const QString& szDescription)
{
    QSqlQuery query(m_database);

    query.prepare(
        "UPDATE recent SET "
        "name=:name, "
        "description=:description "
        "WHERE file=:file"
        );
    query.bindValue(":name", szName);
    query.bindValue(":file", szFile);
    query.bindValue(":description", szDescription);

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to update recent:" << query.lastError().text();
    }
    emit sigChanged();
    return success;
}

bool CRecentDatabase::deleteRecent(int id)
{
    if(0 >= id) return false;
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM recent WHERE id=:id");
    query.bindValue(":id", id);
    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to delete recent id:"
                       << id << query.lastError().text();
    }
    emit sigChanged();
    return success;
}

QList<CRecentDatabase::Item> CRecentDatabase::getRecents(int limit, int offset)
{
    QList<Item> items;

    QSqlQuery query(m_database);
    if(0 > limit) {
        query.prepare(
            "SELECT id, operate_id, icon, name, protocol, type, file, time, description "
            "FROM recent "
            "ORDER BY time DESC "
            );
    } else {
        query.prepare(
            "SELECT id, operate_id, icon, name, protocol, type, file, time, description "
            "FROM recent "
            "ORDER BY time DESC "
            "LIMIT :limit OFFSET :offset"
            );
        query.bindValue(":limit", limit);
        query.bindValue(":offset", offset);
    }
    if (query.exec()) {
        while (query.next()) {
            Item item;
            item.id = query.value(0).toInt();
            item.szOperateId = query.value(1).toString();
            item.icon = getIcon(query.value(2).toInt());
            item.szName = query.value(3).toString();
            item.szProtocol = query.value(4).toString();
            item.szType = query.value(5).toString();
            item.szFile = query.value(6).toString();
            item.time = query.value(7).toDateTime();
            item.szDescription = query.value(8).toString();
            items.append(item);
        }
    } else {
        qCritical(log) << "Failed to get recents:" << query.lastError().text();
    }

    return items;
}

int CRecentDatabase::getIcon(const QIcon &icon)
{
    bool bRet = false;
    QSqlQuery query(m_database);
    QString szName = icon.name();
    if(szName.isEmpty()) {
        QByteArray data = RabbitCommon::CIconUtils::iconToByteArray(icon);
        QString szHash = RabbitCommon::CIconUtils::hashIconData(data);
        query.prepare("SELECT id FROM icon WHERE hash=:hash");
        query.bindValue(":hash", szHash);
        bRet = query.exec();
        if(!bRet) {
            qCritical(log) << "Failed to select recent icon hash:"
                           << szHash << query.lastError().text();
            return 0;
        }
        if(query.next()) {
            return query.value(0).toInt();
        }
        query.prepare(
            "INSERT INTO icon (hash, data) "
            "VALUES (:hash, :data)"
            );
        query.bindValue(":hash", szHash);
        query.bindValue(":data", data);
        bRet = query.exec();
        if(!bRet) {
            qCritical(log) << "Failed to insert recent icon hash:"
                           << szHash << query.lastError().text();
            return 0;
        }
        return query.lastInsertId().toInt();
    }

    query.prepare("SELECT id FROM icon WHERE name=:name");
    query.bindValue(":name", szName);
    bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to select recent icon name:"
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
        qCritical(log) << "Failed to insert recent icon name:"
                       << szName << query.lastError().text();
        return 0;
    }
    return query.lastInsertId().toInt();
}

QIcon CRecentDatabase::getIcon(int id)
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
        qCritical(log) << "Failed to delete recent icon id:"
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
