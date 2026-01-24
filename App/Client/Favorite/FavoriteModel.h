// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QVector>
#include <QAbstractItemModel>
#include "FavoriteDatabase.h"

class CFavoriteModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit CFavoriteModel(CFavoriteDatabase* pDatabase, QObject *parent = nullptr);
    ~CFavoriteModel();

    bool AddNode(const QString& szName, int parentId);
    bool AddFavorite(const QIcon &icon, const QString& szName,
                     const QString &szFile, const QString szDescription,
                     int parentId = 0);

    enum RoleType {
        RoleFile = Qt::UserRole,
        RoleNodeType,
        RoleItem
    };

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    CFavoriteDatabase* m_pDatabase;

    struct tree {
        tree* parent = nullptr;
        CFavoriteDatabase::Item item;
        int row = 0; // the row of item in the it's parent
        QVector<tree*> children;
    };
    tree* m_pRoot;
    QMap<int, tree*> m_Folders;
    void clearTree(tree* node);
};
