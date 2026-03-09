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
#include "ParameterDatabase.h"
static Q_LOGGING_CATEGORY(log, "DB")
CDatabase::CDatabase(QObject *parent)
    : QObject{parent}
    , m_MinVersion("0.1.0")
    , m_pPara(nullptr)
    , m_bOwner(false)
{
    qDebug(log) << Q_FUNC_INFO;
}

CDatabase::~CDatabase()
{
    qDebug(log) << Q_FUNC_INFO;
    if (m_bOwner)
        CloseDatabase();
}

bool CDatabase::SetDatabase(const CDatabase *db)
{
    return SetDatabase(db->GetDatabase(), db->GetParameter());
}

bool CDatabase::SetDatabase(const QSqlDatabase db, const CParameterDatabase *pPara)
{
    QString szErr = "Only one of OpenDatabase or SetDatabase can be called, and it can only be called once";
    Q_ASSERT_X(!IsOpen(), "Database", szErr.toStdString().c_str());
    SetError();
    m_database = db;
    m_pPara = pPara;
    return OnInitializeDatabase();
}

QSqlDatabase CDatabase::GetDatabase() const
{
    return m_database;
}

const CParameterDatabase *CDatabase::GetParameter() const
{
    return m_pPara;
}

const QString CDatabase::GetError() const
{
    return m_szError;
}

void CDatabase::SetError(const QString &szErr)
{
    m_szError = szErr;
}

bool CDatabase::OpenDatabase(const CParameterDatabase *pPara,
                             const QString &szConnectName)
{
    SetError();
    bool bRet = false;
    if(!pPara) {
        bRet = OpenSQLiteDatabase(pPara, szConnectName);
        return bRet;
    }

    m_pPara = pPara;
    QString szErr = "Only one of OpenDatabase or SetDatabase can be called, and it can only be called once";
    Q_ASSERT_X(!IsOpen(), "Database", szErr.toStdString().c_str());

    if(pPara->GetType() == "QSQLITE")
        bRet = OpenSQLiteDatabase(pPara, szConnectName);
    else if(pPara->GetType() == "QMYSQL")
        bRet = OpenMySqlDatabase(pPara, szConnectName);
    else if(pPara->GetType() == "QODBC")
        bRet = OpenODBCDatabase(pPara, szConnectName);
    else
        SetError(tr("Database type not supported:") + " " + pPara->GetType());

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

bool CDatabase::OpenSQLiteDatabase(const CParameterDatabase *pPara,
                                   const QString &szConnectionName)
{
    QString szFile;
    if(pPara) {
        szFile = pPara->GetDatabaseName();
    }
    return OpenSQLiteDatabase(szFile, szConnectionName);
}

bool CDatabase::OpenSQLiteDatabase(
    const QString& szFile, const QString& szConnectionName)
{
    QString databasePath = szFile;
    if (databasePath.isEmpty()) {
        // 使用默认路径
        QString dataDir = RabbitCommon::CDir::Instance()->GetDirUserDatabase();
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(dataDir);
        }
#if DEBUG
        databasePath = dir.filePath("remote_control_dev.db");
#else
        databasePath = dir.filePath("remote_control.db");
#endif
    }

    if(!szConnectionName.isEmpty())
        m_szConnectName = szConnectionName;

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QSQLITE", m_szConnectName);
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        SetError("Failed to open sqlite database: " + m_database.databaseName()
                 + "; Connect name: " + m_database.connectionName()
                 + "; Error: " + m_database.lastError().text());
        qCritical(log) << GetError();
        return false;
    }

    m_bOwner = true;
    qInfo(log) << "Open sqlite database:" << m_database.databaseName()
               << "Connect name:" << m_database.connectionName();

    return OnInitializeDatabase();
}

bool CDatabase::OpenMySqlDatabase(const CParameterDatabase *pPara,
                                  const QString &szConnectName)
{
    bool success = false;
    if(!pPara) return false;

    if(!szConnectName.isEmpty())
        m_szConnectName = szConnectName;

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QMYSQL", m_szConnectName);
    QString szDbName = pPara->GetDatabaseName();
    if(szDbName.isEmpty()) {
#if DEBUG
        szDbName = "remote_control_dev";
#else
        szDbName = "remote_control";
#endif
    }

    auto &net = pPara->m_Net;
    m_database.setHostName(net.GetHost());
    m_database.setPort(net.GetPort());
    auto &user = net.m_User;
    m_database.setUserName(user.GetName());
    m_database.setPassword(user.GetPassword());
    
    if (!m_database.open()) {
        SetError("Failed to open mysql database: " + m_database.databaseName()
                 + "; Connect name: " + m_database.connectionName()
                 + "; Host: " + net.GetHost()
                 + "; Port: " + QString::number(net.GetPort())
                 + "; User: " + user.GetUser()
                 + "; Error: " + m_database.lastError().text()
                 );
        qCritical(log) << GetError();
        return false;
    }

    m_bOwner = true;

    QSqlQuery query(GetDatabase());
    success = query.exec("CREATE DATABASE IF NOT EXISTS " + szDbName);
    if (!success) {
        SetError("Failed to create mysql database: " + szDbName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
        return false;
    }

    success = query.exec("use " + szDbName);
    if (!success) {
        SetError("Failed to use " + szDbName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
        return false;
    }

    m_database.setDatabaseName(szDbName);
    qInfo(log) << "Open mysql database:" << m_database.databaseName()
               << "Connect name:" << m_database.connectionName()
               << "Host:" << net.GetHost() << "Port:" << net.GetPort()
               << "User:" << user.GetUser();

    return OnInitializeDatabase();
}

bool CDatabase::OpenODBCDatabase(const CParameterDatabase *pPara,
                                 const QString &szConnectName)
{
    if(!pPara) return false;

    if(!szConnectName.isEmpty())
        m_szConnectName = szConnectName;

    // 打开或创建数据库
    m_database = QSqlDatabase::addDatabase("QODBC", m_szConnectName);
    QString szDbName = pPara->GetDatabaseName();
    if(szDbName.isEmpty()) {
#if DEBUG
        szDbName = "remote_control_dev";
#else
        szDbName = "remote_control";
#endif
    }
    m_database.setDatabaseName(szDbName);

    if (!m_database.open()) {
        SetError("Failed to open odbc database: " + m_database.databaseName()
                 + "; Connect name: " + m_database.connectionName()
                 + "; Error: " + m_database.lastError().text()
                 );
        qCritical(log) << GetError();
        return false;
    }

    m_bOwner = true;

    QSqlQuery query(GetDatabase());
    bool success = query.exec("CREATE DATABASE IF NOT EXISTS " + szDbName);
    if (!success) {
        SetError("Failed to create database: " + szDbName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
        return false;
    }

    success = query.exec("use " + szDbName);
    if (!success) {
        SetError("Failed to use " + szDbName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
        return false;
    }

    qInfo(log) << "Open odbc database:" << m_database.databaseName()
               << "Connect name:" << m_database.connectionName();

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
    } else {
        SetError("Don't support:" + m_pPara->GetType());
        qWarning(log) << GetError();
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
    if (!m_bOwner) {
        QString szErr = "This instance is not the owner of the database, but it is will close the database.";
        qWarning(log) << szErr;
        Q_ASSERT_X(m_bOwner, "CDatabase", szErr.toStdString().c_str());
    }
    if(m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase(m_szConnectName);
}

bool CDatabase::ExportToJsonFile(const QString &szFile)
{
    SetError();
    QFile file(szFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        SetError("Failed to open export JSON file: " + szFile + "; Error: " + file.errorString());
        qCritical(log) << GetError();
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
    root.insert("Author", "Kang Lin <kl222@126.com>");
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
    SetError();
    QFile file(szFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        SetError("Failed to open import JSON file: " + szFile + "; Error: " + file.errorString());
        qCritical(log) << GetError();
        return false;
    }

    do {
        QJsonDocument doc;
        doc = QJsonDocument::fromJson(file.readAll());
        if(!doc.isObject())
            break;
        auto root = doc.object();
        QString szTitle = root["Title"].toString();
        if(szTitle != "Rabbit Remote Control") {
            SetError("The file format is error. The title: " + szTitle + " != Rabbit Remote Control");
            qCritical(log) << GetError();
            break;
        }
        QString szVersion = root["Version"].toString();
        if(RabbitCommon::CTools::VersionCompare(szVersion, m_MinVersion) < 0) {
            SetError("The version is not support: "
                     + szVersion + " < " + m_MinVersion);
            qCritical(log) << GetError();
            break;
        }
        bRet = ImportFromJson(doc.object());
    } while(0);

    file.close();
    return bRet;
}

bool CDatabase::ImportFromJson(const QJsonObject &obj)
{
    return true;
}

bool CDatabase::ExportToJson(QJsonObject &obj)
{
    return true;
}

CDatabaseIcon::CDatabaseIcon(const QString &szSuffix, QObject *parent)
    : CDatabase(parent)
{
    m_szTableName = "icon";
    if(!szSuffix.isEmpty())
        m_szTableName = m_szTableName + "_" + szSuffix;
    m_szConnectName =  "connect_" + m_szTableName;
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
        SetError("Failed to create icon sqlite table: " + m_szTableName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + szSql);
        qCritical(log) << GetError();
        return false;
    }
    szSql = "CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_name ON " + m_szTableName + "(name)";
    success = query.exec(szSql);
    if (!success) {
        qWarning(log) << "Failed to create icon name index in" << m_szTableName
                      << "Error:" << query.lastError().text()
                      << "Sql:" << szSql;
    }
    szSql = "CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_hash ON " + m_szTableName + "(hash)";
    success = query.exec(szSql);
    if (!success) {
        qWarning(log) << "Failed to create icon hash index in" << m_szTableName
                      << "Error:" << query.lastError().text()
                      << "Sql:" << szSql;
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
        SetError("Failed to create icon mysql table: " + m_szTableName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + szSql);
        qCritical(log) << GetError();
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
            qDebug(log) << "Failed to select icon hash:" << szHash
                        << "Error:" << query.lastError().text()
                        << "Sql:" << query.executedQuery();
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
            SetError("Failed to insert icon hash: " + szHash
                     + "; Error: " + query.lastError().text()
                     + "; Sql: " + query.executedQuery());
            qCritical(log) << GetError();
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
        SetError("Failed to select icon name: " + szName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
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
        SetError("Failed to insert icon name: " + szName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
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
        SetError("Failed to get icon id: " + QString::number(id)
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
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
/*
bool CDatabaseIcon::ExportToJson(QJsonObject &obj)
{
    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, name, hash, data FROM " + m_szTableName
        );
    bool bRet = query.exec();
    if(!bRet) {
        SetError("Failed to export icon to json. Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
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
//*/
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
    QString szErr;
    QFileInfo fi(szFile);
    if(fi.isRelative()) {
        fi = QFileInfo(SetFile(szFile));
    }
    if(!fi.exists()) {
        szErr = "File is not exist: " + fi.filePath();
        qCritical(log) << szErr;
        return false;
    }
    QFile f(fi.absoluteFilePath());
    if(!f.open(QFile::ReadOnly | QFile::Text)) {
        szErr = "Failed to open file: " + f.fileName() + "; Error: " + f.errorString();
        qCritical(log) << szErr;
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
    QString szErr;
    QString szFileContent = obj["FileContent"].toString();
    if(szFileContent.isEmpty()) {
        qCritical(log) << "The file content is empty.";
        return false;
    }
    szFile = obj["FileName"].toString();
    if(szFile.isEmpty()) {
        qCritical(log) << "The file name is empty.";
        return false;
    }
    szFile = GetFile(szFile);
    QFileInfo fi(szFile);
    if(!fi.exists()) {
        QFile f(szFile);
        if(!f.open(QFile::WriteOnly | QFile::Text)) {
            szErr = "Failed to open file: " + szFile
                    + "; Error: " + f.errorString();
            qCritical(log) << szErr;
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

CDatabaseFile::CDatabaseFile(const QString &szSuffix, QObject *parent)
    : CDatabase(parent)
{
    m_szTableName = "file";
    if(!szSuffix.isEmpty())
        m_szTableName = m_szTableName + "_" + szSuffix;
    m_szConnectName = "connect_" + m_szTableName;
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
        SetError("Failed to Load file from: " + m_szTableName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
    }    
    return content;
}

bool CDatabaseFile::Save(const QString &szFile)
{
    bool bRet = true;
    if(szFile.isEmpty()) return false;
    QFile f(szFile);
    if(!f.open(QFile::ReadOnly | QFile::Text)) {
        SetError("Failed to open file: " + szFile
                 + "; Error: " + f.errorString());
        return false;
    }
    QByteArray content = f.readAll();
    f.close();
    QFileInfo fi(szFile);
    QSqlQuery query(GetDatabase());
    bRet = query.prepare(
        "SELECT content FROM " + m_szTableName + " "
        " WHERE file=:file"
        );
    if(!bRet) {
        SetError("Failed to prepare: " + query.executedQuery()
                 + "; Error: " + query.lastError().text());
        qCritical(log) << GetError();
        return false;
    }
    query.bindValue(":file", fi.fileName());
    bRet = query.exec();
    if(!bRet){
        SetError("Failed to exec: " + query.executedQuery()
                 + "; Error: " + query.lastError().text());
        qCritical(log) << GetError();
        return false;
    }
    if(query.next()) {
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
    bRet = query.exec();
    if (!bRet) {
        SetError("Failed to save file to:" + m_szTableName
                     + "; Error: " + query.lastError().text()
                     + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
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
        SetError("Failed to create file sqlite table: " + m_szTableName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
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
        SetError("Failed to create file mysql table: " + m_szTableName
                 + "; Error: " + query.lastError().text()
                 + "; Sql: " + query.executedQuery());
        qCritical(log) << GetError();
        return false;
    }

    return true;
}

bool CDatabaseFile::IsExist(const QString &szFile)
{
    QSqlQuery query(GetDatabase());
    bool ok = query.prepare("SELECT * from " + m_szTableName +
                  " WHERE `file`=:file");
    if(!ok) {
        SetError("Failed to prepare: " + query.executedQuery()
                 + "; Error: " + query.lastError().text());
        qCritical(log) << GetError();
        return false;
    }
    query.bindValue(":file", szFile);
    ok = query.exec();
    if(!ok) {
        SetError("Failed to exec: " + query.executedQuery()
                 + "; Error: " + query.lastError().text());
        qCritical(log) << GetError();
        return false;
    }
    return query.next();
}

QString CDatabaseFile::GetFile(const QString &szFile)
{
    QFileInfo fi(szFile);
    //qDebug(log) << szFile << fi.absolutePath();
    if(fi.isRelative()) {
        return RabbitCommon::CDir::Instance()->GetDirUserData()
            + QDir::separator() + szFile;
    }
    return szFile;
}

QString CDatabaseFile::SetFile(const QString &file)
{
    QString szFile;
    QFileInfo fi(file);
    QFileInfo d(RabbitCommon::CDir::Instance()->GetDirUserData() + QDir::separator());
    if(fi.absolutePath() == d.absolutePath())
        szFile = fi.fileName();
    else
        szFile = file;
    return szFile;
}
