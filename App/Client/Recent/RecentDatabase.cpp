// Author: Kang Lin <kl222@126.com>

#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>
#include "RecentDatabase.h"
#include "IconUtils.h"
#include "RabbitCommonDir.h"

static Q_LOGGING_CATEGORY(log, "App.Recent.Db")
CRecentDatabase::CRecentDatabase(QObject *parent)
    : CDatabase{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CRecentDatabase::~CRecentDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    CloseDatabase();
}

bool CRecentDatabase::OnInitializeDatabase()
{
    bool bRet = false;
    bRet = CDatabase::OnInitializeDatabase();
    if(!bRet) return false;

    // Create icon table
    m_IconDB.SetDatabase(GetDatabase(), m_pPara);
    bRet = m_IconDB.OnInitializeDatabase();
    if(!bRet) return bRet;
    m_FileDB.SetDatabase(GetDatabase(), m_pPara);
    bRet = m_FileDB.OnInitializeDatabase();
    return bRet;
}

bool CRecentDatabase::OnInitializeSqliteDatabase()
{
    QSqlQuery query(GetDatabase());

    // Create recent table
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS recent ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    operate_id TEXT NOT NULL,"
        "    icon INTEGER DEFAULT 0,"
        "    name TEXT NOT NULL,"
        "    protocol TEXT,"
        "    operate_type TEXT,"
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
    success = query.exec("CREATE INDEX IF NOT EXISTS idx_recent_file ON recent(file)");
    if(!success)  {
        qWarning(log) << "Failed to drop index idx_recent_file:"
                      << query.lastError().text()
                      << "Sql:" << query.executedQuery();
    }

    if (!query.exec("DROP TRIGGER IF EXISTS delete_icon_after_recent")) {
        qDebug(log) << "Failed to drop trigger delete_icon_after_recent:"
                    << query.lastError().text()
                    << "Sql:" << query.executedQuery();
    }

    QString szSql = R"(
        CREATE TRIGGER delete_icon_after_recent
        AFTER DELETE ON recent
        FOR EACH ROW
        BEGIN
            DELETE FROM icon
            WHERE id = OLD.icon
              AND OLD.icon != 0
              AND NOT EXISTS (
                  SELECT 1 FROM favorite WHERE icon = OLD.icon
              )
              AND NOT EXISTS (
                  SELECT 1 FROM recent WHERE icon = OLD.icon
              );
        END;
    )";
    success = query.exec(szSql);
    if (!success) {
        qWarning(log) << "Failed to create trigger delete_icon_after_recent."
                      << query.lastError().text()
                      << "Sql:" << query.executedQuery();
    }

    return true;
}

bool CRecentDatabase::OnInitializeMySqlDatabase()
{
    bool success = false;
    QSqlQuery query(GetDatabase());

    // Create recent table
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS recent ("
        "    id INTEGER PRIMARY KEY AUTO_INCREMENT,"
        "    operate_id TEXT NOT NULL,"
        "    icon INTEGER DEFAULT 0,"
        "    name TEXT NOT NULL,"
        "    protocol TEXT,"
        "    operate_type TEXT,"
        "    file TEXT NOT NULL,"
        "    time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    description TEXT,"
        "    UNIQUE KEY uk_recent_file (file(255))"
        ")"
        );
    if (!success) {
        qCritical(log) << "Failed to create recent table:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

    // Create trigger
    if (!query.exec("DROP TRIGGER IF EXISTS delete_icon_after_recent")) {
        qDebug(log) << "Failed to drop trigger delete_icon_after_recent:"
                    << query.lastError().text()
                    << "Sql:" << query.executedQuery();
    }

    QString szSql = R"(
        CREATE TRIGGER delete_icon_after_recent
        AFTER DELETE ON recent
        FOR EACH ROW
        BEGIN
            DECLARE favorite_count INT DEFAULT 0;
            DECLARE recent_count INT DEFAULT 0;
            
            IF OLD.icon != 0 THEN
                -- 统计favorite表中引用该icon的数量
                SELECT COUNT(*) INTO favorite_count 
                FROM favorite 
                WHERE icon = OLD.icon;
                
                -- 统计recent表中引用该icon的数量
                SELECT COUNT(*) INTO recent_count 
                FROM recent 
                WHERE icon = OLD.icon;
                
                -- 如果都没有引用，则删除icon
                IF favorite_count = 0 AND recent_count = 0 THEN
                    DELETE FROM icon WHERE id = OLD.icon;
                END IF;
            END IF;
        END
    )";
    success = query.exec(szSql);
    if (!success) {
        qWarning(log) << "Failed to create trigger delete_icon_after_recent."
                      << query.lastError().text()
                      << "Sql:" << query.executedQuery();
    }

    return true;
}

int CRecentDatabase::AddRecent(const RecentItem &item)
{
    QSqlQuery query(GetDatabase());
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
            "INSERT INTO recent (operate_id, icon, name, protocol, operate_type, file, time, description)"
            "VALUES (:operate_id, :icon, :name, :protocol, :type, :file, :time, :description)"
            );
        query.bindValue(":operate_id", item.szOperateId);
        query.bindValue(":icon", m_IconDB.GetIcon(item.icon));
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

bool CRecentDatabase::UpdateRecent(
    const QString &szFile, const QString& szName, const QString& szDescription)
{
    QSqlQuery query(GetDatabase());

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

bool CRecentDatabase::DeleteRecent(int id)
{
    if(0 >= id) return false;
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM recent WHERE id=:id");
    query.bindValue(":id", id);
    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to delete recent id:"
                       << id << query.lastError().text();
    }
    return success;
}

QList<CRecentDatabase::RecentItem> CRecentDatabase::GetRecents(int limit, int offset)
{
    QList<RecentItem> items;

    QSqlQuery query(GetDatabase());
    if(0 > limit) {
        query.prepare(
            "SELECT id, operate_id, icon, name, protocol, operate_type, file, time, description "
            "FROM recent "
            "ORDER BY time DESC "
            );
    } else {
        query.prepare(
            "SELECT id, operate_id, icon, name, protocol, operate_type, file, time, description "
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
            item.icon = m_IconDB.GetIcon(query.value(2).toInt());
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

bool CRecentDatabase::ExportToJson(QJsonObject &obj)
{
    QJsonArray recents;
    auto items = GetRecents();
    foreach(auto it, items) {
        QJsonObject itemObj;

        bool bRet = CDatabaseFile::ExportFileToJson(it.szFile, itemObj);
        if(!bRet) continue;

        itemObj.insert("OperateId", it.szOperateId);

        bRet = CDatabaseIcon::ExportIconToJson(it.icon, itemObj);
        if(!bRet) continue;

        itemObj.insert("Name", it.szName);
        itemObj.insert("Protocol", it.szProtocol);
        itemObj.insert("Type", it.szType);
        itemObj.insert("Time", it.time.toString());
        itemObj.insert("Description", it.szDescription);

        recents.append(itemObj);
    }
    if(recents.isEmpty())
        return false;
    obj.insert("Recent", recents);
    return true;
}

bool CRecentDatabase::ImportFromJson(const QJsonObject &obj)
{
    QJsonArray recents = obj["Recent"].toArray();
    if(recents.isEmpty()){
        qCritical(log) << "The file format is error. Don't find recents";
        return false;
    }

    for(auto it = recents.begin(); it != recents.end(); it++) {
        QJsonObject itemObj = it->toObject();
        RecentItem item;

        bool bRet = m_FileDB.ImportFileToDatabaseFromJson(itemObj, item.szFile);
        if(!bRet) continue;

        // Check if is exist in recent
        QSqlQuery query(GetDatabase());
        query.prepare(
            "SELECT id FROM recent "
            "WHERE file=:file"
            );
        query.bindValue(":file", item.szFile);
        if (query.exec() && query.next()) {
            continue;
        }

        bRet = CDatabaseIcon::ImportIconFromJson(itemObj, item.icon);
        if(!bRet) continue;

        item.szOperateId = itemObj["OperateId"].toString();
        item.szName = itemObj["Name"].toString();
        item.szProtocol = itemObj["Protocol"].toString();
        item.szType = itemObj["Type"].toString();

        item.time = QDateTime::fromString(itemObj["Time"].toString());
        if(!item.time.isValid())
            item.time = QDateTime::currentDateTime();

        item.szDescription = itemObj["Description"].toString();
        AddRecent(item);
    }
    return true;
}
