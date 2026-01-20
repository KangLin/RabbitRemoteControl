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

    void setDatabase(QSqlDatabase db);
    QSqlDatabase getDatabase() const;

    virtual bool openDatabase(const QString &connectionName = QString(),
        const QString &dbPath = QString());
    virtual bool isOpen() const;
    virtual void closeDatabase();

    virtual bool onInitializeDatabase() = 0;

Q_SIGNALS:
    void sigChanged();

protected:
    QSqlDatabase m_database;
    QString m_szConnectName;
};

class PLUGIN_EXPORT CDatabaseIcon : public CDatabase
{
    Q_OBJECT

public:
    explicit CDatabaseIcon(QObject *parent = nullptr);

    int getIcon(const QIcon& icon);
    QIcon getIcon(int id);

    virtual bool onInitializeDatabase() override;
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
    bool addFolder(const QString &name, int parentId = 0);
    bool renameFolder(int folderId, const QString &newName);
    bool deleteFolder(int folderId);
    bool moveFolder(int folderId, int newParentId);
    // 文件夹查询
    QList<FolderItem> getAllFolders();
    QList<FolderItem> getSubFolders(int parentId);

    virtual bool onInitializeDatabase() override;

protected:
    virtual bool onDeleteItems() = 0;
};
