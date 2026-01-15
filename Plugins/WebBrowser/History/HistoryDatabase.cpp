// Author: Kang Lin <kl222@126.com>

#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QLoggingCategory>
#include <QCoreApplication>
#include "RabbitCommonDir.h"
#include "HistoryDatabase.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.History.Db")

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
    : QObject(parent)
{
    qDebug(log) << Q_FUNC_INFO;
}

CHistoryDatabase::~CHistoryDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    closeDatabase();
}

bool CHistoryDatabase::openDatabase(const QString &dbPath)
{
    QString databasePath;
    if (dbPath.isEmpty()) {
        // 使用默认路径
        QString dataDir = RabbitCommon::CDir::Instance()->GetDirDatabase();
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(dataDir);
        }
        databasePath = dir.filePath("browser_history.db");
    } else {
        databasePath = dbPath;
    }

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", "history_connection");
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        qCritical(log) << "Failed to open database:" << m_database.lastError().text() << ";Library paths:" << QCoreApplication::libraryPaths();;
        return false;
    }

    return initializeDatabase();
}

bool CHistoryDatabase::isOpen()
{
    return m_database.isOpen();
}

void CHistoryDatabase::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase("history_connection");
}

bool CHistoryDatabase::initializeDatabase()
{
    QSqlQuery query(m_database);

    // 创建历史记录表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS history ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    url TEXT NOT NULL,"
        "    title TEXT NOT NULL,"
        "    visit_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    visit_count INTEGER DEFAULT 1,"
        "    last_visit_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        );

    if (!success) {
        qWarning() << "Failed to create table:" << query.lastError().text();
        return false;
    }

    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_url ON history(url)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_time ON history(visit_time)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_title ON history(title)");

    return true;
}

bool CHistoryDatabase::addHistoryEntry(const QString &url, const QString &title, bool bSingle)
{
    if (url.isEmpty()) return false;

    QSqlQuery query(m_database);

    // 检查URL是否已存在
    query.prepare("SELECT id, visit_count FROM history WHERE url = :url");
    query.bindValue(":url", url);

    // 直接打印查询信息
    qDebug(log) << "Executed query:" << query.executedQuery();
    qDebug(log) << "Bound values:" << query.boundValues();

    if (bSingle && query.exec() && query.next()) {
        // 更新现有记录
        int id = query.value(0).toInt();
        int visitCount = query.value(1).toInt() + 1;

        query.prepare(
            "UPDATE history SET "
            "title = :title, "
            "visit_count = :visit_count, "
            "last_visit_time = :last_visit_time "
            "WHERE id = :id"
            );
        query.bindValue(":title", title);
        query.bindValue(":visit_count", visitCount);
        query.bindValue(":last_visit_time", QDateTime::currentDateTime());
        query.bindValue(":id", id);
    } else {
        // 插入新记录
        query.prepare(
            "INSERT INTO history (url, title, visit_time, visit_count, last_visit_time) "
            "VALUES (:url, :title, :visit_time, 1, :last_visit_time)"
            );
        query.bindValue(":url", url);
        query.bindValue(":title", title);
        QDateTime tm = QDateTime::currentDateTime();
        query.bindValue(":visit_time", tm);
        query.bindValue(":last_visit_time", tm);
    }

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to add history:" << query.lastError().text();
    }

    return success;
}

bool CHistoryDatabase::updateHistoryEntry(const QString& url, const QString &title, const QIcon &icon)
{
    QSqlQuery query(m_database);
    QStringList lstFields;
    if(!title.isEmpty())
        lstFields << "title = '" + title + "'";
    //TODO: add icon
    // if(!icon.isNull())
    //     lstFields << "icon = " + icon;
    if(lstFields.isEmpty())
        return false;

    QString szSql;
    szSql = "UPDATE history SET ";
    szSql += lstFields.join(",");
    szSql += " WHERE url = '" + url + "'";
    qDebug(log) << szSql;
    bool bRet = query.exec(szSql);
    if (!bRet) {
        qCritical(log) << "Failed to update history:" << query.lastError().text();
    }
    return bRet;
}

bool CHistoryDatabase::updateHistoryEntry(int id, const QString &title, const QIcon &icon)
{
    QSqlQuery query(m_database);
    QStringList lstFields;
    if(!title.isEmpty())
        lstFields << "title = '" + title + "'";
    //TODO: add icon
    // if(!icon.isNull())
    //     lstFields << "icon = " + icon;
    if(lstFields.isEmpty())
        return false;

    QString szSql;
    szSql = "UPDATE history SET ";
    szSql += lstFields.join(",");
    szSql += " WHERE id = " + QString::number(id);
    qDebug(log) << szSql;
    bool bRet = query.exec(szSql);
    if (!bRet) {
        qCritical(log) << "Failed to update history:" << query.lastError().text();
    }
    return bRet;
}

bool CHistoryDatabase::deleteHistoryEntry(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM history WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec();
}

bool CHistoryDatabase::deleteHistoryEntry(const QString &url)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM history WHERE url = :url");
    query.bindValue(":url", url);

    return query.exec();
}

bool CHistoryDatabase::deleteDomainEntries(const QString &szDomain)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM history WHERE url LIKE :url");
    query.bindValue(":url", QString("%%://%%%1%%").arg(szDomain));

    qDebug(log) << "Executed query:" << query.executedQuery();
    qDebug(log) << "Bound values:" << query.boundValues();

    bool bRet = query.exec();
    if (bRet) {
        int deletedCount = query.numRowsAffected();
        qDebug(log) << "Deleted" << deletedCount << "records for domain" << szDomain;
    } else {
        qCritical(log) << "Failed to delete domain history:" << query.lastError().text();
    }

    return bRet;
}

bool CHistoryDatabase::clearHistory(int days)
{
    QSqlQuery query(m_database);

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
    }

    return success;
}

void CHistoryDatabase::scheduleCleanup(int maxDays, int maxCount)
{
    // 按时间清理
    if (maxDays > 0) {
        QSqlQuery query(m_database);
        QDateTime cutoff = QDateTime::currentDateTime().addDays(-maxDays);
        query.prepare("DELETE FROM history WHERE visit_time < :cutoff");
        query.bindValue(":cutoff", cutoff);
        query.exec();
    }

    // 按数量清理
    if (maxCount > 0) {
        QSqlQuery query(m_database);
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

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, url, title, visit_time, visit_count, last_visit_time "
        "FROM history "
        "ORDER BY visit_time DESC "
        "LIMIT :limit OFFSET :offset"
        );
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (query.exec()) {
        while (query.next()) {
            HistoryItem item;
            item.id = query.value(0).toInt();
            item.url = query.value(1).toString();
            item.title = query.value(2).toString();
            item.visitTime = query.value(3).toDateTime();
            item.visitCount = query.value(4).toInt();
            item.lastVisitTime = query.value(5).toDateTime();
            historyList.append(item);
        }
    }

    return historyList;
}

QList<HistoryItem> CHistoryDatabase::getHistoryByDate(const QDate &date)
{
    QList<HistoryItem> historyList;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, url, title, visit_time, visit_count, last_visit_time "
        "FROM history "
        "WHERE date(visit_time) = date(:date) "
        "ORDER BY visit_time DESC"
        );
    query.bindValue(":date", date.toString("yyyy-MM-dd"));

    if (query.exec()) {
        while (query.next()) {
            HistoryItem item;
            item.id = query.value(0).toInt();
            item.url = query.value(1).toString();
            item.title = query.value(2).toString();
            item.visitTime = query.value(3).toDateTime();
            item.visitCount = query.value(4).toInt();
            item.lastVisitTime = query.value(5).toDateTime();
            historyList.append(item);
        }
    }

    return historyList;
}

QList<HistoryItem> CHistoryDatabase::getHistoryByDate(
    const QDate &start, const QDate &end, int limit)
{
    QList<HistoryItem> historyList;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, url, title, visit_time, visit_count, last_visit_time "
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
            item.url = query.value(1).toString();
            item.title = query.value(2).toString();
            item.visitTime = query.value(3).toDateTime();
            item.visitCount = query.value(4).toInt();
            item.lastVisitTime = query.value(5).toDateTime();
            historyList.append(item);
        }
    }

    return historyList;
}

QList<HistoryItem> CHistoryDatabase::searchHistory(const QString &keyword)
{
    QList<HistoryItem> historyList;

    QSqlQuery query(m_database);
    QString searchPattern = "%" + keyword + "%";
    query.prepare(
        "SELECT id, url, title, visit_time, visit_count, last_visit_time "
        "FROM history "
        "WHERE url LIKE :pattern OR title LIKE :pattern "
        "ORDER BY visit_time DESC"
        );
    query.bindValue(":pattern", searchPattern);

    if (query.exec()) {
        while (query.next()) {
            HistoryItem item;
            item.id = query.value(0).toInt();
            item.url = query.value(1).toString();
            item.title = query.value(2).toString();
            item.visitTime = query.value(3).toDateTime();
            item.visitCount = query.value(4).toInt();
            item.lastVisitTime = query.value(5).toDateTime();
            historyList.append(item);
        }
    }

    return historyList;
}

HistoryItem CHistoryDatabase::getHistoryByUrl(const QString &url)
{
    HistoryItem item;

    QSqlQuery query(m_database);
    query.prepare(
        "SELECT id, url, title, visit_time, visit_count, last_visit_time "
        "FROM history WHERE url = :url"
        );
    query.bindValue(":url", url);

    if (query.exec() && query.next()) {
        item.id = query.value(0).toInt();
        item.url = query.value(1).toString();
        item.title = query.value(2).toString();
        item.visitTime = query.value(3).toDateTime();
        item.visitCount = query.value(4).toInt();
        item.lastVisitTime = query.value(5).toDateTime();
    }

    return item;
}

int CHistoryDatabase::getHistoryCount()
{
    QSqlQuery query(m_database);
    query.exec("SELECT COUNT(*) FROM history");

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

QDateTime CHistoryDatabase::getLastVisitTime()
{
    QSqlQuery query(m_database);
    query.exec("SELECT MAX(visit_time) FROM history");

    if (query.next()) {
        return query.value(0).toDateTime();
    }

    return QDateTime();
}

bool CHistoryDatabase::exportToCSV(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out.setGenerateByteOrderMark(true);  // 添加 UTF-8 BOM

    // 写入表头
    const QStringList headers = {
        "ID", "URL", "Title", "Visit Time",
        "Visit Count", "Last Visit Time"
    };
    out << headers.join(",") << "\n";

    QList<HistoryItem> items = getAllHistory(0, 0);
    foreach (const auto &item, items) {
        QStringList row;
        row << QString::number(item.id);
        row << escapeForCsv(item.url);
        row << escapeForCsv(item.title);
        row << item.visitTime.toString(Qt::ISODate);
        row << QString::number(item.visitCount);
        row << item.lastVisitTime.toString(Qt::ISODate);

        out << row.join(",") << "\n";
    }

    file.close();
    return true;
}

QString CHistoryDatabase::escapeForCsv(const QString &text)
{
    if (text.isEmpty())
        return "\"\"";

    // 判断是否需要引号
    bool needQuotes = text.contains(',') ||
                      text.contains('"') ||
                      text.contains('\n') ||
                      text.contains('\r') ||
                      text.contains('\t') ||
                      text.startsWith(' ') ||
                      text.endsWith(' ') ||
                      text.startsWith('\t') ||
                      text.endsWith('\t');

    if (!needQuotes)
        return text;

    // 转义双引号
    QString escaped = text;
    escaped.replace("\"", "\"\"");

    return "\"" + escaped + "\"";
}
