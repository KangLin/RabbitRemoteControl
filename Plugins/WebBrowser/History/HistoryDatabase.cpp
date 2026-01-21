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
        "    url TEXT NOT NULL,"
        "    title TEXT NOT NULL,"
        "    icon INTEGER DEFAULT 0,"
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

    m_iconDB.SetDatabase(GetDatabase());
    m_iconDB.OnInitializeDatabase();
    return true;
}

bool CHistoryDatabase::addHistoryEntry(const QString &url, const QString &title, const QIcon &icon, bool bSingle)
{
    if (url.isEmpty()) return false;

    QSqlQuery query(GetDatabase());

    // 检查URL是否已存在
    query.prepare("SELECT id, visit_count, icon FROM history WHERE url = :url");
    query.bindValue(":url", url);

    // 直接打印查询信息
    qDebug(log) << "Executed query:" << query.executedQuery();
    qDebug(log) << "Bound values:" << query.boundValues();

    if (bSingle && query.exec() && query.next()) {
        // 更新现有记录
        int id = query.value(0).toInt();
        int visitCount = query.value(1).toInt() + 1;
        int iconID = query.value(2).toInt();
        if(!icon.isNull())
            iconID = m_iconDB.GetIcon(icon);

        query.prepare(
            "UPDATE history SET "
            "title = :title, "
            "visit_count = :visit_count, "
            "last_visit_time = :last_visit_time "
            "icon = :icon "
            "WHERE id = :id"
            );
        query.bindValue(":title", title);
        query.bindValue(":visit_count", visitCount);
        query.bindValue(":last_visit_time", QDateTime::currentDateTime());
        query.bindValue(":icon", iconID);
        query.bindValue(":id", id);
    } else {
        // 插入新记录
        query.prepare(
            "INSERT INTO history (url, title, icon, visit_time, visit_count, last_visit_time) "
            "VALUES (:url, :title, :icon, :visit_time, 1, :last_visit_time)"
            );
        query.bindValue(":url", url);
        query.bindValue(":title", title);
        query.bindValue(":icon", m_iconDB.GetIcon(icon));
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
    QSqlQuery query(GetDatabase());
    QStringList lstFields;
    if(!title.isEmpty())
        lstFields << "title = '" + title + "'";
    int iconId = m_iconDB.GetIcon(icon);
    if(!icon.isNull() && iconId > 0)
        lstFields << "icon = " + QString::number(iconId);
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
    QSqlQuery query(GetDatabase());
    QStringList lstFields;
    if(!title.isEmpty())
        lstFields << "title = '" + title + "'";
    int iconId = m_iconDB.GetIcon(icon);
    if(!icon.isNull() && iconId > 0)
        lstFields << "icon = " + QString::number(iconId);
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
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM history WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec();
}

bool CHistoryDatabase::deleteHistoryEntry(const QString &url)
{
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM history WHERE url = :url");
    query.bindValue(":url", url);

    return query.exec();
}

bool CHistoryDatabase::deleteDomainEntries(const QString &szDomain)
{
    QSqlQuery query(GetDatabase());
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
            "SELECT id, url, title, visit_time, visit_count, last_visit_time, icon "
            "FROM history "
            "ORDER BY visit_time DESC "
            );
    } else {
        query.prepare(
            "SELECT id, url, title, visit_time, visit_count, last_visit_time, icon "
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
            item.url = query.value(1).toString();
            item.title = query.value(2).toString();
            item.visitTime = query.value(3).toDateTime();
            item.visitCount = query.value(4).toInt();
            item.lastVisitTime = query.value(5).toDateTime();
            item.iconId = query.value(6).toInt();
            item.icon = m_iconDB.GetIcon(item.iconId);
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
        "SELECT id, url, title, visit_time, visit_count, last_visit_time, icon "
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
            item.iconId = query.value(6).toInt();
            item.icon = m_iconDB.GetIcon(item.iconId);
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
        "SELECT id, url, title, visit_time, visit_count, last_visit_time, icon "
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
            item.iconId = query.value(6).toInt();
            item.icon = m_iconDB.GetIcon(item.iconId);
            historyList.append(item);
        }
    }

    return historyList;
}

QList<HistoryItem> CHistoryDatabase::searchHistory(const QString &keyword)
{
    QList<HistoryItem> historyList;

    QSqlQuery query(GetDatabase());
    QString searchPattern = "%" + keyword + "%";
    query.prepare(
        "SELECT id, url, title, visit_time, visit_count, last_visit_time, icon "
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
            item.iconId = query.value(6).toInt();
            item.icon = m_iconDB.GetIcon(item.iconId);
            historyList.append(item);
        }
    }

    return historyList;
}

HistoryItem CHistoryDatabase::getHistoryByUrl(const QString &url)
{
    HistoryItem item;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, url, title, visit_time, visit_count, last_visit_time, icon "
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
        item.iconId = query.value(6).toInt();
        item.icon = m_iconDB.GetIcon(item.iconId);
    }

    return item;
}

HistoryItem CHistoryDatabase::getHistoryById(int id)
{
    HistoryItem item;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, url, title, visit_time, visit_count, last_visit_time, icon "
        "FROM history WHERE id = :id"
        );
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        item.id = query.value(0).toInt();
        item.url = query.value(1).toString();
        item.title = query.value(2).toString();
        item.visitTime = query.value(3).toDateTime();
        item.visitCount = query.value(4).toInt();
        item.lastVisitTime = query.value(5).toDateTime();
        item.iconId = query.value(6).toInt();
        item.icon = m_iconDB.GetIcon(item.iconId);
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

            if (fields.size() >= 6) {
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
        "ID", "URL", "Title", "Icon", "Visit Time",
        "Visit Count", "Last Visit Time"
    };
    out << headers.join(",") << "\n";

    QList<HistoryItem> items = getAllHistory();
    foreach (const auto &item, items) {
        QStringList row;
        row << QString::number(item.id);
        row << escapeForCsv(item.url);
        row << escapeForCsv(item.title);
        row << QString::number(item.iconId);
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
    if (headers.size() < 6) {
        return false;
    }

    // 检查关键字段
    QStringList requiredHeaders = {"URL", "Title", "Visit Time"};
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
    if (fields.size() < 6) {
        return false;
    }

    HistoryItem item;

    // 解析字段
    // 字段顺序：ID, URL, Title, Icon, Visit Time, Visit Count, Last Visit Time

    // ID（可选）
    if (!fields[0].isEmpty()) {
        bool ok;
        item.id = fields[0].toInt(&ok);
        if (!ok) item.id = 0;
    }

    // URL（必需）
    item.url = unescapeCsvField(fields[1]);
    if (item.url.isEmpty()) {
        return false;
    }

    // Title（必需）
    item.title = unescapeCsvField(fields[2]);
    if (item.title.isEmpty()) {
        item.title = item.url;  // 使用URL作为标题
    }

    // Icon
    item.iconId = fields[3].toInt();

    // Visit Time（必需）
    item.visitTime = QDateTime::fromString(fields[4], Qt::ISODate);
    if (!item.visitTime.isValid()) {
        // 尝试其他格式
        item.visitTime = QDateTime::fromString(fields[4], Qt::TextDate);
        if (!item.visitTime.isValid()) {
            item.visitTime = QDateTime::currentDateTime();
        }
    }

    // Visit Count（可选）
    if (fields.size() > 5 && !fields[5].isEmpty()) {
        bool ok;
        item.visitCount = fields[5].toInt(&ok);
        if (!ok) item.visitCount = 1;
    } else {
        item.visitCount = 1;
    }

    // Last Visit Time（可选）
    if (fields.size() > 6 && !fields[6].isEmpty()) {
        item.lastVisitTime = QDateTime::fromString(fields[6], Qt::ISODate);
        if (!item.lastVisitTime.isValid()) {
            item.lastVisitTime = item.visitTime;
        }
    } else {
        item.lastVisitTime = item.visitTime;
    }

    // 检查是否已存在
    HistoryItem existing = getHistoryById(item.id);
    if (existing.id > 0 && existing.url == item.url) {
        // 更新现有记录
        existing.title = item.title;
        existing.visitCount += item.visitCount;
        existing.lastVisitTime = item.visitTime > existing.lastVisitTime ?
                                     item.visitTime : existing.lastVisitTime;
        // existing.faviconUrl = item.faviconUrl.isEmpty() ?
        //                           existing.faviconUrl : item.faviconUrl;

        return updateHistoryEntry(item.id, item.title, m_iconDB.GetIcon(item.iconId));
    } else {
        // 插入新记录
        return addHistoryEntry(item.url, item.title, m_iconDB.GetIcon(item.iconId));
    }
}
