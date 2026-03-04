// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QJsonObject>
#include "plugin_export.h"

class CParameterDatabase;
class PLUGIN_EXPORT CDatabase : public QObject
{
    Q_OBJECT

public:
    explicit CDatabase(QObject *parent = nullptr);
    virtual ~CDatabase();

    /*!
     * \brief Share an existing database
     * \param db
     * \note After the call is completed, OnInitializeDatabase needs to be called
     */
    void SetDatabase(const CDatabase* db);
    /*!
     * \brief Share an existing database
     * \param db
     * \param pPara
     */
    void SetDatabase(const QSqlDatabase db, const CParameterDatabase* pPara);
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
    [[nodiscard]] virtual bool OpenMySqlDatabase(
        const CParameterDatabase* pPara,
        const QString& szConnectName = QString());
    [[nodiscard]] virtual bool OpenODBCDatabase(
        const CParameterDatabase* pPara,
        const QString& szConnectName = QString());
    [[nodiscard]] virtual bool OpenSQLiteDatabase(
        const CParameterDatabase* pPara,
        const QString &szConnectionName = QString());
    [[nodiscard]] virtual bool OpenSQLiteDatabase(
        const QString& szFile,
        const QString& szConnectionName = QString());
    [[nodiscard]] virtual bool IsOpen() const;
    virtual void CloseDatabase();

    /*!
     * \brief Initialize database
     * \return
     */
    [[nodiscard]] virtual bool OnInitializeDatabase();

    [[nodiscard]] const CParameterDatabase* GetParameter() const;

    [[nodiscard]] virtual bool ExportToJsonFile(const QString& szFile);
    [[nodiscard]] virtual bool ImportFromJsonFile(const QString& szFile);
    [[nodiscard]] virtual bool ExportToJson(QJsonObject& obj);
    [[nodiscard]] virtual bool ImportFromJson(const QJsonObject& obj);

Q_SIGNALS:
    void sigChanged();

protected:
    [[nodiscard]] virtual bool OnInitializeSqliteDatabase();
    [[nodiscard]] virtual bool OnInitializeMySqlDatabase();

protected:
    QString m_szConnectName;
    QString m_MinVersion;
    const CParameterDatabase* m_pPara;

private:
    QSqlDatabase m_database;
};

class PLUGIN_EXPORT CDatabaseIcon : public CDatabase
{
    Q_OBJECT

public:
    explicit CDatabaseIcon(QObject *parent = nullptr);
    explicit CDatabaseIcon(const QString& szPrefix, QObject *parent = nullptr);

    /*!
     * \brief Get icon id
     * \param icon
     * \return icon's id
     * \note If the icon is not exist. then insert the icon to table
     */
    [[nodiscard]] int GetIcon(const QIcon& icon);
    [[nodiscard]] QIcon GetIcon(int id);

    [[nodiscard]] virtual bool ExportToJson(QJsonObject& obj) override;
    [[nodiscard]] virtual bool ImportFromJson(const QJsonObject& obj) override;

    [[nodiscard]] static bool ExportIconToJson(/*in*/const QIcon& icon, /*out*/QJsonObject& obj);
    [[nodiscard]] static bool ImportIconFromJson(/*in*/const QJsonObject &obj, /*out*/QIcon& icon);

protected:
    bool OnInitializeSqliteDatabase() override;
    bool OnInitializeMySqlDatabase() override;

private:
    QString m_szTableName;
};

/*!
 * \brief The CDatabaseFile class
 * \note The file field is filename, don't include path.
 */
class PLUGIN_EXPORT CDatabaseFile : public CDatabase
{
    Q_OBJECT
    
public:
    explicit CDatabaseFile(QObject* parent = nullptr);
    explicit CDatabaseFile(const QString& szPrefix, QObject *parent = nullptr);

    bool IsExist(const QString& szFile);
    
    /*!
     * \brief Load
     * \param szFile: the file path
     */
    [[nodiscard]] QByteArray Load(const QString &szFile);
    /*!
     * \brief Save
     * \param szFile: the file path
     */
    bool Save(const QString& szFile);

    [[nodiscard]] virtual bool ExportToJson(QJsonObject &obj) override;
    [[nodiscard]] virtual bool ImportFromJson(const QJsonObject &obj) override;
    
    [[nodiscard]] static bool ExportFileToJson(const QString &szFile, QJsonObject &obj);
    [[nodiscard]] static bool ImportFileFromJson(const QJsonObject &obj, QString &szFile);
    [[nodiscard]] bool ImportFileToDatabaseFromJson(const QJsonObject &obj, QString &szFile);

protected:
    [[nodiscard]] virtual bool OnInitializeSqliteDatabase() override;
    [[nodiscard]] virtual bool OnInitializeMySqlDatabase() override;

private:
    QString m_szTableName;
};
