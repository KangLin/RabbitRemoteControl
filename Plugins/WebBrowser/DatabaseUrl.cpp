// Author: Kang Lin <kl222@126.com>

#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QLoggingCategory>
#include "DatabaseUrl.h"

static Q_LOGGING_CATEGORY(log, "DB.Url")
CDatabaseUrl::CDatabaseUrl(const QString &szSuffix, QObject *parent)
    : CDatabase{parent}
{
    m_szTableName = "url";
    if(!szSuffix.isEmpty()) {
        m_szTableName = m_szTableName + "_" + szSuffix;
    }
    m_szConnectName = "connect_" + m_szTableName;
}

bool CDatabaseUrl::OnInitializeDatabase()
{
    bool success = false;
    success = CDatabase::OnInitializeDatabase();
    if(!success) return success;
    success = m_iconDB.SetDatabase(GetDatabase(), m_pPara);
    return success;
}

bool CDatabaseUrl::OnInitializeSqliteDatabase()
{
    QSqlQuery query(GetDatabase());
    bool bRet = query.prepare(
        "CREATE TABLE IF NOT EXISTS " + m_szTableName + " ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    url TEXT UNIQUE NOT NULL,"
        "    title TEXT,"
        "    icon INTEGER DEFAULT 0,"
        "    visit_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        );
    if (!bRet) {
        SetError("Failed to create " + m_szTableName + " sqlite table. Error: " + query.lastError().text());
        qCritical(log) << GetError();
        return false;
    }
    bRet = query.exec();
    if (!bRet) {
        SetError("Failed to create " + m_szTableName + " sqlite table: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        return false;
    }
    
    // 创建索引
    query.prepare("CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_url ON " + m_szTableName + "(url)");
    bRet = query.exec();
    if (!bRet) {
        qWarning(log) << "Failed to create index idx_" + m_szTableName + "_url:"
                      << query.lastError().text()
                      << query.executedQuery();
    }
    
    return true;
}

bool CDatabaseUrl::OnInitializeMySqlDatabase()
{
    QSqlQuery query(GetDatabase());
    
    // 创建历史记录表
    bool bRet = query.prepare(
        "CREATE TABLE IF NOT EXISTS `" + m_szTableName + "` ("
        "    `id` INTEGER PRIMARY KEY AUTO_INCREMENT,"
        "    `url` TEXT NOT NULL,"
        "    `title` TEXT,"
        "    `icon` INTEGER DEFAULT 0,"
        "    `visit_time` DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    UNIQUE KEY `idx_url` (`url`(255))"
        ")"
        );
    if (!bRet) {
        SetError("Failed to create " + m_szTableName + " mysql table. Error: " + query.lastError().text());
        qCritical(log) << GetError();
        return false;
    }
    bRet = query.exec();
    if (!bRet) {
        SetError("Failed to create " + m_szTableName + " mysql table. Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
        return false;
    }
    
    return bRet;
}

int CDatabaseUrl::AddUrl(const QString &url, const QString &title, const QIcon &icon)
{
    int nId = 0;
    if (url.isEmpty()) return 0;

    QSqlQuery query(GetDatabase());

    // 检查URL是否已存在
    query.prepare("SELECT id, title, icon FROM " + m_szTableName + " WHERE url = :url");
    query.bindValue(":url", url);
    if (!query.exec()) {
        SetError("Failed to exec: " + query.executedQuery()
                 + "; Error: " + query.lastError().text());
        qCritical(log) << GetError();
        return -1;
    }
    if(query.next()) {
        // 更新现有记录
        nId = query.value(0).toInt();
        QString szTitle = query.value(1).toString();
        if(!title.isEmpty())
            szTitle = title;
        int iconID = query.value(2).toInt();
        if(!icon.isNull())
            iconID = m_iconDB.GetIcon(icon);
        query.prepare(
            "UPDATE " + m_szTableName + " SET "
            "title = :title, "
            "icon = :icon, "
            "visit_time = :visit_time "
            "WHERE id = :id"
            );
        query.bindValue(":title", szTitle);
        query.bindValue(":icon", iconID);
        query.bindValue(":visit_time", QDateTime::currentDateTime());
        query.bindValue(":id", nId);
    } else {
        // 插入新记录
        query.prepare(
            "INSERT INTO " + m_szTableName + " (url, title, icon) "
            "VALUES (:url, :title, :icon)"
            );
        query.bindValue(":url", url);
        QString szTitle = title;
        if(szTitle.isEmpty())
            szTitle = url;
        query.bindValue(":title", szTitle);
        query.bindValue(":icon", m_iconDB.GetIcon(icon));
    }

    qDebug(log) << "Sql:" << query.executedQuery();
    qDebug(log) << "Bound values:" << query.boundValues();
    bool success = query.exec();
    if (!success) {
        nId = 0;
        SetError("Failed to add " + m_szTableName + ": " + url
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
    } else {
        if(0 == nId)
            nId = query.lastInsertId().toInt();
    }

    return nId;
}

bool CDatabaseUrl::DeleteUrl(const QString &url)
{
    if (url.isEmpty()) return false;

    QSqlQuery query(GetDatabase());
    query.prepare("DELETE from " + m_szTableName + " WHERE url = :url");
    query.bindValue(":url", url);
    bool bRet = query.exec();
    if (!bRet) {
        SetError("Failed to delete " + m_szTableName + ": " + url
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
    }

    return bRet;
}

bool CDatabaseUrl::DeleteUrl(int id)
{
    if (0 >= id) return false;

    QSqlQuery query(GetDatabase());

    query.prepare("DELETE from " + m_szTableName + " WHERE id = :id");
    query.bindValue(":id", id);

    bool success = query.exec();
    if (!success) {
        SetError("Failed to delete " + m_szTableName + ": " + QString::number(id)
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
    }

    return success;
}

bool CDatabaseUrl::UpdateUrl(const QString &url, const QString &title, const QIcon &icon)
{
    if (url.isEmpty()) return false;
    if(title.isEmpty() && icon.isNull()) return false;

    QString szSql;
    szSql += "visit_time=\"" + QDateTime::currentDateTime().toString(Qt::ISODate) + "\" ";
    if(!title.isEmpty())
        szSql += ", title=\"" + title + "\" ";
    if(!icon.isNull()) {
        szSql += ", icon=" + QString::number(m_iconDB.GetIcon(icon)) + " ";
    }

    szSql = "UPDATE " + m_szTableName + " SET " + szSql;
    szSql += " WHERE url=\"" + url + "\"";
    
    QSqlQuery query(GetDatabase());
    bool success = query.exec(szSql);
    if (!success) {
        SetError("Failed to update url: " + query.lastError().text() + "; Sql: " + szSql);
        qCritical(log) << GetError();
    }

    return success;
}

bool CDatabaseUrl::UpdateUrl(int id, const QString &title, const QIcon &icon)
{
    if (0 >= id) return false;
    if(title.isEmpty() && icon.isNull()) return false;
    
    QString szSql;
    szSql += "visit_time=\"" + QDateTime::currentDateTime().toString(Qt::ISODate) + "\" ";
    if(!title.isEmpty())
        szSql += ", title=\"" + title + "\" ";
    if(!icon.isNull()) {
        szSql += ", icon=" + QString::number(m_iconDB.GetIcon(icon)) + " ";
    }
    szSql = "UPDATE " + m_szTableName + " SET " + szSql;
    szSql += " WHERE id = " + QString::number(id);

    QSqlQuery query(GetDatabase());
    query.prepare(szSql);
    query.bindValue(":id", id);
    bool success = query.exec();
    if (!success) {
        SetError("Failed to update url: " + query.lastError().text() + "; Sql: " + szSql);
        qCritical(log) << GetError();
    }

    return success;
}

CDatabaseUrl::UrlItem CDatabaseUrl::GetItem(int id)
{
    UrlItem item;
    if (0 >= id) return item;

    QSqlQuery query(GetDatabase());
    query.prepare("SELECT url, title, icon, visit_time FROM " + m_szTableName +
                  " WHERE id = :id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        item.id = id;
        item.szUrl = query.value(0).toString();
        item.szTitle = query.value(1).toString();
        item.iconId = query.value(2).toInt();
        item.icon = m_iconDB.GetIcon(item.iconId);
        item.visit_time = query.value(3).toDateTime();
    }
    return item;
}

CDatabaseUrl::UrlItem CDatabaseUrl::GetItem(const QString& url)
{
    UrlItem item;
    if (url.isEmpty()) return item;

    QSqlQuery query(GetDatabase());
    query.prepare("SELECT id, title, icon, visit_time FROM " + m_szTableName +
                  " WHERE url = :url");
    query.bindValue(":url", url);
    if (query.exec() && query.next()) {
        item.szUrl = url;
        item.id = query.value(0).toInt();
        item.szTitle = query.value(1).toString();
        item.iconId = query.value(2).toInt();
        item.icon = m_iconDB.GetIcon(item.iconId);
        item.visit_time = query.value(3).toDateTime();
    }
    return item;
}

int CDatabaseUrl::GetId(const QString& url)
{
    if (url.isEmpty()) return 0;
    QSqlQuery query(GetDatabase());
    bool bRet = query.prepare("SELECT id, title, icon FROM " + m_szTableName +
                  " WHERE url = :url");
    if(!bRet) {
        SetError("Failed to get id prepare: " + query.lastError().text() + "; url: " + url);
        qCritical(log) << GetError();
        return 0;
    }
    query.bindValue(":url", url);
    bRet = query.exec();
    if (!bRet) {
        SetError("Failed to get id: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery() + "; url: " + url);
        qCritical(log) << GetError();
    }
    if(query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

QList<int> CDatabaseUrl::GetDomain(const QString &szDomain)
{
    QList<int> ret;
    QSqlQuery query(GetDatabase());
    query.prepare("SELECT id FROM " + m_szTableName + " WHERE url LIKE :url");
    query.bindValue(":url", QString("%%://%%%1%%").arg(szDomain));
    bool bRet = query.exec();
    if (bRet) {
        while(query.next()) {
            ret << query.value(0).toInt();
        }
    } else {
        qCritical(log) << "Failed to get domain:"
                       << query.lastError().text()
                       << query.executedQuery();
    }
    return ret;
}

QList<CDatabaseUrl::UrlItem> CDatabaseUrl::Search(const QString &keyword)
{
    QList<UrlItem> items;

    QSqlQuery query(GetDatabase());
    QString searchPattern = "%" + keyword + "%";
    query.prepare(
        "SELECT id, url, title, visit_time, icon "
        "FROM " + m_szTableName + " "
        "WHERE url LIKE :pattern OR title LIKE :pattern "
        "ORDER BY visit_time DESC"
        );
    query.bindValue(":pattern", searchPattern);
    if (query.exec()) {
        while (query.next()) {
            UrlItem item;
            item.id = query.value(0).toInt();
            item.szUrl = query.value(1).toString();
            item.szTitle = query.value(2).toString();
            item.visit_time = query.value(3).toDateTime();
            item.icon = m_iconDB.GetIcon(query.value(4).toInt());
            items.append(item);
        }
    } else {
        SetError("Failed to search:" + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
    }

    return items;
}

bool CDatabaseUrl::ExportToJson(QJsonObject &obj)
{
    return true;
}

bool CDatabaseUrl::ImportFromJson(const QJsonObject &obj)
{
    return true;
}
