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
    bool AddFavorite(const QString &szFile,
                     const QString& szName,
                     const QIcon &icon,
                     const QString szDescription,
                     int parentId = 0);
    bool UpdateFavorite(
        const QString &szFile, const QString &szName = QString(),
        const QString &szDescription = QString(), const QIcon &icon = QIcon());
    CFavoriteDatabase::Item GetFavorite(const QString& szFile);
    void Refresh();

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
        CFavoriteDatabase::Item item;
        tree* parent = nullptr;
        QVector<tree*> children;

        tree();
        tree(CFavoriteDatabase::Item i);
        ~tree();

        int GetRow() const;
        void SetRow(int r);
        bool IsFolder() const;
        bool IsFavorite() const;
        int ChildCount() const;
        tree* ChildAt(int index) const;
        int GetInserIndex(tree* child);
        bool AddChild(tree* child);
        bool InsertChild(int index, tree* child);
        bool RemoveChild(tree* child);
        tree* FindChild(int id) const;
        tree* FindRecursive(int id) const;

    private:
        int m_row = 0; // the row of item in the it's parent
    };

    tree* m_pRoot;

    QMap<int, tree*> m_Folders;
    tree* GetTree(int id) const;

    tree* GetTree(QModelIndex index) const;
    QModelIndex CreateIndex(tree* t) const;

    void ClearTree(tree* node);
    bool AddTree(const CFavoriteDatabase::Item& item, int parentId);
    bool UpdateTree(const QString &szFile);
    bool MoveTree(int id, int newParentId);
};
