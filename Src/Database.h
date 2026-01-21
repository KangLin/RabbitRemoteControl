// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include "plugin_export.h"

class PLUGIN_EXPORT CDatabase : public QObject
{
    Q_OBJECT
public:
    explicit CDatabase(QObject *parent = nullptr);
    virtual ~CDatabase();

    void SetDatabase(QSqlDatabase db);
    QSqlDatabase GetDatabase() const;

    virtual bool OpenDatabase(const QString &connectionName = QString(),
        const QString &dbPath = QString());
    virtual bool IsOpen() const;
    virtual void CloseDatabase();

    virtual bool OnInitializeDatabase() = 0;

Q_SIGNALS:
    void sigChanged();

protected:
    QString m_szConnectName;
private:
    QSqlDatabase m_database;
};

class PLUGIN_EXPORT CDatabaseIcon : public CDatabase
{
    Q_OBJECT

public:
    explicit CDatabaseIcon(QObject *parent = nullptr);

    /*!
     * \brief Get icon id
     * \param icon
     * \return icon's id
     * \note If the icon is not exist. then insert the icon to table
     */
    int GetIcon(const QIcon& icon);
    QIcon GetIcon(int id);

    virtual bool OnInitializeDatabase() override;
};

class PLUGIN_EXPORT CDatabaseFolder : public CDatabase
{
    Q_OBJECT
public:
    explicit CDatabaseFolder(QObject *parent = nullptr);

    struct FolderItem {
        int id;
        QString szName;
        int parentId;
        int sortOrder;
        QDateTime createTime;
    };

    // 文件夹操作
    bool AddFolder(const QString &name, int parentId = 0);
    bool RenameFolder(int folderId, const QString &newName);
    bool DeleteFolder(int folderId);
    bool MoveFolder(int folderId, int newParentId);
    // 文件夹查询
    QList<FolderItem> GetAllFolders();
    QList<FolderItem> GetSubFolders(int parentId);

    virtual bool OnInitializeDatabase() override;

protected:
    virtual bool OnDeleteItems() = 0;
};
