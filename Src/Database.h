// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QJsonObject>
#include "plugin_export.h"
#include "ParameterDatabase.h"

class PLUGIN_EXPORT CDatabase : public QObject
{
    Q_OBJECT
public:
    explicit CDatabase(QObject *parent = nullptr);
    virtual ~CDatabase();

    void SetDatabase(QSqlDatabase db, CParameterDatabase* pPara = nullptr);
    QSqlDatabase GetDatabase() const;

    /*!
     * \brief OpenDatabase
     * \param pPara: nullptr, use sqlite database
     * \param szConnectName: connect name
     * \return 
     */
    virtual bool OpenDatabase(CParameterDatabase* pPara = nullptr,
                              const QString& szConnectName = QString());
    virtual bool OpenMySqlDatabase(CParameterDatabase* pPara,
                                   const QString& szConnectName = QString());
    virtual bool OpenODBCDatabase(CParameterDatabase* pPara,
                                  const QString& szConnectName = QString());
    virtual bool OpenSQLiteDatabase(CParameterDatabase* pPara = nullptr,
                                    const QString &szConnectionName = QString());
    virtual bool IsOpen() const;
    virtual void CloseDatabase();

    virtual bool OnInitializeDatabase();

    virtual bool ExportToJsonFile(const QString& szFile);
    virtual bool ImportFromJsonFile(const QString& szFile);
    virtual bool ExportToJson(QJsonObject& obj) = 0;
    virtual bool ImportFromJson(const QJsonObject& obj) = 0;

Q_SIGNALS:
    void sigChanged();

protected:
    virtual bool OnInitializeSqliteDatabase();
    virtual bool OnInitializeMySqlDatabase();

protected:
    QString m_szConnectName;
    QString m_MinVersion;
    CParameterDatabase* m_pPara;

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
    int GetIcon(const QIcon& icon);
    QIcon GetIcon(int id);

    virtual bool ExportToJson(QJsonObject& obj) override;
    virtual bool ImportFromJson(const QJsonObject& obj) override;

    static bool ExportIconToJson(/*in*/const QIcon& icon, /*out*/QJsonObject& obj);
    static bool ImportIconFromJson(/*in*/const QJsonObject &obj, /*out*/QIcon& icon);

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
    
    /*!
     * \brief Load
     * \param szFile: the file path
     * \note The file field in database is filename, don't include path.
     */
    QByteArray Load(const QString &szFile);
    /*!
     * \brief Save
     * \param szFile: the file path
     * \note The file field in database is filename, don't include path.
     */
    bool Save(const QString& szFile);

    virtual bool ExportToJson(QJsonObject &obj) override;
    virtual bool ImportFromJson(const QJsonObject &obj) override;
    
    static bool ExportFileToJson(const QString &szFile, QJsonObject &obj);
    static bool ImportFileFromJson(const QJsonObject &obj, QString &szFile);
    bool ImportFileToDatabaseFromJson(const QJsonObject &obj, QString &szFile);

protected:
    virtual bool OnInitializeSqliteDatabase() override;
    virtual bool OnInitializeMySqlDatabase() override;

private:
    QString m_szTableName;
};
