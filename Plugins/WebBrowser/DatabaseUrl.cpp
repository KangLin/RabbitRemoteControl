// Author: Kang Lin <kl222@126.com>

#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QLoggingCategory>
#include "DatabaseUrl.h"

static Q_LOGGING_CATEGORY(log, "DB.Url")
CDatabaseUrl::CDatabaseUrl(QObject *parent)
    : CDatabase{parent}
{}


bool CDatabaseUrl::OnInitializeDatabase()
{
    bool success = false;
    success = CDatabase::OnInitializeDatabase();
    if(!success) return success;
    m_iconDB.SetDatabase(GetDatabase());
    success = m_iconDB.OnInitializeDatabase();
    return success;
}

int CDatabaseUrl::AddUrl(const QString &url, const QString &title, const QIcon &icon)
{
    int nId = 0;
    if (url.isEmpty()) return 0;

    QSqlQuery query(GetDatabase());

    // 检查URL是否已存在
    query.prepare("SELECT id, title, icon FROM url WHERE url = :url");
    query.bindValue(":url", url);

    if (query.exec() && query.next()) {
        // 更新现有记录
        nId = query.value(0).toInt();
        QString szTitle = query.value(1).toString();
        if(!title.isEmpty())
            szTitle = title;
        int iconID = query.value(2).toInt();
        if(!icon.isNull())
            iconID = m_iconDB.GetIcon(icon);

        query.prepare(
            "UPDATE url SET "
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
            "INSERT INTO url (url, title, icon) "
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
        qCritical(log) << "Failed to add url:" << url << query.lastError().text();
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

    query.prepare("DELETE from url WHERE url = :url");
    query.bindValue(":url", url);

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to delete url:" << url << query.lastError().text();
    }

    return success;
}

bool CDatabaseUrl::DeleteUrl(int id)
{
    if (0 >= id) return false;

    QSqlQuery query(GetDatabase());

    query.prepare("DELETE from url WHERE id = :id");
    query.bindValue(":id", id);

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to delete url:" << id << query.lastError().text();
    }

    return success;
}

bool CDatabaseUrl::UpdateUrl(const QString &url, const QString &title, const QIcon &icon)
{
    if (url.isEmpty()) return false;
    if(title.isEmpty() && icon.isNull()) return false;

    QSqlQuery query(GetDatabase());

    QString szSql;
    szSql += "visit_time=\"" + QDateTime::currentDateTime().toString() + "\" ";
    if(!title.isEmpty())
        szSql += ", title=\"" + title + "\" ";
    if(!icon.isNull()) {
        szSql += ", icon=" + QString::number(m_iconDB.GetIcon(icon)) + " ";
    }

    szSql = "UPDATE url SET " + szSql;
    szSql += "WHERE url=\"" + url + "\"";

    bool success = query.exec(szSql);
    if (!success) {
        qCritical(log) << "Failed to update url:" << szSql << query.lastError().text();
    }

    return success;
}

bool CDatabaseUrl::UpdateUrl(int id, const QString &title, const QIcon &icon)
{
    if (0 >= id) return false;

    if(title.isEmpty() && icon.isNull()) return false;
    QString szSql;
    szSql += "visit_time=\"" + QDateTime::currentDateTime().toString() + "\" ";
    if(!title.isEmpty())
        szSql += ", title=\"" + title + "\" ";
    if(!icon.isNull()) {
        szSql += ", icon=" + QString::number(m_iconDB.GetIcon(icon)) + " ";
    }

    QSqlQuery query(GetDatabase());
    query.prepare("SELECT title, icon FROM url WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        if(szSql.isEmpty())
            return false;

        szSql = "UPDATE url SET " + szSql;
        szSql += "WHERE id = " + QString::number(id);
    } else {
        qCritical(log) << "Failed to update url, url is not exist:" << id;
        return false;
    }

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to update url:" << szSql << query.lastError().text();
    }

    return success;
}

CDatabaseUrl::UrlItem CDatabaseUrl::GetItem(int id)
{
    UrlItem item;
    if (0 >= id) return item;

    QSqlQuery query(GetDatabase());

    query.prepare("SELECT url, title, icon, visit_time FROM url "
                  "WHERE id = :id");
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

    query.prepare("SELECT id, title, icon, visit_time FROM url "
                  "WHERE url = :url");
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

    query.prepare("SELECT id, title, icon FROM url "
                  "WHERE url = :url");
    query.bindValue(":url", url);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

QList<int> CDatabaseUrl::GetDomain(const QString &szDomain)
{
    QList<int> ret;
    QSqlQuery query(GetDatabase());
    query.prepare("SELECT id FROM url WHERE url LIKE :url");
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
        "FROM url "
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
        qCritical(log) << "Failed to search:"
                       << query.lastError().text()
                       << query.executedQuery();
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

bool CDatabaseUrl::OnInitializeSqliteDatabase()
{
    QSqlQuery query(GetDatabase());
    
    // 创建历史记录表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS url ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    url TEXT UNIQUE NOT NULL,"
        "    title TEXT,"
        "    icon INTEGER DEFAULT 0,"
        "    visit_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        );
    
    if (!success) {
        qCritical(log) << "Failed to create url table:"
                       << query.lastError().text()
                       << query.executedQuery();
        return false;
    }
    
    // 创建索引
    success = query.exec("CREATE INDEX IF NOT EXISTS idx_url_url ON url(url)");
    if (!success) {
        qWarning(log) << "Failed to create index idx_url_url:"
                       << query.lastError().text()
                       << query.executedQuery();
    }

    return true;
}

bool CDatabaseUrl::OnInitializeMySqlDatabase()
{
    QSqlQuery query(GetDatabase());
    
    // 创建历史记录表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS url ("
        "    id INTEGER PRIMARY KEY AUTO_INCREMENT,"
        "    url TEXT NOT NULL,"
        "    title TEXT,"
        "    icon INTEGER DEFAULT 0,"
        "    visit_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    INDEX idx_url_url (url)"
        ")"
        );
    
    if (!success) {
        qCritical(log) << "Failed to create url table:"
                       << query.lastError().text()
                       << query.executedQuery();
        return false;
    }

    return success;
}
