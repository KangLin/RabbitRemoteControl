// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QDateTime>
#include <QIcon>
#include "DatabaseTree.h"

class CFavoriteDatabase : public CDatabaseTree
{
    Q_OBJECT

public:
    explicit CFavoriteDatabase(QObject *parent = nullptr);

    struct Item {
        int id;          // is the id of tree table
        int parentId;
        QString szName;
        QIcon icon;
        QString szFile;
        QString szDescription;
        TreeItem::TYPE type;
        Item()
            : id(0)
            , parentId(0)
            , type(TreeItem::Leaf)
        {}
        QIcon GetIcon() {
            if(!icon.isNull())
                return icon;
            if(isFolder())
                return QIcon::fromTheme("folder");
            return QIcon::fromTheme("file");
        }
        bool isFolder() const { return type == TreeItem::Node; }
        bool isFavorite() const { return type == TreeItem::Leaf; }
    };

    int AddFavorite(const QString &szFile, const QString& szName,
                     const QIcon &icon, const QString szDescription,
                     int parentId = 0);
    bool UpdateFavorite(int id, const QString& szName = QString(),
                        const QIcon &icon = QIcon(),
                        const QString szDescription = QString());
    bool UpdateFavorite(const QString& szFile, const QString& szName = QString(),
                        const QIcon &icon = QIcon(),
                        const QString szDescription = QString());
    Item GetFavorite(int id);
    QList<Item> GetFavorite(const QString &szFile);
    Item GetGroup(int id);
    QList<Item> GetChildren(int parentId);

private:
    bool OnInitializeDatabase() override;
    CDatabaseIcon m_IconDB;

    // CDatabaseTree interface
protected:
    virtual bool OnDeleteKey(int key) override;
    virtual bool ExportToJson(QJsonObject &obj) override;
    virtual bool ImportFromJson(const QJsonObject &obj) override;
private:
    bool ExportToJson(int parentId, QJsonArray& obj);
    bool ImportFromJson(int parentId, const QJsonArray& obj);
};

Q_DECLARE_METATYPE(CFavoriteDatabase::Item)
