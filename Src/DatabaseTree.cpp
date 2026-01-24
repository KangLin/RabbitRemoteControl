// Author: Kang Lin <kl222@126.com>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QLoggingCategory>

#include "DatabaseTree.h"

static Q_LOGGING_CATEGORY(log, "DB.Tree")
TreeItem::TreeItem()
    : m_Type(Node)
    , m_id(0)
    , m_nKey(0)
    , m_ParentId(0)
    , m_SortOrder(0)
{}

TreeItem::TreeItem(const TreeItem &item)
{
    m_Type = item.m_Type;
    m_id = item.m_id;
    m_szName = item.m_szName;
    m_nKey = item.m_nKey;
    m_ParentId = item.m_ParentId;
    m_SortOrder = item.m_SortOrder;
    m_CreateTime = item.m_CreateTime;
    m_ModifyTime = item.m_ModifyTime;
    m_LastVisitTime = item.m_LastVisitTime;
}

bool TreeItem::IsNode() const
{
    return Node == GetType();
}

bool TreeItem::IsLeaf() const
{
    return Leaf == GetType();
}

TreeItem::TYPE TreeItem::GetType() const
{
    return m_Type;
}

void TreeItem::SetType(TYPE type)
{
    m_Type = type;
}

int TreeItem::GetId() const
{
    return m_id;
}

void TreeItem::SetId(int newId)
{
    m_id = newId;
}

QString TreeItem::GetName() const
{
    return m_szName;
}

void TreeItem::SetName(const QString &newName)
{
    m_szName = newName;
}

int TreeItem::GetParentId() const
{
    return m_ParentId;
}

void TreeItem::SetParentId(int newParentId)
{
    m_ParentId = newParentId;
}

int TreeItem::GetSortOrder() const
{
    return m_SortOrder;
}

void TreeItem::SetSortOrder(int newSortOrder)
{
    m_SortOrder = newSortOrder;
}

int TreeItem::GetKey() const
{
    return m_nKey;
}

void TreeItem::SetKey(int newNKey)
{
    m_nKey = newNKey;
}

QDateTime TreeItem::GetCreateTime() const
{
    return m_CreateTime;
}

void TreeItem::SetCreateTime(const QDateTime &newCreateTime)
{
    m_CreateTime = newCreateTime;
}

// QIcon TreeItem::GetIcon() const
// {
//     if (!m_Icon.isNull()) {
//         return m_Icon;
//     }

//     QString url = m_Value;
//     if (url.startsWith("https://")) {
//         return QIcon::fromTheme("security-high");
//     } else if (url.startsWith("http://")) {
//         return QIcon::fromTheme("security-low");
//     }

//     if (IsNode()) {
//         return QIcon::fromTheme("folder");
//     }

//     return QIcon::fromTheme("file");
// }

// void TreeItem::SetIcon(const QIcon &newIcon)
// {
//     m_Icon = newIcon;
// }

QDateTime TreeItem::GetModifyTime() const
{
    return m_ModifyTime;
}

void TreeItem::SetModifyTime(const QDateTime &newModifyTime)
{
    m_ModifyTime = newModifyTime;
}

QDateTime TreeItem::GetLastVisitTime() const
{
    return m_LastVisitTime;
}

void TreeItem::SetLastVisitTime(const QDateTime &newLastVisitTime)
{
    m_LastVisitTime = newLastVisitTime;
}

CDatabaseFolder::CDatabaseFolder(QObject *parent)
    : CDatabase(parent)
{
    m_szConnectName = "folder_connect";
    m_szTableName = "folders";
}

CDatabaseFolder::CDatabaseFolder(const QString &szPrefix, QObject *parent)
    : CDatabaseFolder(parent)
{
    if(!szPrefix.isEmpty())
        m_szTableName = szPrefix + "_" + m_szTableName;
}

bool CDatabaseFolder::OnInitializeDatabase()
{
    QSqlQuery query(GetDatabase());

    // 启用外键约束
    query.exec("PRAGMA foreign_keys = ON");

    // 创建文件夹表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS "
        + m_szTableName +
        " ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL,"
        "    parent_id INTEGER DEFAULT 0,"
        "    sort_order INTEGER DEFAULT 0,"
        "    created_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")"
        );

    if (!success) {
        qCritical(log) << "Failed to create folders table:"
                       << m_szTableName << query.lastError().text();
        return false;
    }

    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_parent ON " + m_szTableName + "(parent_id)");

    return true;
}


int CDatabaseFolder::AddFolder(const QString &name, int parentId)
{
    QSqlQuery query(GetDatabase());

    // 获取最大排序值
    query.prepare("SELECT MAX(sort_order) FROM " + m_szTableName + " WHERE parent_id = :parent_id");
    query.bindValue(":parent_id", parentId);
    query.exec();

    int maxOrder = 0;
    if (query.next()) {
        maxOrder = query.value(0).toInt() + 1;
    }

    query.prepare(
        "INSERT INTO " + m_szTableName + " (name, parent_id, sort_order) "
                                         "VALUES (:name, :parent_id, :sort_order)"
        );
    query.bindValue(":name", name);
    query.bindValue(":parent_id", parentId);
    query.bindValue(":sort_order", maxOrder);

    int id = 0;
    bool success = query.exec();
    if (success) {
        id = query.lastInsertId().toInt();
        if(id > 0) {
            emit sigAddFolder(id, parentId);
            emit sigChanged();
        }
    } else
        qCritical(log) << "Failed to add folders:" << query.lastError().text();

    return id;
}

bool CDatabaseFolder::RenameFolder(int id, const QString &newName)
{
    QSqlQuery query(GetDatabase());
    query.prepare("UPDATE " + m_szTableName + " SET name = :name WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":name", newName);

    bool success = query.exec();

    if (success)
        emit sigChanged();
    else
        qCritical(log) << "Failed to rename folders:" << query.lastError().text();

    return success;
}

bool CDatabaseFolder::DeleteFolder(int id, std::function<int (int parentId)> cbDeleteLeaf)
{
    // 删除子目录
    auto folders = GetSubFolders(id);
    foreach(auto f, folders) {
        DeleteFolder(f.GetId());
    }

    if(cbDeleteLeaf)
        cbDeleteLeaf(id);

    // 删除其下面的所有条目
    OnDeleteLeafs(id);

    // 删除文件夹
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM " + m_szTableName + " WHERE id = :id");
    query.bindValue(":id", id);

    bool success = query.exec();

    if (success)
        emit sigChanged();
    else
        qCritical(log) << "Failed to delete folders:" << query.lastError().text();

    return success;
}

bool CDatabaseFolder::OnDeleteLeafs(int id)
{
    return true;
}

QString CDatabaseFolder::GetTableName() const
{
    return m_szTableName;
}

void CDatabaseFolder::SetTableName(const QString &newSzTableName)
{
    m_szTableName = newSzTableName;
}

bool CDatabaseFolder::MoveFolder(int id, int newParentId)
{
    QSqlQuery query(GetDatabase());
    query.prepare("UPDATE " + m_szTableName + " SET parent_id = :parent_id WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":parent_id", newParentId);

    bool success = query.exec();

    if (success)
        emit sigChanged();
    else
        qCritical(log) << "Failed to move folders:" << query.lastError().text();

    return success;
}

TreeItem CDatabaseFolder::GetFolder(int id)
{
    TreeItem folder;
    if(0 >= id) return folder;
    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, name, parent_id, sort_order, created_time "
        "FROM " + m_szTableName + " "
        "WHERE id=:id"
        );
    query.bindValue(":id", id);
    if (query.exec()) {
        if(query.next()) {
            folder.SetType(TreeItem::Node);
            folder.SetId(query.value(0).toInt());
            folder.SetName(query.value(1).toString());
            folder.SetParentId(query.value(2).toInt());
            folder.SetSortOrder(query.value(3).toInt());
            folder.SetCreateTime(query.value(4).toDateTime());
        }
    } else
        qCritical(log) << "Failed to get folder:" << id << query.lastError().text();

    return folder;
}

QList<TreeItem> CDatabaseFolder::GetAllFolders()
{
    QList<TreeItem> folders;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, name, parent_id, sort_order, created_time "
        "FROM " + m_szTableName + " "
                          "ORDER BY parent_id, sort_order, name"
        );

    if (query.exec()) {
        while (query.next()) {
            TreeItem folder;
            folder.SetType(TreeItem::Node);
            folder.SetId(query.value(0).toInt());
            folder.SetName(query.value(1).toString());
            folder.SetParentId(query.value(2).toInt());
            folder.SetSortOrder(query.value(3).toInt());
            folder.SetCreateTime(query.value(4).toDateTime());

            folders.append(folder);
        }
    }

    return folders;
}

QList<TreeItem> CDatabaseFolder::GetSubFolders(int parentId)
{
    QList<TreeItem> folders;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, name, parent_id, sort_order, created_time "
        "FROM " + m_szTableName + " "
                          "WHERE parent_id = :parent_id "
                          "ORDER BY sort_order, name"
        );
    query.bindValue(":parent_id", parentId);

    if (query.exec()) {
        while (query.next()) {
            TreeItem folder;
            folder.SetType(TreeItem::Node);
            folder.SetId(query.value(0).toInt());
            folder.SetName(query.value(1).toString());
            folder.SetParentId(query.value(2).toInt());
            folder.SetSortOrder(query.value(3).toInt());
            folder.SetCreateTime(query.value(4).toDateTime());

            folders.append(folder);
        }
    }

    return folders;
}

int CDatabaseFolder::GetCount(int parentId)
{
    QSqlQuery query(GetDatabase());
    if(0 == parentId) {
        query.prepare("SELECT COUNT(*) FROM " + m_szTableName);
    } else {
        query.prepare("SELECT COUNT(*) FROM " + m_szTableName + " WHERE parent_id=:id");
        query.bindValue(":id", parentId);
    }
    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}

CDatabaseTree::CDatabaseTree(QObject *parent)
    : CDatabase(parent)
{
    m_szTableName = "tree";
    m_szConnectName = "tree_connect";
}

CDatabaseTree::CDatabaseTree(const QString &szPrefix, QObject *parent)
    : CDatabaseTree(parent)
{
    if(!szPrefix.isEmpty())
        m_szTableName = szPrefix + "_" + m_szTableName;
    m_FolderDB.SetTableName(szPrefix + "_" + m_FolderDB.GetTableName());
}

bool CDatabaseTree::OnInitializeDatabase()
{
    bool bRet = false;

    m_FolderDB.SetDatabase(GetDatabase());
    bRet = m_FolderDB.OnInitializeDatabase();
    if(!bRet) return false;
    bRet = connect(&m_FolderDB, &CDatabaseFolder::sigAddFolder,
                   this, &CDatabaseTree::sigAddFolder);
    Q_ASSERT(bRet);

    QSqlQuery query(GetDatabase());

    // 启用外键约束
    query.exec("PRAGMA foreign_keys = ON");

    // 创建书签表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS " + m_szTableName + " ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT,"
        "    key INTEGER DEFAULT 0,"
        "    created_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    modified_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    last_visit_time DATETIME,"
        "    parent_id INTEGER DEFAULT 0"
        ")"
        );

    if (!success) {
        qCritical(log) << "Failed to create tree table:" << m_szTableName << query.lastError().text();
        return false;
    }

    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_key ON " + m_szTableName + "(key)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_" + m_szTableName + "_parent_id ON " + m_szTableName + "(parent_id)");

    return true;
}

int CDatabaseTree::Add(const TreeItem &item)
{
    QSqlQuery query(GetDatabase());

    query.prepare(
        "INSERT INTO " + m_szTableName + " (name, key, "
        "created_time, modified_time, last_visit_time, parent_id) "
        "VALUES (:name, :key, "
        ":created_time, :modified_time, :last_visit_time, :parent_id)"
        );

    query.bindValue(":name", item.GetName());
    query.bindValue(":key", item.GetKey());
    QDateTime time = QDateTime::currentDateTime();
    query.bindValue(":created_time", time);
    query.bindValue(":modified_time", time);
    query.bindValue(":last_visit_time", time);
    query.bindValue(":parent_id", item.GetParentId());

    bool success = query.exec();

    if (!success) {
        qCritical(log) << "Failed to add tree item:" << query.lastError().text();
        return 0;
    }

    int id = query.lastInsertId().toInt();
    if(0 < id)
        emit sigAdd(id, item.GetParentId());
    return id;
}

bool CDatabaseTree::Update(const TreeItem &item)
{
    QSqlQuery query(GetDatabase());
    query.prepare(
        "UPDATE " + m_szTableName + " SET "
        "name = :name, "
        "key = :key, "
        "created_time = :created_time, "
        "modified_time = :modified_time, "
        "last_visit_time = :last_visit_time, "
        "parent_id = :parent_id "
        "WHERE id = :id"
        );
    query.bindValue(":name", item.GetName());
    query.bindValue(":key", item.GetKey());
    query.bindValue(":created_time", item.GetCreateTime());
    query.bindValue(":modified_time", QDateTime::currentDateTime());
    query.bindValue(":last_visit_time", item.GetLastVisitTime());
    query.bindValue(":parent_id", item.GetParentId());
    query.bindValue(":id", item.GetId());
    qDebug(log) << "Sql:" << query.executedQuery();
    qDebug(log) << "Bound value:" << query.boundValues();
    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to update tree item:" << m_szTableName << query.lastError().text();
    }
    return success;
}

bool CDatabaseTree::Delete(int id, bool delKey)
{
    // 如果是最后的一个，则从 key 相关表中删除 key
    if(delKey) {
        auto leaf = GetLeaf(id);
        if(leaf.GetKey() > 0) {
            auto leaves = GetLeavesByKey(leaf.GetKey());
            if(leaves.count() == 1) {
                bool ok = OnDeleteKey(leaf.GetKey());
                if(!ok)
                    return ok;
            }
        }
    }
    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM " + m_szTableName + " WHERE id = :id");
    query.bindValue(":id", id);

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to delete item:" << m_szTableName << query.lastError().text();
    }

    return success;
}

bool CDatabaseTree::Delete(QList<int> items, bool delKey)
{
    if(items.isEmpty()) return false;

    if(delKey) {
        foreach(auto id, items) {
            // 如果是最后的一个，则从 key 相关表中删除 key
            auto leaf = GetLeaf(id);
            if(leaf.GetKey() > 0) {
                auto leaves = GetLeavesByKey(leaf.GetKey());
                if(leaves.count() == 1) {
                    bool ok = OnDeleteKey(leaf.GetKey());
                    if(!ok)
                        return ok;
                }
            }
        }
    }

    QSqlQuery query(GetDatabase());
    QString szSql = "DELETE FROM " + m_szTableName + " WHERE ";
    int i = 0;
    foreach(auto id, items) {
        if(0 == i) {
            i++;
            szSql += " id = " + QString::number(id);
        } else {
            szSql += " OR id = " + QString::number(id);
        }
    }
    bool success = query.exec(szSql);
    if (!success) {
        qCritical(log) << "Failed to delete item:" << m_szTableName << query.lastError().text();
    }

    return success;
}

bool CDatabaseTree::DeleteChild(int parentId, bool delKey)
{
    if(delKey) {
        auto leaves = GetLeaves(parentId);
        foreach(auto leaf, leaves) {
            if(!Delete(leaf.GetId(), delKey))
                return false;
        }
        return true;
    }

    QSqlQuery query(GetDatabase());
    query.prepare("DELETE FROM " + m_szTableName + " WHERE parent_id = :parent_id");
    query.bindValue(":parent_id", parentId);

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to delete child item:" << m_szTableName << query.lastError().text();
    }

    return success;
}

bool CDatabaseTree::Move(int id, int newParent)
{
    QSqlQuery query(GetDatabase());
    query.prepare(
        "UPDATE " + m_szTableName + " SET "
        "parent_id = :parent_id WHERE id = :id");
    query.bindValue(":parent_id", newParent);
    query.bindValue(":id", id);

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to add tree item:" << m_szTableName << query.lastError().text();
    }

    return success;
}

TreeItem CDatabaseTree::GetLeaf(int id)
{
    TreeItem item;
    item.SetType(TreeItem::Leaf);

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT name, key,  "
        "created_time, modified_time, last_visit_time, parent_id FROM " + m_szTableName +
        " WHERE id = :id");
    query.bindValue(":id", id);

    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to get leaf:" << m_szTableName << query.lastError().text();
        return item;
    }

    if(query.next()) {
        item.SetId(id);
        item.SetName(query.value(0).toString());
        item.SetKey(query.value(1).toInt());
        item.SetCreateTime(query.value(2).toDateTime());
        item.SetModifyTime(query.value(3).toDateTime());
        item.SetLastVisitTime(query.value(4).toDateTime());
        item.SetParentId(query.value(5).toInt());
    }
    return item;
}

QList<TreeItem> CDatabaseTree::GetLeaves(int nodeId)
{
    QList<TreeItem> items;
    QSqlQuery query(GetDatabase());
    QString szSql;
    szSql = "SELECT id, name, key, "
            "created_time, modified_time, last_visit_time, parent_id "
            "FROM " + m_szTableName;
    if(0 <= nodeId)
        szSql += " WHERE parent_id = :parent_id";
    query.prepare(szSql);
    query.bindValue(":parent_id", nodeId);
    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to get leaves:" << m_szTableName << query.lastError().text();
        return items;
    }

    while(query.next()) {
        TreeItem item;
        item.SetType(TreeItem::Leaf);
        item.SetId(query.value(0).toInt());
        item.SetName(query.value(1).toString());
        item.SetKey(query.value(2).toInt());
        item.SetCreateTime(query.value(3).toDateTime());
        item.SetModifyTime(query.value(4).toDateTime());
        item.SetLastVisitTime(query.value(5).toDateTime());
        item.SetParentId(query.value(6).toInt());

        items.append(item);
    }
    return items;
}

QList<TreeItem> CDatabaseTree::GetLeavesByKey(int key)
{
    QList<TreeItem> items;

    QSqlQuery query(GetDatabase());
    QString szSql;
    szSql = "SELECT id, name, "
            "created_time, modified_time, last_visit_time, parent_id "
            "FROM " + m_szTableName +
            " WHERE key = :key";
    query.prepare(szSql);
    query.bindValue(":key", key);
    bool success = query.exec();
    if (!success) {
        qCritical(log) << "Failed to get leaves:" << m_szTableName << query.lastError().text();
        return items;
    }

    while(query.next()) {
        TreeItem item;
        item.SetType(TreeItem::Leaf);
        item.SetId(query.value(0).toInt());
        item.SetName(query.value(1).toString());
        item.SetKey(key);
        item.SetCreateTime(query.value(2).toDateTime());
        item.SetModifyTime(query.value(3).toDateTime());
        item.SetLastVisitTime(query.value(4).toDateTime());
        item.SetParentId(query.value(5).toInt());

        items.append(item);
    }
    return items;
}

QList<TreeItem> CDatabaseTree::GetLeavesByKey(QList<int> key)
{
    QList<TreeItem> items;
    if(key.isEmpty()) return items;

    QSqlQuery query(GetDatabase());
    QString szSql;
    szSql = "SELECT id, name, key, "
            "created_time, modified_time, last_visit_time, parent_id "
            "FROM " + m_szTableName +
            " WHERE ";
    int i = 0;
    foreach(auto KeyId, key) {
        if(0 == i++) {
            szSql += " key = " + QString::number(KeyId);
            continue;
        }
            szSql += " OR key = " + QString::number(KeyId);
    }
    bool success = query.exec(szSql);
    if (!success) {
        qCritical(log) << "Failed to get leaves:" << m_szTableName << query.lastError().text();
        return items;
    }

    while(query.next()) {
        TreeItem item;
        item.SetType(TreeItem::Leaf);
        item.SetId(query.value(0).toInt());
        item.SetName(query.value(1).toString());
        item.SetKey(query.value(2).toInt());
        item.SetCreateTime(query.value(3).toDateTime());
        item.SetModifyTime(query.value(4).toDateTime());
        item.SetLastVisitTime(query.value(5).toDateTime());
        item.SetParentId(query.value(6).toInt());

        items.append(item);
    }
    return items;
}

int CDatabaseTree::GetLeafCount(int parentId)
{
    QSqlQuery query(GetDatabase());
    if(0 == parentId) {
        query.prepare("SELECT COUNT(*) FROM " + m_szTableName);
    } else {
        query.prepare("SELECT COUNT(*) FROM " + m_szTableName + " WHERE parent_id=:id");
        query.bindValue(":id", parentId);
    }
    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}

int CDatabaseTree::AddNode(const QString &name, int parentId)
{
    return m_FolderDB.AddFolder(name, parentId);
}

bool CDatabaseTree::RenameNode(int id, const QString &newName)
{
    return m_FolderDB.RenameFolder(id, newName);
}

bool CDatabaseTree::DeleteNode(int id, bool delKey)
{
    return m_FolderDB.DeleteFolder(id, [&, delKey](int parentId)->int {
        return DeleteChild(parentId, delKey);
    });
}

bool CDatabaseTree::MoveNode(int id, int newParentId)
{
    return m_FolderDB.MoveFolder(id, newParentId);
}

TreeItem CDatabaseTree::GetNode(int id)
{
    return m_FolderDB.GetFolder(id);
}

QList<TreeItem> CDatabaseTree::GetAllNodes()
{
    return m_FolderDB.GetAllFolders();
}

QList<TreeItem> CDatabaseTree::GetSubNodes(int parentId)
{
    return m_FolderDB.GetSubFolders(parentId);
}

int CDatabaseTree::GetNodeCount(int nParentId)
{
    return m_FolderDB.GetCount(nParentId);
}

int CDatabaseTree::GetCount(int parentId)
{
    return GetNodeCount(parentId) + GetLeafCount(parentId);
}

bool CDatabaseTree::OnDeleteKey(int key)
{
    return 0;
}
