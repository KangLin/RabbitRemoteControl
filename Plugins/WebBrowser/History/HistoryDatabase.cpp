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

CHistoryDatabase* CHistoryDatabase::Instance(const QString &szPath)
{
    static CHistoryDatabase* p = nullptr;
    if(!p) {
        p = new CHistoryDatabase();
        if(p) {
            bool bRet = p->OpenSQLiteDatabase("history_connection", szPath);
            if(!bRet) {
                delete p;
                p = nullptr;
            }
        }
    }
    return p;
}

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
    QSqlQuery query(GetDatabase());

    // 创建历史记录表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS history ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    url INTEGER NOT NULL,"
        "    visit_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        );

    if (!success) {
        qCritical(log) << "Failed to create table:" << query.lastError().text();
        return false;
    }

    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_url ON history(url)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_time ON history(visit_time)");

    m_UrlDB.SetDatabase(GetDatabase());
    m_UrlDB.OnInitializeDatabase();
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
        "VALUES (:url, :visit_time)"
        );
    query.bindValue(":url", nUrlId);
    QDateTime tm = QDateTime::currentDateTime();
    query.bindValue(":visit_time", tm);

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to add history:" << query.lastError().text();
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
        "VALUES (:url, :visit_time)"
        );
    query.bindValue(":url", nUrlId);
    query.bindValue(":visit_time", time);

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to add history:" << query.lastError().text();
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
    if(query.exec() && query.next()) {
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

    return query.exec();
}

bool CHistoryDatabase::deleteHistoryEntry(const QString &url)
{
    int urlId = m_UrlDB.GetId(url);
    if(0 == urlId)
        return false;
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM history WHERE url = :url");
    query.bindValue(":url", urlId);
    return query.exec();
}

bool CHistoryDatabase::deleteDomainEntries(const QString &szDomain)
{
    if(szDomain.isEmpty()) return false;
    auto domains = m_UrlDB.GetDomain(szDomain);
    foreach(auto urlId, domains) {
        QSqlQuery query(GetDatabase());
        query.prepare("DELETE FROM history WHERE url = :url");
        query.bindValue(":url", urlId);
        if(!query.exec())
            qCritical(log) << "Failed to delete domain:" << szDomain << "url id:" << urlId;
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
        qCritical(log) << "Failed to get all history:" << query.lastError().text();
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

    if (query.exec() && query.next()) {
        item.id = query.value(0).toInt();
        item.url = urlItem.szUrl;
        item.title = urlItem.szTitle;
        item.icon = urlItem.icon;
        item.visitTime = query.value(1).toDateTime();
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

    if (query.exec() && query.next()) {
        item.id = query.value(0).toInt();
        int urlId = query.value(1).toInt();
        auto urlItem = m_UrlDB.GetItem(urlId);
        item.url = urlItem.szUrl;
        item.title = urlItem.szTitle;
        item.icon = urlItem.icon;
        item.visitTime = query.value(2).toDateTime();
    }

    return item;
}

int CHistoryDatabase::getHistoryCount()
{
    QSqlQuery query(GetDatabase());
    query.exec("SELECT COUNT(*) FROM history");

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

QDateTime CHistoryDatabase::getLastVisitTime()
{
    QSqlQuery query(GetDatabase());
    query.exec("SELECT MAX(visit_time) FROM history");

    if (query.next()) {
        return query.value(0).toDateTime();
    }

    return QDateTime();
}

bool CHistoryDatabase::importFromJson(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    do {
        QJsonDocument doc;
        doc = QJsonDocument::fromJson(file.readAll());
        if(!doc.isArray())
            break;
        auto array = doc.array();
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
    } while(0);

    file.close();
    return true;
}

bool CHistoryDatabase::exportToJson(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out.setGenerateByteOrderMark(true);  // 添加 UTF-8 BOM

    QJsonDocument doc;
    QJsonArray list;
    auto items = getAllHistory();
    foreach(auto it, items) {
        QJsonObject title;
        title.insert("title", it.title);
        title.insert("url", it.url);
        title.insert("visit_time", it.visitTime.toString());
        list.append(title);
    }
    doc.setArray(list);
    out << doc.toJson();

    file.close();
    return true;
}

bool CHistoryDatabase::importFromCSV(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical(log) << "Failed to open file" << filename;
        return false;
    }

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif
    in.setGenerateByteOrderMark(true);  // 添加 UTF-8 BOM
    int importedCount = 0;
    int lineNumber = 0;

    // 开始事务
    GetDatabase().transaction();

    try {
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            lineNumber++;

            // 跳过空行和注释
            if (line.isEmpty() || line.startsWith("#")) {
                continue;
            }

            // 跳过表头（第一行）
            if (lineNumber == 1) {
                // 验证表头格式
                if (!validateCsvHeader(line)) {
                    throw QString("Invalid CSV header format");
                }
                continue;
            }

            // 解析 CSV 行
            QStringList fields = parseCsvLine(line);

            if (fields.size() >= 3) {
                if (importCsvRecord(fields)) {
                    importedCount++;
                }
            } else {
                qWarning() << "Invalid CSV line" << lineNumber << ":" << line;
            }
        }

        file.close();

        if (importedCount == 0) {
            throw QString("No valid records found in CSV file");
        }

        if (!GetDatabase().commit()) {
            throw QString("Failed to commit transaction: %1").arg(GetDatabase().lastError().text());
        }

        qDebug(log) << "Successfully imported" << importedCount << "records from CSV file";
        return true;
    } catch (const QString &error) {
        GetDatabase().rollback();
        file.close();
        qCritical(log) << "CSV import failed at line" << lineNumber << ":" << error;
        return false;
    }
    return false;
}

bool CHistoryDatabase::exportToCSV(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out.setGenerateByteOrderMark(true);  // 添加 UTF-8 BOM

    // 写入表头
    const QStringList headers = {
        "Title", "URL", "Visit Time"
    };
    out << headers.join(",") << "\n";

    auto items = getAllHistory();
    foreach(auto it, items) {
            QStringList row;
            row << escapeForCsv(it.title);
            row << escapeForCsv(it.url);
            row << it.visitTime.toString();
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

QString CHistoryDatabase::unescapeCsvField(const QString &field)
{
    if (field.isEmpty()) {
        return QString();
    }

    QString unescaped = field;

    // 去除包围的引号
    if (unescaped.startsWith('"') && unescaped.endsWith('"')) {
        unescaped = unescaped.mid(1, unescaped.length() - 2);
    }

    // 反转义双引号
    unescaped.replace("\"\"", "\"");

    return unescaped.trimmed();
}

QStringList CHistoryDatabase::parseCsvLine(const QString &line)
{
    QStringList fields;
    QString field;
    bool inQuotes = false;

    for (int i = 0; i < line.length(); ++i) {
        QChar ch = line[i];

        if (ch == '"') {
            // 处理转义的双引号
            if (i + 1 < line.length() && line[i + 1] == '"') {
                field += '"';
                i++;  // 跳过下一个引号
            } else {
                inQuotes = !inQuotes;
            }
        } else if (ch == ',' && !inQuotes) {
            fields.append(field.trimmed());
            field.clear();
        } else {
            field += ch;
        }
    }

    // 添加最后一个字段
    if (!field.isEmpty()) {
        fields.append(field.trimmed());
    }

    return fields;
}

bool CHistoryDatabase::validateCsvHeader(const QString &headerLine)
{
    QStringList headers = parseCsvLine(headerLine);

    // 检查必需的表头
    if (headers.size() < 3) {
        return false;
    }

    // 检查关键字段
    QStringList requiredHeaders = {"Title", "URL", "Visit Time"};
    for (const QString &required : requiredHeaders) {
        if (!headers.contains(required, Qt::CaseInsensitive)) {
            qWarning(log) << "Missing required header:" << required;
            return false;
        }
    }

    return true;
}

bool CHistoryDatabase::importCsvRecord(const QStringList &fields)
{
    if (fields.size() < 3) {
        return false;
    }

    HistoryItem item;

    // 解析字段
    // 字段顺序：Title, URL, Visit Time

    // Title（可选）
    item.title = fields[0];
    // URL（必需）
    item.url = fields[1];

    // Visit Time（必需）
    item.visitTime = QDateTime::fromString(fields[2]);
    if (!item.visitTime.isValid()) {
        // 尝试其他格式
        item.visitTime = QDateTime::fromString(fields[2]);
        if (!item.visitTime.isValid()) {
            item.visitTime = QDateTime::currentDateTime();
        }
    }

    return addHistoryEntry(item.url, item.title, item.visitTime);
}

bool CHistoryDatabase::ExportToJson(QJsonObject &obj)
{
    return true;
}

bool CHistoryDatabase::ImportFromJson(const QJsonObject &obj)
{
    return true;
}
