// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QJsonObject>
#include "plugin_export.h"

class CParameterDatabase;

/*!
 * \chinese 提供打开数据库和初始化数据库等接口
 * \english Provide interfaces such as opening the database and initializing the database
 *
 * \~
 * \ingroup APP_API PLUGIN_API
 * \defgroup DATABASE_API
 */
class PLUGIN_EXPORT CDatabase : public QObject
{
    Q_OBJECT

public:
    explicit CDatabase(QObject *parent = nullptr);
    virtual ~CDatabase();

    /*!
     * \brief Share an existing database
     * \param db
     */
    [[nodiscard]] bool SetDatabase(const CDatabase* db);
    /*!
     * \brief Share an existing database
     * \param db
     * \param pPara
     */
    [[nodiscard]] bool SetDatabase(const QSqlDatabase db, const CParameterDatabase* pPara);
    [[nodiscard]] QSqlDatabase GetDatabase() const;

    /*!
     * \brief Open a new database
     * \param pPara: nullptr, use sqlite database
     * \param szConnectName: connect name
     * \return
     *        - true: successfully
     *        - false: failed
     */
    [[nodiscard]] virtual bool OpenDatabase(
        const CParameterDatabase* pPara = nullptr,
        const QString& szConnectName = QString());
    [[nodiscard]] bool OpenMySqlDatabase(
        const CParameterDatabase* pPara,
        const QString& szConnectName = QString());
    [[nodiscard]] bool OpenODBCDatabase(
        const CParameterDatabase* pPara,
        const QString& szConnectName = QString());
    [[nodiscard]] bool OpenSQLiteDatabase(
        const CParameterDatabase* pPara,
        const QString &szConnectionName = QString());
    [[nodiscard]] bool OpenSQLiteDatabase(
        const QString& szFile,
        const QString& szConnectionName = QString());

    [[nodiscard]] virtual bool IsOpen() const;
    /*!
     * \brief Close database
     * \note It is only necessary to use it to close the database when using OpenDatabase.
     */
    void CloseDatabase();

    [[nodiscard]] const CParameterDatabase* GetParameter() const;
    [[nodiscard]] const QString GetError() const;
    
    [[nodiscard]] virtual bool ExportToJsonFile(const QString& szFile);
    [[nodiscard]] virtual bool ImportFromJsonFile(const QString& szFile);

Q_SIGNALS:
    void sigChanged();

protected:
    /*!
     * \brief Initialize database
     * \return
     */
    [[nodiscard]] virtual bool OnInitializeDatabase();
    [[nodiscard]] virtual bool OnInitializeSqliteDatabase();
    [[nodiscard]] virtual bool OnInitializeMySqlDatabase();
    void SetError(const QString& szErr = QString());

    [[nodiscard]] virtual bool ExportToJson(QJsonObject& obj);
    [[nodiscard]] virtual bool ImportFromJson(const QJsonObject& obj);

protected:
    QString m_szConnectName;
    QString m_MinVersion;
    const CParameterDatabase* m_pPara;

private:
    QSqlDatabase m_database;
    bool m_bOwner;
    QString m_szError;
};

/*!
 * \brief Icon database
 * \ingroup DATABASE_API
 */
class PLUGIN_EXPORT CDatabaseIcon : public CDatabase
{
    Q_OBJECT

public:
    explicit CDatabaseIcon(const QString& szSuffix = QString(),
                           QObject *parent = nullptr);

    /*!
     * \brief Get icon id
     * \param icon
     * \return icon's id
     * \note If the icon is not exist. then insert the icon to table
     */
    [[nodiscard]] int GetIcon(const QIcon& icon);
    [[nodiscard]] QIcon GetIcon(int id);

    [[nodiscard]] static bool ExportIconToJson(/*in*/const QIcon& icon, /*out*/QJsonObject& obj);
    [[nodiscard]] static bool ImportIconFromJson(/*in*/const QJsonObject &obj, /*out*/QIcon& icon);

protected:
    bool OnInitializeSqliteDatabase() override;
    bool OnInitializeMySqlDatabase() override;

    //[[nodiscard]] virtual bool ExportToJson(QJsonObject& obj) override;

private:
    QString m_szTableName;
};

/*!
 * \brief File database
 * \note The file field is filename, don't include path.
 * \ingroup DATABASE_API
 */
class PLUGIN_EXPORT CDatabaseFile : public CDatabase
{
    Q_OBJECT
    
public:
    explicit CDatabaseFile(const QString& szSuffix = QString(),
                           QObject *parent = nullptr);

    bool IsExist(const QString& szFile);

    /*!
     * \brief Load
     * \param szFile: the file path(with file system)
     */
    [[nodiscard]] QByteArray Load(const QString &szFile);
    /*!
     * \brief Save
     * \param szFile: the file path(with file system)
     */
    bool Save(const QString& szFile);
    
    /*!
     * \brief ExportFileToJson
     * \param szFile: file path(with file system)
     * \param obj
     * \return 
     */
    [[nodiscard]] static bool ExportFileToJson(const QString &szFile, QJsonObject &obj);
    /*!
     * \brief Import file from JSON
     * \param obj
     * \param szFile: file path(with file system)
     * \return 
     */
    [[nodiscard]] static bool ImportFileFromJson(const QJsonObject &obj, QString &szFile);
    /*!
     * \brief Import file to database from JSON
     * \param obj
     * \param szFile: file path(with file system)
     * \return 
     */
    [[nodiscard]] bool ImportFileToDatabaseFromJson(const QJsonObject &obj, QString &szFile);

    /*!
     * \brief Get the file with file system from the file in database
     * \param szFile: the file in database
     * \return the file with file system
     */
    [[nodiscard]] static QString GetFile(const QString& szFile);
    /*!
     * \brief Set the file with file system to the file in database
     * \param szFile: the file with system system
     * \return the file in database
     */
    [[nodiscard]] static QString SetFile(const QString& szFile);

protected:
    [[nodiscard]] virtual bool OnInitializeSqliteDatabase() override;
    [[nodiscard]] virtual bool OnInitializeMySqlDatabase() override;

private:
    QString m_szTableName;
};
