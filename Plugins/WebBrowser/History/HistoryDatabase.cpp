// Author: Kang Lin <kl222@126.com>

#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QLoggingCategory>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "HistoryDatabase.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.History.DB")

#if HAVE_SQLITE
#include <sqlite3.h>  // 需要链接 sqlite3 库
// SQLite 回调函数
static void sqlTrace(void* /*data*/, const char* sql) {
    qDebug(log) << "SQL Trace:" << sql;
}

// 启用 SQL 跟踪
void enableSqlTrace(const QString& connectionName = QSqlDatabase::defaultConnection) {
    QSqlDatabase db = QSqlDatabase::database(connectionName);

    // 获取底层 SQLite 数据库句柄
    QVariant v = db.driver()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*") == 0) {
        sqlite3* handle = *static_cast<sqlite3**>(v.data());
        if (handle) {
            sqlite3_trace(handle, sqlTrace, nullptr);
            qDebug(log) << "SQLite trace enabled";
        }
    }
}
#else // HAVE_SQLITE
void enableSqlTrace(const QString& connectionName)
{
}
#endif // HAVE_SQLITE

CHistoryDatabase::CHistoryDatabase(QObject *parent)
    : CDatabase(parent)
{
    qDebug(log) << Q_FUNC_INFO;
}

CHistoryDatabase::~CHistoryDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
}

bool CHistoryDatabase::OnInitializeDatabase()
{
    bool success = false;
    success = CDatabase::OnInitializeDatabase();
    if(!success) return false;
    success = m_UrlDB.SetDatabase(GetDatabase(), m_pPara);
    return success;
}

bool CHistoryDatabase::addHistoryEntry(const QString &url)
{
    if (url.isEmpty()) return false;

    int nUrlId = 0;
    // 检查 URL 是否已存在。如果不存在，则增加
    nUrlId = m_UrlDB.GetId(url);
    if(0 == nUrlId) {
        nUrlId = m_UrlDB.AddUrl(url);
        if(0 == nUrlId)
            return false;
    }

    // 插入新记录
    QSqlQuery query(GetDatabase());
    query.prepare(
        "INSERT INTO history (url, visit_time) "
        "VALUES (:url, :visit_time) "
        );
    query.bindValue(":url", nUrlId);
    QDateTime tm = QDateTime::currentDateTime();
    query.bindValue(":visit_time", tm);

    bool success = query.exec();
    if (!success) {
        QString szErr = "Failed to add history: " + query.lastError().text()
                + "; Sql: " + query.executedQuery()
                + "; url: " + url;
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return success;
}

bool CHistoryDatabase::addHistoryEntry(const QString &url, const QString& title, const QDateTime& time)
{
    if (url.isEmpty()) return false;

    int nUrlId = 0;
    // 检查 URL 是否已存在。如果不存在，则增加
    nUrlId = m_UrlDB.GetId(url);
    if(0 == nUrlId) {
        nUrlId = m_UrlDB.AddUrl(url, title);
        if(0 == nUrlId)
            return false;
    }

    // 插入新记录
    QSqlQuery query(GetDatabase());
    query.prepare(
        "INSERT INTO history (url, visit_time) "
        "VALUES (:url, :visit_time) "
        );
    query.bindValue(":url", nUrlId);
    query.bindValue(":visit_time", time);

    bool success = query.exec();
    if (!success) {
        QString szErr = "Failed to add history: " + query.lastError().text()
                            + "; Sql: " + query.executedQuery()
                            + "; url: " + url;
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return success;
}

bool CHistoryDatabase::updateHistoryEntry(const QString& url, const QString &title, const QIcon &icon)
{
    if (url.isEmpty()) return false;

    int nUrlId = 0;
    nUrlId = m_UrlDB.GetId(url);
    if(0 == nUrlId) {
        nUrlId = m_UrlDB.AddUrl(url, title, icon);
        return (0 < nUrlId);
    }
    return m_UrlDB.UpdateUrl(url, title, icon);
}

bool CHistoryDatabase::updateHistoryEntry(int id, const QString &title, const QIcon &icon)
{
    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT url"
        "FROM history "
        "WHERE id=:id"
        );
    query.bindValue(":id", id);
    if(!query.exec()) {
        QString szErr = "Failed to update history: " + query.lastError().text()
                + "; Sql: " + query.executedQuery()
                + "; id: " + QString::number(id);
        SetError(szErr);
        qCritical(log) << GetError();
        return false;
    }

    if(query.next()) {
        int urlId = query.value(0).toInt();
        return m_UrlDB.UpdateUrl(urlId, title, icon);
    }

    return false;
}

bool CHistoryDatabase::deleteHistoryEntry(int id)
{
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM history WHERE id = :id");
    query.bindValue(":id", id);

    bool bRet = query.exec();
    if(!bRet) {
        QString szErr = "Failed to delete history: " + query.lastError().text()
                + "; Sql: " + query.executedQuery()
                + "; id: " + QString::number(id);
        SetError(szErr);
        qCritical(log) << GetError();
    }
    return bRet;
}

bool CHistoryDatabase::deleteHistoryEntry(const QString &url)
{
    int urlId = m_UrlDB.GetId(url);
    if(0 == urlId)
        return false;
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM history WHERE url = :url");
    query.bindValue(":url", urlId);
    bool bRet = query.exec();
    if(!bRet) {
        QString szErr = "Failed to delete history: " + query.lastError().text()
                + "; Sql: " + query.executedQuery()
                + "; url: " + url;
        SetError(szErr);
        qCritical(log) << GetError();
    }
    return bRet;
}

bool CHistoryDatabase::deleteDomainEntries(const QString &szDomain)
{
    if(szDomain.isEmpty()) return false;
    auto domains = m_UrlDB.GetDomain(szDomain);
    foreach(auto urlId, domains) {
        QSqlQuery query(GetDatabase());
        query.prepare("DELETE FROM history WHERE url = :url");
        query.bindValue(":url", urlId);
        if(!query.exec()) {
            QString szErr = "Failed to delete domain: " + szDomain
                            + "; Error: " + query.lastError().text()
                            + "; Sql: " + query.executedQuery()
                            + "; url id: " + QString::number(urlId);
            SetError(szErr);
            qCritical(log) << GetError();
        }
        m_UrlDB.DeleteUrl(urlId);
    }
    return true;
}

bool CHistoryDatabase::clearHistory(int days)
{
    QSqlQuery query(GetDatabase());

    if (days > 0) {
        QDateTime cutoff = QDateTime::currentDateTime().addDays(-days);
        query.prepare("DELETE FROM history WHERE visit_time < :cutoff");
        query.bindValue(":cutoff", cutoff);
    } else {
        query.prepare("DELETE FROM history");
    }

    bool success = query.exec();
    if (success) {
        // 清理后重置自增ID
        query.exec("VACUUM");
    } else {
        QString szErr = "Failed to clear history: " + query.lastError().text()
                        + "; Sql: " + query.executedQuery();
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return success;
}

void CHistoryDatabase::scheduleCleanup(int maxDays, int maxCount)
{
    // 按时间清理
    if (maxDays > 0) {
        QSqlQuery query(GetDatabase());
        QDateTime cutoff = QDateTime::currentDateTime().addDays(-maxDays);
        query.prepare("DELETE FROM history WHERE visit_time < :cutoff");
        query.bindValue(":cutoff", cutoff);
        query.exec();
    }

    // 按数量清理
    if (maxCount > 0) {
        QSqlQuery query(GetDatabase());
        query.prepare(
            "DELETE FROM history WHERE id IN ("
            "    SELECT id FROM history "
            "    ORDER BY visit_time DESC "
            "    LIMIT -1 OFFSET :maxCount"
            ")"
            );
        query.bindValue(":maxCount", maxCount);
        query.exec();
    }
}

QList<HistoryItem> CHistoryDatabase::getAllHistory(int limit, int offset)
{
    QList<HistoryItem> historyList;

    QSqlQuery query(GetDatabase());
    if(0 > limit) {
        query.prepare(
            "SELECT id, url, visit_time "
            "FROM history "
            "ORDER BY visit_time DESC "
            );
    } else {
        query.prepare(
            "SELECT id, url, visit_time "
            "FROM history "
            "ORDER BY visit_time DESC "
            "LIMIT :limit OFFSET :offset"
            );
        query.bindValue(":limit", limit);
        query.bindValue(":offset", offset);
    }
    if (query.exec()) {
        while (query.next()) {
            HistoryItem item;
            item.id = query.value(0).toInt();
            CDatabaseUrl::UrlItem urlItem = m_UrlDB.GetItem(query.value(1).toInt());
            item.url = urlItem.szUrl;
            item.title = urlItem.szTitle;
            item.visitTime = query.value(2).toDateTime();
            item.icon = urlItem.icon;
            historyList.append(item);
        }
    } else {
        QString szErr = "Failed to get all history:" + query.lastError().text()
                        + "; Sql: " + query.executedQuery();
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return historyList;
}

QList<HistoryItem> CHistoryDatabase::getHistoryByDate(const QDate &date)
{
    QList<HistoryItem> historyList;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, url, visit_time "
        "FROM history "
        "WHERE date(visit_time) = date(:date) "
        "ORDER BY visit_time DESC"
        );
    query.bindValue(":date", date.toString("yyyy-MM-dd"));

    if (query.exec()) {
        while (query.next()) {
            HistoryItem item;
            item.id = query.value(0).toInt();
            CDatabaseUrl::UrlItem urlItem = m_UrlDB.GetItem(query.value(1).toInt());
            item.url = urlItem.szUrl;
            item.title = urlItem.szTitle;
            item.visitTime = query.value(2).toDateTime();
            item.icon = urlItem.icon;
            historyList.append(item);
        }
    } else {
        QString szErr = "Failed to get history by date:" + query.lastError().text()
            + "; Sql: " + query.executedQuery();
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return historyList;
}

QList<HistoryItem> CHistoryDatabase::getHistoryByDate(
    const QDate &start, const QDate &end, int limit)
{
    QList<HistoryItem> historyList;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, url, visit_time "
        "FROM history "
        "WHERE date(visit_time) >= date(:start) AND date(visit_time) <= date(:end) "
        "ORDER BY visit_time DESC "
        "LIMIT :limit"
        );
    query.bindValue(":start", start.toString("yyyy-MM-dd"));
    query.bindValue(":end", end.toString("yyyy-MM-dd"));
    query.bindValue(":limit", limit);

    if (query.exec()) {
        while (query.next()) {
            HistoryItem item;
            item.id = query.value(0).toInt();
            CDatabaseUrl::UrlItem urlItem = m_UrlDB.GetItem(query.value(1).toInt());
            item.url = urlItem.szUrl;
            item.title = urlItem.szTitle;
            item.visitTime = query.value(2).toDateTime();
            item.icon = urlItem.icon;
            historyList.append(item);
        }
    } else {
        QString szErr = "Failed to get history by date:" + query.lastError().text()
            + "; Sql: " + query.executedQuery();
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return historyList;
}

QList<HistoryItem> CHistoryDatabase::searchHistory(const QString &keyword)
{
    auto items = m_UrlDB.Search(keyword);
    QList<HistoryItem> retItems;
    foreach (auto i, items) {
        HistoryItem item;
        item.icon = i.icon;
        item.title = i.szTitle;
        item.url = i.szUrl;
        retItems << item;
    }
    return retItems;
}

HistoryItem CHistoryDatabase::getHistoryByUrl(const QString &url)
{
    HistoryItem item;

    if(url.isEmpty()) return item;
    auto urlItem = m_UrlDB.GetItem(url);
    if(0 >= urlItem.id)
        return item;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, visit_time "
        "FROM history WHERE url = :url"
        );
    query.bindValue(":url", urlItem.id);

    if (query.exec()) {
        if(query.next()) {
            item.id = query.value(0).toInt();
            item.url = urlItem.szUrl;
            item.title = urlItem.szTitle;
            item.icon = urlItem.icon;
            item.visitTime = query.value(1).toDateTime();
        }
    } else {
        QString szErr = "Failed to get history by url:" + query.lastError().text()
            + "; Sql: " + query.executedQuery()
            + "; Url: " + url;
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return item;
}

HistoryItem CHistoryDatabase::getHistoryById(int id)
{
    HistoryItem item;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, url, visit_time "
        "FROM history WHERE id = :id"
        );
    query.bindValue(":id", id);
    
    if (query.exec()) {
        if(query.next()) {
            item.id = query.value(0).toInt();
            int urlId = query.value(1).toInt();
            auto urlItem = m_UrlDB.GetItem(urlId);
            item.url = urlItem.szUrl;
            item.title = urlItem.szTitle;
            item.icon = urlItem.icon;
            item.visitTime = query.value(2).toDateTime();
        }
    } else {
        QString szErr = "Failed to get history by id:" + query.lastError().text()
        + "; Sql: " + query.executedQuery()
            + "; id: " + QString::number(id);
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return item;
}

int CHistoryDatabase::getHistoryCount()
{
    QSqlQuery query(GetDatabase());
    query.exec("SELECT COUNT(*) FROM history");

    if (query.next()) {
        return query.value(0).toInt();
    } else {
        QString szErr = "Failed to get history count:" + query.lastError().text()
                        + "; Sql: " + query.executedQuery();
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return 0;
}

QDateTime CHistoryDatabase::getLastVisitTime()
{
    QSqlQuery query(GetDatabase());
    query.exec("SELECT MAX(visit_time) FROM history");

    if (query.next()) {
        return query.value(0).toDateTime();
    } else {
        QString szErr = "Failed to get last visit time:" + query.lastError().text()
            + "; Sql: " + query.executedQuery();
        SetError(szErr);
        qCritical(log) << GetError();
    }

    return QDateTime();
}

bool CHistoryDatabase::ExportToJson(QJsonObject &obj)
{
    QJsonArray list;
    auto items = getAllHistory();
    foreach(auto it, items) {
        QJsonObject title;
        title.insert("title", it.title);
        title.insert("url", it.url);
        title.insert("visit_time", it.visitTime.toString());
        list.append(title);
    }
    obj.insert("browser_history", list);
    return true;
}

bool CHistoryDatabase::ImportFromJson(const QJsonObject &obj)
{
    auto array = obj["browser_history"].toArray();
    if(array.isEmpty()) {
        SetError(tr("The file format is error. Json without \"browser_history\""));
        qCritical(log) << GetError();
        return false;
    }
    for(auto it = array.begin(); it != array.end(); it++) {
        auto o = it->toObject();
        QString url = o["url"].toString();
        QString title = o["title"].toString();
        QDateTime time = QDateTime::fromString(o["visit_time"].toString());
        qDebug(log) << "title:" << title << "url:" << url << "visit_time:" << time;
        bool ok = addHistoryEntry(url, title, time);
        if(!ok)
            qWarning(log) << "Failed to add history:" << o["title"].toString();
    }
    return true;
}

bool CHistoryDatabase::OnInitializeSqliteDatabase()
{
    QSqlQuery query(GetDatabase());
    
    // 创建历史记录表
    query.prepare(
        "CREATE TABLE IF NOT EXISTS history ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    url INTEGER NOT NULL,"
        "    visit_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        );
    bool success = query.exec();
    
    if (!success) {
        QString szErr = "Failed to create history sqlite table:"
                        + query.lastError().text()
                        + "; Sql: " + query.executedQuery();
        SetError(szErr);
        qCritical(log) << GetError();
        return false;
    }
    
    // 创建索引
    query.prepare("CREATE INDEX IF NOT EXISTS idx_history_url ON history(url)");
    success = query.exec();
    if (!success) {
        qWarning(log) << "Failed to create index idx_history_url:"
                       << query.lastError().text()
                       << query.executedQuery();
    }
    query.prepare("CREATE INDEX IF NOT EXISTS idx_history_time ON history(visit_time)");
    success = query.exec();
    if (!success) {
        qWarning(log) << "Failed to create index idx_history_time:"
                      << query.lastError().text()
                      << query.executedQuery();
    }
    
    return true;
}

bool CHistoryDatabase::OnInitializeMySqlDatabase()
{
    QSqlQuery query(GetDatabase());
    query.prepare(
        "CREATE TABLE IF NOT EXISTS history ("
        "    id INTEGER PRIMARY KEY AUTO_INCREMENT,"
        "    url INTEGER NOT NULL,"
        "    visit_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    INDEX idx_history_url (url),"
        "    INDEX idx_history_time (visit_time)"
        ")"
        );
    bool success = query.exec();
    if (!success) {
        QString szErr = "Failed to create history mysql table:"
                       + query.lastError().text()
                       + "; Sql: " + query.executedQuery();
        SetError(szErr);
        qCritical(log) << GetError();
    }
    return success;
}
