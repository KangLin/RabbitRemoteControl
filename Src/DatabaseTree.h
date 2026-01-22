// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QIcon>
#include "Database.h"

class PLUGIN_EXPORT TreeItem {
public:
    TreeItem();
    TreeItem(const TreeItem& item);

    bool IsNode() const;
    bool IsLeaf() const;

    enum TYPE {
        Node,
        Leaf
    };
    TYPE GetType() const;
    void SetType(TYPE type);
    int GetId() const;
    void SetId(int newId);
    QString GetName() const;
    void SetName(const QString &newName);
    QDateTime GetCreateTime() const;
    void SetCreateTime(const QDateTime &newCreateTime);
    QDateTime GetModifyTime() const;
    void SetModifyTime(const QDateTime &newModifyTime);
    QDateTime GetLastVisitTime() const;
    void SetLastVisitTime(const QDateTime &newLastVisitTime);
    int GetParentId() const;
    void SetParentId(int newParentId);
    int GetSortOrder() const;
    void SetSortOrder(int newSortOrder);

    int GetKey() const;
    void SetKey(int newNKey);

private:
    TYPE m_Type;
    int m_id;
    QString m_szName;
    int m_nKey;
    QDateTime m_CreateTime;
    QDateTime m_ModifyTime;
    QDateTime m_LastVisitTime;
    int m_ParentId;
    int m_SortOrder;
};

class PLUGIN_EXPORT CDatabaseFolder : public CDatabase
{
    Q_OBJECT

public:
    explicit CDatabaseFolder(QObject *parent = nullptr);
    explicit CDatabaseFolder(const QString& szPrefix, QObject *parent = nullptr);

    // 文件夹操作
    bool AddFolder(const QString &name, int parentId = 0);
    bool RenameFolder(int id, const QString &newName);
    bool DeleteFolder(int id, std::function<int(int parentId)> cbDeleteLeaf = nullptr);
    bool MoveFolder(int id, int newParentId);
    // 文件夹查询
    QList<TreeItem> GetAllFolders();
    QList<TreeItem> GetSubFolders(int parentId);
    int GetCount();

    virtual bool OnInitializeDatabase() override;

    QString GetTableName() const;
    void SetTableName(const QString &newSzTableName);

protected:
    virtual bool OnDeleteLeafs(int id);

private:
    QString m_szTableName;
};

class PLUGIN_EXPORT CDatabaseTree : public CDatabase
{
    Q_OBJECT

public:
    explicit CDatabaseTree(QObject* parent = nullptr);
    explicit CDatabaseTree(const QString& szPrefix, QObject* parent = nullptr);

    // Leaf operate

    /*!
     * \brief Add item
     * \param item
     * \return > 0 , id of added item.
     *         = 0 , failed
     */
    int Add(const TreeItem& item);
    bool Update(const TreeItem& item);
    bool Delete(int id);
    bool Delete(QList<int> items);
    bool DeleteChild(int parentId);
    bool Move(int id, int newParent);

    TreeItem GetLeaf(int id);
    /*!
     * \brief Get the leaves under nodeId
     * \param nodeId:
     *          - = 0: Get all leaves
     *          - > 0: Get the leaves under the node
     */
    QList<TreeItem> GetLeaves(int nodeId);
    /*!
     * \brief Get leaves
     * \param value: Get the leaves of value
     */
    QList<TreeItem> GetLeavesByKey(int key);
    QList<TreeItem> GetLeavesByKey(QList<int> key);

    // Node operate
    bool AddNode(const QString &name, int parentId = 0);
    bool RenameNode(int id, const QString &newName);
    bool DeleteNode(int id);
    bool MoveNode(int id, int newParentId);

    QList<TreeItem> GetAllNodes();
    QList<TreeItem> GetSubNodes(int parentId);
    int GetNodeCount();

    virtual bool OnInitializeDatabase() override;

private:
    QString m_szTableName;
    CDatabaseFolder m_FolderDB;
};
