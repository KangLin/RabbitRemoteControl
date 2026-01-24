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
        QString szName;
        QIcon icon;
        QString szFile;
        QString szDescription;
        TreeItem::TYPE type;

        Item()
            : id(0)
            , type(TreeItem::Leaf)
        {}
        QIcon GetIcon() {
            if(!icon.isNull())
                return icon;
            if(isGroup())
                return QIcon::fromTheme("folder");
            return QIcon::fromTheme("file");
        }
        bool isGroup() const { return type == TreeItem::Node; }
        bool isFavorite() const { return type == TreeItem::Leaf; }
    };

    int AddFavorite(const QIcon &icon, const QString& szName,
                     const QString &szFile, const QString szDescription,
                     int parentId = 0);
    bool UpdateFavorite(const QString& szName, int id);
    Item GetFavorite(int id);
    Item GetGroup(int id);
    QList<Item> GetChildren(int parentId);

private:
    bool OnInitializeDatabase() override;
    CDatabaseIcon m_IconDB;

    // CDatabaseTree interface
protected:
    virtual bool OnDeleteKey(int key) override;
};
