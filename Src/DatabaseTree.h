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
    int AddFolder(const QString &name, int parentId = 0);
    bool RenameFolder(int id, const QString &newName);
    bool DeleteFolder(int id, std::function<int(int parentId)> cbDeleteLeaf = nullptr);
    bool MoveFolder(int id, int newParentId);
    // 文件夹查询
    TreeItem GetFolder(int id);
    QList<TreeItem> GetAllFolders();
    QList<TreeItem> GetSubFolders(int parentId);
    /*!
     * \brief Get count
     * \param parentId: 0 : Get all count
     * \return
     */
    int GetCount(int parentId = 0);

    virtual bool OnInitializeDatabase() override;
    virtual bool ExportToJson(QJsonObject& obj) override;
    virtual bool ImportFromJson(const QJsonObject& obj) override;

    QString GetTableName() const;
    void SetTableName(const QString &newSzTableName);

Q_SIGNALS:
    void sigAddFolder(int id, int parentId);

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
    virtual int Add(const TreeItem& item);
    virtual bool Update(const TreeItem& item);
    virtual bool Delete(int id, bool delKey = false);
    virtual bool Delete(QList<int> items, bool delKey = false);
    virtual bool DeleteChild(int parentId, bool delKey = false);
    virtual bool Move(int id, int newParent);

    TreeItem GetLeaf(int id);
    /*!
     * \brief Get the leaves under nodeId
     * \param nodeId:
     *          - < 0: Get all leaves
     *          - >= 0: Get the leaves under the node
     */
    QList<TreeItem> GetLeaves(int nodeId);
    /*!
     * \brief Get leaves
     * \param value: Get the leaves of value
     */
    QList<TreeItem> GetLeavesByKey(int key);
    QList<TreeItem> GetLeavesByKey(QList<int> key);
    int GetLeafCount(int parentId = 0);

    // Node operate
    virtual int AddNode(const QString &name, int parentId = 0);
    virtual bool RenameNode(int id, const QString &newName);
    virtual bool DeleteNode(int id, bool delKey = false);
    virtual bool MoveNode(int id, int newParentId);
    TreeItem GetNode(int id);
    QList<TreeItem> GetAllNodes();
    QList<TreeItem> GetSubNodes(int parentId);
    int GetNodeCount(int nParentId = 0);

    /*!
     * \~chinese
     * \brief 得到指定id节点下的所有节点和叶子数。不递归。
     * \param parentId: 0, 得到树中所有节点和叶子。
     * \~english
     * \brief Get the count of parentId. include nodes and leaves
     * \param parentId: 0, Get all count
     * \return
     */
    int GetCount(int parentId = 0);

    virtual bool OnInitializeDatabase() override;
    virtual bool ExportToJson(QJsonObject& obj) override;
    virtual bool ImportFromJson(const QJsonObject& obj) override;

Q_SIGNALS:
    void sigAddFolder(int id, int parentId);
    void sigAdd(int id, int parentId);

protected:
    /*!
     * \~chinese
     * \brief 从 key 相关的表中删除 key
     * \param key
     * \return treu: 成功
     *         false: 失败。删除停止。
     */
    virtual bool OnDeleteKey(int key);

private:
    QString m_szTableName;
    CDatabaseFolder m_FolderDB;
};
