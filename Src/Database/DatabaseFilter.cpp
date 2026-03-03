#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>

#include "DatabaseFilter.h"

static Q_LOGGING_CATEGORY(log, "DB.Filter")
CDatabaseFilter::CDatabaseFilter(const QString& szSuffix, QObject *parent)
    : CDatabase{parent}
    , m_szTableName("filter")
{
    if(!szSuffix.isEmpty()) {
        QString szSuf = szSuffix;
        szSuf = szSuf.replace("/", "_");
        m_szTableName = m_szTableName + "_" + szSuf;
        m_szConnectName = "connect_filter_" + szSuf;
    }
}

bool CDatabaseFilter::contains(const QString& szKey)
{
    bool bRet = false;
    QSqlQuery query(GetDatabase());

    // Check if it already exists
    query.prepare("SELECT `key` FROM " + m_szTableName
                  + " WHERE `key` = :key "
                  );
    query.bindValue(":key", szKey);
    bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to contains[" + szKey + "]:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }
    return query.next();
}

int CDatabaseFilter::AddKey(const QString &szKey)
{

    if(contains(szKey)) return false;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "INSERT INTO " + m_szTableName + " (`key`) VALUES (:key) "
        );
    query.bindValue(":key", szKey);
    bool bRet = query.exec();

    if (!bRet) {
        qCritical(log) << "Failed to add key[" + szKey + "]:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return -1;
    }
    return 0;
}

int CDatabaseFilter::RemoveKey(const QString &szKey)
{
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM " + m_szTableName + " WHERE `key` = :key");
    query.bindValue(":key", szKey);

    bool bRet = query.exec();
    if (!bRet) {
        qCritical(log) << "Failed to remove key[" + szKey + "]:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return -1;
    }
    return 0;
}

int CDatabaseFilter::Clear()
{
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM " + m_szTableName);
    bool bRet = query.exec();
    if (!bRet) {
        qCritical(log) << "Failed to clear:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return -1;
    }
    return 0;
}

bool CDatabaseFilter::isEmpty()
{
    QSqlQuery query(GetDatabase());
    query.prepare("SELECT `key` FROM " + m_szTableName);
    bool bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to isEmpty:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return true;
    }
    return !query.next();
}

int CDatabaseFilter::OnProcess(std::function<int (const QString &)> cb, bool bErrExit)
{
    QSqlQuery query(GetDatabase());
    query.prepare("SELECT `key` FROM " + m_szTableName);
    bool bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to isEmpty:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return -1;
    }
    while(query.next()) {
        QString szKey = query.value(0).toString();
        int nRet = cb(szKey);
        if(bErrExit && nRet)
            return nRet;
    }
    return 0;
}

bool CDatabaseFilter::ExportToJson(QJsonObject &obj)
{
    return true;
}

bool CDatabaseFilter::ImportFromJson(const QJsonObject &obj)
{
    return true;
}

bool CDatabaseFilter::OnInitializeSqliteDatabase()
{
    QSqlQuery query(GetDatabase());

    query.prepare(
        "CREATE TABLE IF NOT EXISTS "
        + m_szTableName +
        " ("
        "    `key` TEXT PRIMARY KEY NOT NULL"
        " )"
        );
    bool success = query.exec();

    if (!success) {
        qCritical(log) << "Failed to create sqlite table:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }
    return true;
}

bool CDatabaseFilter::OnInitializeMySqlDatabase()
{
    QSqlQuery query(GetDatabase());

    query.prepare(
        "CREATE TABLE IF NOT EXISTS "
        + m_szTableName +
        " ("
        "    `key` TEXT NOT NULL,"
        "    UNIQUE KEY `uk_key` (`key`(255))"
        " )"
        );
    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to create mysql table:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }
    return true;
}
