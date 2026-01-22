// Author: Kang Lin <kl222@126.com>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QLoggingCategory>
#include "RabbitCommonDir.h"
#include "IconUtils.h"
#include "Database.h"

static Q_LOGGING_CATEGORY(log, "DB")
CDatabase::CDatabase(QObject *parent)
    : QObject{parent}
{
    qDebug(log) << Q_FUNC_INFO;
    m_szConnectName = "connect";
}

CDatabase::~CDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    CloseDatabase();
}

void CDatabase::SetDatabase(QSqlDatabase db)
{
    m_database = db;
}

QSqlDatabase CDatabase::GetDatabase() const
{
    return m_database;
}

bool CDatabase::OpenDatabase(const QString &connectionName, const QString &dbPath)
{
    QString databasePath;
    if (dbPath.isEmpty()) {
        // 使用默认路径
        QString dataDir = RabbitCommon::CDir::Instance()->GetDirUserDatabase();
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(dataDir);
        }
        databasePath = dir.filePath("database.db");
    } else {
        databasePath = dbPath;
    }

    if(IsOpen())
        CloseDatabase();

    if(!connectionName.isEmpty())
        m_szConnectName = connectionName;

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", m_szConnectName);
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        qCritical(log) << "Failed to open database:"
                       << m_database.lastError().text()
                       << m_database.connectionName() << databasePath;
        return false;
    }

    qInfo(log) << "Open database connect:" << m_database.connectionName() << "File:" << databasePath;
    return OnInitializeDatabase();
}

bool CDatabase::IsOpen() const
{
    return m_database.isOpen();
}

void CDatabase::CloseDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase(m_szConnectName);
}

CDatabaseIcon::CDatabaseIcon(QObject *parent)
    : CDatabase(parent)
{
    m_szConnectName = "icon_connect";
    m_szTableName = "icon";
}

CDatabaseIcon::CDatabaseIcon(const QString &szPrefix, QObject *parent)
    : CDatabaseIcon(parent)
{
    if(!szPrefix.isEmpty())
        m_szTableName = szPrefix + "_" + m_szTableName;
}

bool CDatabaseIcon::OnInitializeDatabase()
{
    QSqlQuery query(GetDatabase());

    // Create icon table
    QString szSql =
        "CREATE TABLE IF NOT EXISTS " + m_szTableName + " ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT UNIQUE,"       // Icon name. see QIcon::name()
        "    hash TEXT,"              // Icon hash value
        "    data BLOB,"              // Icon binary data
        "    visit_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        ;
    bool success = query.exec(szSql);
    if (!success) {
        qCritical(log) << "Failed to create icon table:" << m_szTableName << query.lastError().text();
        return false;
    }
    query.exec("CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_name ON " + m_szTableName + "(name)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_hash ON " + m_szTableName + "(hash)");
    return true;
}

int CDatabaseIcon::GetIcon(const QIcon &icon)
{
    bool bRet = false;
    if(icon.isNull()) return 0;

    QString szSql;
    QSqlQuery query(GetDatabase());
    QString szName = icon.name();
    if(szName.isEmpty()) {
        // Check hash and data
        QByteArray data = RabbitCommon::CIconUtils::iconToByteArray(icon);
        QString szHash = RabbitCommon::CIconUtils::hashIconData(data);
        if(data.isEmpty() || szHash.isEmpty())
            return 0;
        szSql = "SELECT id, data FROM " + m_szTableName + " WHERE hash=:hash";
        query.prepare(szSql);
        query.bindValue(":hash", szHash);
        // qDebug(log) << "prepare:" << query.executedQuery();
        // qDebug(log) << "Bound values:" << query.boundValues();
        bRet = query.exec();
        if(!bRet) {
            qCritical(log) << "Failed to select icon hash:"
                           << szHash << query.lastError().text();
            return 0;
        }
        while (query.next()) {
            // check a same data
            if(data == query.value(1).toByteArray()) {
                return query.value(0).toInt();
            }
        }

        szSql = "INSERT INTO " + m_szTableName + " (hash, data) "
                "VALUES (:hash, :data)";
        query.prepare(szSql);
        query.bindValue(":hash", szHash);
        query.bindValue(":data", data);
        bRet = query.exec();
        if(!bRet) {
            qCritical(log) << "Failed to insert icon hash:"
                           << szHash << query.lastError().text();
            return 0;
        }
        return query.lastInsertId().toInt();
    }

    // Check name
    szSql = "SELECT id FROM " + m_szTableName + " WHERE name=:name";
    query.prepare(szSql);
    query.bindValue(":name", szName);
    bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to select icon name:"
                       << szName << query.lastError().text();
        return 0;
    }
    if(query.next()) {
        return query.value(0).toInt();
    }

    // Insert icon
    szSql = "INSERT INTO " + m_szTableName + " (name) VALUES (:name)";
    query.prepare(szSql);
    query.bindValue(":name", szName);
    bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to insert icon name:"
                       << szName << query.lastError().text();
        return 0;
    }
    return query.lastInsertId().toInt();
}

QIcon CDatabaseIcon::GetIcon(int id)
{
    QIcon icon;
    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT name, data FROM " + m_szTableName +
        " WHERE id=:id "
        );
    query.bindValue(":id", id);
    bool bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to delete icon id:"
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
