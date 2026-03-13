// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QIcon>
#include "Database.h"

class PLUGIN_EXPORT TreeItem {
public:
    enum TYPE {
        Node,
        Leaf
    };

    TreeItem(TYPE type);
    TreeItem(const TreeItem& item);
    TreeItem& operator = (const TreeItem& item);

    [[nodiscard]] bool IsNode() const;
    [[nodiscard]] bool IsLeaf() const;

    [[nodiscard]] TYPE GetType() const;
    void SetType(TYPE type);
    [[nodiscard]] int GetId() const;
    void SetId(int newId);
    [[nodiscard]] QString GetName() const;
    void SetName(const QString &newName);
    [[nodiscard]] QDateTime GetCreateTime() const;
    void SetCreateTime(const QDateTime &newCreateTime);
    [[nodiscard]] QDateTime GetModifyTime() const;
    void SetModifyTime(const QDateTime &newModifyTime);
    [[nodiscard]] QDateTime GetLastVisitTime() const;
    void SetLastVisitTime(const QDateTime &newLastVisitTime);
    [[nodiscard]] int GetParentId() const;
    void SetParentId(int newParentId);
    [[nodiscard]] int GetSortOrder() const;
    void SetSortOrder(int newSortOrder);

    [[nodiscard]] int GetKey() const;
    void SetKey(int newNKey);

private:
    struct _DATA{
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
    _DATA m_Data;
};

/*!
 * \brief The CDatabaseFolder class
 * \ingroup DATABASE_API
 */
class PLUGIN_EXPORT CDatabaseFolder : public CDatabase
{
    Q_OBJECT

public:
    explicit CDatabaseFolder(const QString& szPrefix = QString(),
                             QObject *parent = nullptr);

    // 文件夹操作
    int AddFolder(const QString &name, int parentId = 0);
    bool RenameFolder(int id, const QString &newName);
    bool DeleteFolder(
        int id,
        std::function<bool(int parentId)> cbDeleteLeaf = nullptr,
        bool checkReturn = true);
    bool MoveFolder(int id, int newParentId);
    // 文件夹查询
    [[nodiscard]] TreeItem GetFolder(int id);
    [[nodiscard]] QList<TreeItem> GetAllFolders();
    [[nodiscard]] QList<TreeItem> GetSubFolders(int parentId);
    [[nodiscard]] bool InSubFolder(int parentId, int id);

    /*!
     * \brief Get count
     * \param parentId: - 0 : Get all count
     *                  - other: get the count of children in parentId 
     * \return
     */
    [[nodiscard]] int GetCount(int parentId = 0);

    [[nodiscard]] virtual bool ExportToJson(QJsonObject& obj) override;
    [[nodiscard]] virtual bool ImportFromJson(const QJsonObject& obj) override;

Q_SIGNALS:
    void sigAddFolder(int id, int parentId);

protected:
    [[nodiscard]] virtual bool OnDeleteLeafs(int id);
    [[nodiscard]] virtual bool OnInitializeSqliteDatabase() override;
    [[nodiscard]] virtual bool OnInitializeMySqlDatabase() override;
    
private:
    QString m_szTableName;
};

/*!
 * \brief The CDatabaseTree class
 * \ingroup DATABASE_API
 */
class PLUGIN_EXPORT CDatabaseTree : public CDatabase
{
    Q_OBJECT

public:
    explicit CDatabaseTree(const QString& szPrefix = QString(),
                           QObject* parent = nullptr);

    // ====== Leaf operate ======
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

    [[nodiscard]] TreeItem GetLeaf(int id);
    /*!
     * \brief Get the leaves under nodeId
     * \param nodeId:
     *          - < 0: Get all leaves
     *          - >= 0: Get the leaves under the node
     */
    [[nodiscard]] QList<TreeItem> GetLeaves(int nodeId);
    /*!
     * \brief Get leaves
     * \param value: Get the leaves of value
     */
    [[nodiscard]] QList<TreeItem> GetLeavesByKey(int key);
    [[nodiscard]] QList<TreeItem> GetLeavesByKey(QList<int> key);
    [[nodiscard]] int GetLeafCount(int parentId = 0);

    // ====== Node operate ======
    virtual int AddNode(const QString &name, int parentId = 0);
    virtual bool RenameNode(int id, const QString &newName);
    virtual bool DeleteNode(int id, bool delKey = false);
    virtual bool MoveNode(int id, int newParentId);
    [[nodiscard]] TreeItem GetNode(int id);
    [[nodiscard]] QList<TreeItem> GetAllNodes();
    [[nodiscard]] QList<TreeItem> GetSubNodes(int parentId);
    [[nodiscard]] int GetNodeCount(int nParentId = 0);

    /*!
     * \~chinese
     * \brief 得到指定id节点下的所有节点和叶子数。不递归。
     * \param parentId: 0, 得到树中所有节点和叶子。
     * \~english
     * \brief Get the count of parentId. include nodes and leaves
     * \param parentId: 0, Get all count
     * \return
     */
    [[nodiscard]] int GetCount(int parentId = 0);

    [[nodiscard]] virtual bool ExportToJson(QJsonObject& obj) override;
    [[nodiscard]] virtual bool ImportFromJson(const QJsonObject& obj) override;

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
    [[nodiscard]] virtual bool OnDeleteKey(int key);
    [[nodiscard]] virtual bool OnInitializeDatabase() override;
    [[nodiscard]] virtual bool OnInitializeSqliteDatabase() override;
    [[nodiscard]] virtual bool OnInitializeMySqlDatabase() override;

private:
    QString m_szTableName;
    CDatabaseFolder m_FolderDB;
};
