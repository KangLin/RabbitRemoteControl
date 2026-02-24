// Author: Kang Lin <kl222@126.com>

#include <QSqlDriver>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QLoggingCategory>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "IconUtils.h"
#include "Database.h"

static Q_LOGGING_CATEGORY(log, "DB")
CDatabase::CDatabase(QObject *parent)
    : QObject{parent}
    , m_MinVersion("0.1.0")
    , m_pPara(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    m_szConnectName = "connect";
}

CDatabase::~CDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    CloseDatabase();
}

void CDatabase::SetDatabase(QSqlDatabase db, CParameterDatabase *pPara)
{
    QString szErr = "Only one of OpenDatabase and SetDatabase can be called, and it can only be called once";
    Q_ASSERT_X(!IsOpen(), "Database", szErr.toStdString().c_str());
    m_database = db;
    m_pPara = pPara;
}

QSqlDatabase CDatabase::GetDatabase() const
{
    return m_database;
}

bool CDatabase::OpenDatabase(CParameterDatabase *pPara)
{
    bool bRet = false;
    if(!pPara) {
        bRet = OpenSQLiteDatabase();
        return bRet;
    }

    m_pPara = pPara;
    QString szErr = "Only one of OpenDatabase or SetDatabase can be called, and it can only be called once";
    Q_ASSERT_X(!IsOpen(), "Database", szErr.toStdString().c_str());

    if(pPara->GetType() == "QSQLITE")
        bRet = OpenSQLiteDatabase(QString(), pPara->GetDatabaseName());
    else if(pPara->GetType() == "QMYSQL")
        bRet = OpenMySqlDatabase(pPara);
    else if(pPara->GetType() == "QODBC")
        bRet = OpenODBCDatabase(pPara);

    QSqlDriver *driver = GetDatabase().driver();
    if (driver) {
        qDebug(log) << "=== Features for" << pPara->GetType() << "===";
        qDebug(log) << "Transactions:" << driver->hasFeature(QSqlDriver::Transactions);
        qDebug(log) << "Query size:" << driver->hasFeature(QSqlDriver::QuerySize);
        qDebug(log) << "BLOB:" << driver->hasFeature(QSqlDriver::BLOB);
        qDebug(log) << "Unicode:" << driver->hasFeature(QSqlDriver::Unicode);
        qDebug(log) << "Prepared queries:" << driver->hasFeature(QSqlDriver::PreparedQueries);
        qDebug(log) << "Named placeholders:" << driver->hasFeature(QSqlDriver::NamedPlaceholders);
        qDebug(log) << "Positional placeholders:" << driver->hasFeature(QSqlDriver::PositionalPlaceholders);
        qDebug(log) << "Last insert ID:" << driver->hasFeature(QSqlDriver::LastInsertId);
        qDebug(log) << "Batch operations:" << driver->hasFeature(QSqlDriver::BatchOperations);
        qDebug(log) << "Event notifications:" << driver->hasFeature(QSqlDriver::EventNotifications);
        qDebug(log) << "Finish query:" << driver->hasFeature(QSqlDriver::FinishQuery);
        qDebug(log) << "Multiple result sets:" << driver->hasFeature(QSqlDriver::MultipleResultSets);
        qDebug(log) << "Cancel query:" << driver->hasFeature(QSqlDriver::CancelQuery);
    }
    
    return bRet;
}

bool CDatabase::OpenSQLiteDatabase(
    const QString &connectionName, const QString &dbPath)
{
    QString databasePath;
    if (dbPath.isEmpty()) {
        // 使用默认路径
        QString dataDir = RabbitCommon::CDir::Instance()->GetDirUserDatabase();
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(dataDir);
        }
        databasePath = dir.filePath("remote_control.db");
    } else {
        databasePath = dbPath;
    }

    if(!connectionName.isEmpty())
        m_szConnectName = connectionName;

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", m_szConnectName);
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        qCritical(log) << "Failed to open sqlite database:"
                       << m_database.lastError().text()
                       << "connect name:" << m_database.connectionName()
                       << "database name:" << m_database.databaseName();
        return false;
    }

    qInfo(log) << "Open sqlite database connect:"
               << m_database.connectionName()
               << "database name:" << m_database.databaseName();
    
    return OnInitializeDatabase();
}

bool CDatabase::OpenMySqlDatabase(CParameterDatabase *pPara)
{
    if(!pPara) return false;
    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QMYSQL", m_szConnectName);
    QString szDbName = pPara->GetDatabaseName();
    if(szDbName.isEmpty())
        szDbName = "remote_control";
    m_database.setDatabaseName(szDbName);
    auto &net = pPara->m_Net;
    m_database.setHostName(net.GetHost());
    m_database.setPort(net.GetPort());
    auto &user = net.m_User;
    m_database.setUserName(user.GetName());
    m_database.setPassword(user.GetPassword());

    if (!m_database.open()) {
        qCritical(log) << "Failed to open mysql database:"
                       << m_database.lastError().text()
                       << "connect name:" << m_database.connectionName()
                       << "database name:" << m_database.databaseName();
        return false;
    }

    QSqlQuery query(GetDatabase());
    bool success = query.exec("CREATE DATABASE IF NOT EXISTS " + szDbName);
    if (!success) {
        qCritical(log) << "Failed to create" << szDbName << "database:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

    success = query.exec("use remote_control");
    if (!success) {
        qCritical(log) << "Failed to use" << szDbName << "database:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

    qInfo(log) << "Open mysql database connect:"
               << m_database.connectionName()
               << "database name:" << m_database.databaseName();

    return OnInitializeDatabase();
}

bool CDatabase::OpenODBCDatabase(CParameterDatabase* pPara)
{
    if(!pPara) return false;
    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QODBC", m_szConnectName);
    QString szDbName = pPara->GetDatabaseName();
    if(szDbName.isEmpty())
        szDbName = "remote_control";
    m_database.setDatabaseName(szDbName);

    if (!m_database.open()) {
        qCritical(log) << "Failed to open odbc database:"
                       << m_database.lastError().text()
                       << "connect name:" << m_database.connectionName()
                       << "database name:" << m_database.databaseName();
        return false;
    }

    QSqlQuery query(GetDatabase());
    bool success = query.exec("CREATE DATABASE IF NOT EXISTS " + szDbName);
    if (!success) {
        qCritical(log) << "Failed to create" << szDbName << "database:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

    success = query.exec("use remote_control");
    if (!success) {
        qCritical(log) << "Failed to use" << szDbName << "database:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

    qInfo(log) << "Open odbc database connect:"
               << m_database.connectionName()
               << "database name:" << m_database.databaseName();

    return OnInitializeDatabase();
}

bool CDatabase::OnInitializeDatabase()
{
    bool bRet = false;
    if(!m_pPara) {
        bRet = OnInitializeSqliteDatabase();
        return bRet;
    }
    if(m_pPara->GetType() == "QSQLITE")
        bRet = OnInitializeSqliteDatabase();
    else if(m_pPara->GetType() == "QMYSQL" || m_pPara->GetType() == "QODBC") {
        bRet = OnInitializeMySqlDatabase();
    }
    return bRet;
}

bool CDatabase::OnInitializeSqliteDatabase()
{
    return true;
}

bool CDatabase::OnInitializeMySqlDatabase()
{
    return true;
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

bool CDatabase::ExportToJsonFile(const QString &szFile)
{
    QFile file(szFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical(log) << "Failed to open file:" << szFile << file.errorString();
        return false;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out.setGenerateByteOrderMark(true);  // 添加 UTF-8 BOM

    QJsonDocument doc;
    QJsonObject root;
    root.insert("Title", "Rabbit Remote Control");
    root.insert("Author", "Kang Lin");
    root.insert("Version", "0.1.0");

    bool bRet = true;
    bRet = ExportToJson(root);
    if(bRet) {
        doc.setObject(root);
        out << doc.toJson();
    }

    file.close();
    return true;
}

bool CDatabase::ImportFromJsonFile(const QString &szFile)
{
    bool bRet = false;
    QFile file(szFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical(log) << "Failed to open file:" << szFile << file.errorString();
        return false;
    }

    do {
        QJsonDocument doc;
        doc = QJsonDocument::fromJson(file.readAll());
        if(!doc.isObject())
            break;
        auto root = doc.object();
        if(root["Title"] != "Rabbit Remote Control") {
            qCritical(log) << "The file format is error";
            break;
        }
        QString szVersion = root["Version"].toString();
        if(RabbitCommon::CTools::VersionCompare(szVersion, m_MinVersion) < 0) {
            qCritical(log) << "The version is not support:"
                           << szVersion << "<" << m_MinVersion;
            break;
        }
        bRet = ImportFromJson(doc.object());
    } while(0);

    file.close();
    return bRet;
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
    m_szConnectName = "icon_connect";
    if(!szPrefix.isEmpty())
        m_szTableName = szPrefix + "_" + m_szTableName;
}

bool CDatabaseIcon::OnInitializeSqliteDatabase()
{
    QSqlQuery query(GetDatabase());

    // Create icon table
    QString szSql =
        "CREATE TABLE IF NOT EXISTS " + m_szTableName + " ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT UNIQUE,"       // Icon name. see QIcon::name()
        "    hash TEXT,"              // Icon hash value
        "    data BLOB"              // Icon binary data
        ")"
        ;
    bool success = query.exec(szSql);
    if (!success) {
        qCritical(log) << "Failed to create icon sqlite table:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }
    success = query.exec("CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_name ON " + m_szTableName + "(name)");
    if (!success) {
        qWarning(log) << "Failed to create icon name index:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
    }
    success = query.exec("CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_hash ON " + m_szTableName + "(hash)");
    if (!success) {
        qWarning(log) << "Failed to create icon hash index:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
    }
    return true;
}

bool CDatabaseIcon::OnInitializeMySqlDatabase()
{
    QSqlQuery query(GetDatabase());
    
    // Create icon table
    QString szSql =
        "CREATE TABLE IF NOT EXISTS " + m_szTableName + " ("
        "    id INTEGER PRIMARY KEY AUTO_INCREMENT,"
        "    name TEXT,"              // Icon name. see QIcon::name()
        "    hash TEXT,"              // Icon hash value
        "    data LONGBLOB,"          // Icon binary data
        "    UNIQUE KEY idx_icon_name (name(255)),"
        "    INDEX idx_icon_hash (hash(255))"
        ")"
        ;
    bool success = query.exec(szSql);
    if (!success) {
        qCritical(log) << "Failed to create icon mysql table:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

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
        qCritical(log) << "Failed to get icon id:"
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

bool CDatabaseIcon::ExportToJson(QJsonObject &obj)
{
    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, name, hash, data FROM " + m_szTableName
        );
    //qDebug(log) << "Sql:" << query.executedQuery();
    bool bRet = query.exec();
    if(!bRet) {
        qCritical(log) << "Failed to export icon to json:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

    QJsonArray icon;
    while (query.next()) {
        QJsonObject i;
        i.insert("id", query.value(0).toInt());
        i.insert("name", query.value(1).toString());
        i.insert("hash", query.value(2).toString());
        i.insert("data", query.value(3).toByteArray().toBase64().toStdString().c_str());
        icon.append(i);
    }
    if(!icon.isEmpty())
        obj.insert("icon", icon);
    return true;
}

bool CDatabaseIcon::ImportFromJson(const QJsonObject &obj)
{
    return true;
}

bool CDatabaseIcon::ExportIconToJson(const QIcon &icon, QJsonObject &obj)
{
    QString szIconName = icon.name();
    if(szIconName.isEmpty()) {
        QByteArray baIcon = RabbitCommon::CIconUtils::iconToByteArray(icon);
        obj.insert("IconData", baIcon.toBase64().toStdString().c_str());
    } else {
        obj.insert("IconName", szIconName);
    }
    return true;
}

bool CDatabaseIcon::ImportIconFromJson(const QJsonObject &itemObj, QIcon &icon)
{
    QString szIconName = itemObj["IconName"].toString();
    if(szIconName.isEmpty()) {
        QByteArray baIcon(itemObj["IconData"].toString().toStdString().c_str());
        if(!baIcon.isEmpty()) {
            baIcon = QByteArray::fromBase64(baIcon);
            icon = RabbitCommon::CIconUtils::byteArrayToIcon(baIcon);
        }
    } else {
        icon = QIcon::fromTheme(szIconName);
    }
    return true;
}

bool CDatabaseFile::ExportFileToJson(const QString &szFile, QJsonObject &obj)
{
    QFileInfo fi(szFile);
    if(!fi.exists()) {
        qCritical(log) << "File is not exist:" << szFile;
        return false;
    }
    QFile f(szFile);
    if(!f.open(QFile::ReadOnly | QFile::Text)) {
        qCritical(log) << "Failed to open file:" << szFile << f.errorString();
        return false;
    }
    QString szFileContent = f.readAll();
    f.close();
    if(szFileContent.isEmpty()) {
        qCritical(log) << "The file is empty:" << szFile;
        return false;
    };
    obj.insert("FileName", fi.fileName());
    obj.insert("FileContent", szFileContent);
    return true;
}

bool CDatabaseFile::ImportFileFromJson(const QJsonObject &obj, QString &szFile)
{
    QString szFileContent = obj["FileContent"].toString();
    if(szFileContent.isEmpty()) {
        qCritical(log) << "The file is empty";
        return false;
    }
    szFile = obj["FileName"].toString();
    if(szFile.isEmpty()) {
        qCritical(log) << "The file name is empty";
        return false;
    }
    szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
             + QDir::separator() + szFile;
    QFileInfo fi(szFile);
    if(!fi.exists()) {
        QFile f(szFile);
        if(!f.open(QFile::WriteOnly | QFile::Text)) {
            qCritical(log) << "Failed to open file:" << szFile
                           << f.errorString();
            return false;
        }
        f.write(szFileContent.toStdString().c_str(), szFileContent.size());
        f.close();
    }
    return true;
}

bool CDatabaseFile::ImportFileToDatabaseFromJson(const QJsonObject &obj, QString &szFile)
{
    bool bRet = ImportFileFromJson(obj, szFile);
    if(!bRet) return bRet;
    bRet = Save(szFile);
    return bRet;
}

CDatabaseFile::CDatabaseFile(QObject* parent) : CDatabase(parent)
{
    m_szConnectName = "file_connect";
    m_szTableName = "file";
}

CDatabaseFile::CDatabaseFile(const QString &szPrefix, QObject *parent)
    : CDatabase(parent)
{
    m_szConnectName = "file_connect";
    if(!szPrefix.isEmpty())
        m_szTableName = szPrefix + "_" + m_szTableName;
}

bool CDatabaseFile::ExportToJson(QJsonObject &obj)
{
    return true;
}

bool CDatabaseFile::ImportFromJson(const QJsonObject &obj)
{
    return true;
}

QByteArray CDatabaseFile::Load(const QString &szFile)
{
    QByteArray content;
    if(szFile.isEmpty()) return content;
    QFileInfo fi(szFile);
    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT content FROM " + m_szTableName + " "
        " WHERE file=:file");
    query.bindValue(":file", fi.fileName());
    bool ok = query.exec();
    if(ok) {
        if(query.next()) {
            content = query.value(0).toByteArray();
        }
    } else {
        qCritical(log) << "Failed to Load:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
    }    
    return content;
}

bool CDatabaseFile::Save(const QString &szFile)
{
    bool bRet = true;
    if(szFile.isEmpty()) return false;
    QFile f(szFile);
    if(!f.open(QFile::ReadOnly | QFile::Text)) {
        qCritical(log) << "Failed to open file:"
                       << szFile << f.errorString() << ":" << f.error();
        return false;
    }
    QByteArray content = f.readAll();
    f.close();
    QFileInfo fi(szFile);
    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT content FROM " + m_szTableName + " "
        " WHERE file=:file");
    query.bindValue(":file", fi.fileName());
    bool success = query.exec();
    if(success && query.next()) {
        query.prepare(
            "UPDATE " + m_szTableName + " "
            "SET content = :content "
            "WHERE file = :file"
            );
        query.bindValue(":file", fi.fileName());
        query.bindValue(":content", content);
    } else {
        query.prepare(
            "INSERT INTO " + m_szTableName + " "
            "(file, content) "
            "VALUES (:file, :content)"
            );
        query.bindValue(":file", fi.fileName());
        query.bindValue(":content", content);
    }
    success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to save file:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }
    
    return bRet;
}

bool CDatabaseFile::OnInitializeSqliteDatabase()
{
    QSqlQuery query(GetDatabase());

    // Create file table
    QString szSql =
        "CREATE TABLE IF NOT EXISTS " + m_szTableName + " ("
        "    file TEXT KEY NOT NULL UNIQUE,"
        "    content LONGBLOB"
        ")"
        ;
    bool success = query.exec(szSql);
    if (!success) {
        qCritical(log) << "Failed to create file table:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

    return true;
}

bool CDatabaseFile::OnInitializeMySqlDatabase()
{
    QSqlQuery query(GetDatabase());
    QString szSql = "CREATE TABLE IF NOT EXISTS `" + m_szTableName + "` ( "
        "`file` TEXT NOT NULL , "
        "`content` LONGBLOB, "
        "UNIQUE KEY `uk_file` (`file`(255))"
        ")";
    bool success = query.exec(szSql);
    if (!success) {
        qCritical(log) << "Failed to create file table:"
                       << m_szTableName << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

    return true;
}
