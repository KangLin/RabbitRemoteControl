// Author: Kang Lin <kl222@126.com>

#include <QIcon>
#include <QSqlQuery>
#include <QSqlError>
#include <QLoggingCategory>
#include "RecentDatabase.h"

static Q_LOGGING_CATEGORY(log, "App.Recent.Db")
CRecentDatabase::CRecentDatabase(QObject *parent)
    : CDatabase{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CRecentDatabase::~CRecentDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    closeDatabase();
}

bool CRecentDatabase::onInitializeDatabase()
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
    m_IconDB.setDatabase(m_database);
    m_IconDB.onInitializeDatabase();
    return true;
}

int CRecentDatabase::addRecent(const RecentItem &item)
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
        query.bindValue(":icon", m_IconDB.getIcon(item.icon));
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
    return success;
}

QList<CRecentDatabase::RecentItem> CRecentDatabase::getRecents(int limit, int offset)
{
    QList<RecentItem> items;

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
            RecentItem item;
            item.id = query.value(0).toInt();
            item.szOperateId = query.value(1).toString();
            item.icon = m_IconDB.getIcon(query.value(2).toInt());
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
