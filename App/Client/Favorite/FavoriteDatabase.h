// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QIcon>

class CFavoriteDatabase : public QObject
{
    Q_OBJECT

public:
    explicit CFavoriteDatabase(QObject *parent = nullptr);
    ~CFavoriteDatabase();
    bool openDatabase(const QString &dbPath = QString());
    bool isOpen();
    void closeDatabase();

    enum Type {
        Type_Favorite,
        Type_Folder,
        Type_Separator
    };

    struct Item {
        int id;
        QString szName;
        QIcon icon;
        QString szFile;
        QDateTime time;
        QString szDescription;
        Type type;
        int parentId;

        // 运行时属性
        mutable QIcon cachedIcon;
        QList<Item> children;

        Item()
            : id(0)
            , parentId(0) // 指向默认收藏夹
            , type(Type_Favorite)
        {}

        bool isFolder() const { return type == Type_Folder; }
        bool isSeparator() const { return type == Type_Separator; }
        bool isFavorite() const { return type == Type_Favorite; }
    };

    bool AddFavorite(const Item &item);
    bool deleteFavorite(int id);
    bool moveFavorite(int id, int newParentId);

    // 文件夹操作
    bool addFolder(const QString &name, int parentId = 0);
    bool renameFolder(int folderId, const QString &newName);
    bool deleteFolder(int folderId);
    bool moveFolder(int folderId, int newParentId);

    // 文件夹查询
    QList<Item> getAllFolders();
    QList<Item> getSubFolders(int parentId);

Q_SIGNALS:
    void sigChanged();

private:
    QSqlDatabase m_database;
    bool initializeDatabase();
};
