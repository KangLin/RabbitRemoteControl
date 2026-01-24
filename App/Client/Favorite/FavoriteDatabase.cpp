// Author: Kang Lin <kl222@126.com>

#include <QSqlQuery>
#include <QSqlError>
#include <QLoggingCategory>
#include "FavoriteDatabase.h"

static Q_LOGGING_CATEGORY(log, "App.Favorite.Db")
CFavoriteDatabase::CFavoriteDatabase(QObject *parent)
    : CDatabaseTree{"favorite", parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

bool CFavoriteDatabase::OnInitializeDatabase()
{
    QSqlQuery query(GetDatabase());

    // 创建书签表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS favorite ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT," // the id is the key of tree table
        "    name TEXT NOT NULL,"
        "    icon INTEGER DEFAULT 0,"
        "    file TEXT UNIQUE NOT NULL,"
        "    description TEXT"
        ")"
        );

    if (!success) {
        qCritical(log) << "Failed to create favorite table:" << query.lastError().text();
        return false;
    }
    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_favorite_file ON favorite(file)");

    m_IconDB.SetDatabase(GetDatabase());
    m_IconDB.OnInitializeDatabase();
    return CDatabaseTree::OnInitializeDatabase();
}

int CFavoriteDatabase::AddFavorite(
    const QIcon &icon, const QString &szName,
    const QString &szFile, const QString szDescription, int parentId)
{
    int ret = 0;
    if (szName.trimmed().isEmpty()) {
        qWarning(log) << "Favorite name cannot be empty";
        return ret;
    }

    if (szFile.trimmed().isEmpty()) {
        qWarning(log) << "Favorite file cannot be empty";
        return ret;
    }

    // 验证 parentId 是否存在（如果parentId不为0）
    if (parentId > 0) {
        if (GetNode(parentId).GetId() == 0) {
            qWarning(log) << "Parent item with ID" << parentId << "does not exist";
            return ret;
        }
    }

    QSqlDatabase db = GetDatabase();
    bool success = false;
    // 使用事务确保数据一致性
    if (!db.transaction()) {
        qCritical(log) << "Failed to start transaction:" << db.lastError().text();
        return ret;
    }

    try {
        QSqlQuery query(db);
        int key = 0;
        //检查是否已存在
        query.prepare(
            "SELECT id FROM favorite "
            "WHERE file=:file");
        query.bindValue(":file", szFile);
        if(query.exec() && query.next()) {
            key = query.value(0).toInt();
        }

        if(0 == key) {
            // 插入新记录
            query.prepare(
                "INSERT INTO favorite (name, icon, file, description)"
                "VALUES (:name, :icon, :file, :description)"
                );
            query.bindValue(":name", szName);
            query.bindValue(":icon", m_IconDB.GetIcon(icon));
            query.bindValue(":file", szFile);
            query.bindValue(":description", szDescription);

            success = query.exec();
            if (!success) {
                QString szErr = "Failed to insert favorite table: " + query.lastError().text();
                qCritical(log) << szErr;
                throw std::runtime_error(szErr.toStdString());
            }

            key = query.lastInsertId().toInt();
            if(0 >= key) {
                QString szErr = "Failed to insert favorite table";
                throw std::runtime_error(szErr.toStdString());
            }
        }

        // 在 tree 表中增加
        TreeItem item;
        item.SetKey(key);
        item.SetParentId(parentId);
        int id = Add(item);
        if (0 >= id) {
            QString szErr = "Failed to insert favorite folder table";
            throw std::runtime_error(szErr.toStdString());
        }

        // 提交事务
        if (!db.commit()) {
            QString szErr = "Failed to commit transaction:" + db.lastError().text();
            throw std::runtime_error(szErr.toStdString());
        }

        qDebug(log) << "Successfully added favorite:" << szName << "ID:" << key << "Parent:" << parentId;
        ret = id;

    } catch (const std::exception &e) {
        qCritical(log) << "Exception in AddFavorite:" << e.what();
        db.rollback();
    } catch (...) {
        qCritical(log) << "Unknown exception in AddFavorite";
        db.rollback();
    }

    return ret;
}

bool CFavoriteDatabase::UpdateFavorite(const QString& szName, int id)
{
    QSqlQuery query(GetDatabase());
    query.prepare(
        "UPDATE favorite SET "
        "name = :name "
        "WHERE id = :id"
        );
    query.bindValue(":name", szName);
    query.bindValue(":id", id);
    bool ok = query.exec();
    if(!ok)
        qCritical(log) << "Failed to update favorite:" << id;
    return ok;
}

CFavoriteDatabase::Item CFavoriteDatabase::GetFavorite(int id)
{
    Item item;
    item.type = TreeItem::Leaf;
    auto leaf = GetLeaf(id);
    if(leaf.GetId() == 0)
        return item;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, name, icon, file, description FROM favorite "
        "WHERE id = :id"
        );
    query.bindValue(":id", leaf.GetKey());
    qDebug(log) << "SQL:" << query.executedQuery();
    qDebug(log) << "Bound value:" << query.boundValues();
    bool ok = query.exec();
    if(!ok) {
        qCritical(log) << "Failed to update favorite:"
                       << id << query.lastError().text();
        return item;
    }
    if(query.next()) {
        item.id = leaf.GetId();
        item.szName = query.value(1).toString();
        item.icon = m_IconDB.GetIcon(query.value(2).toInt());
        item.szFile = query.value(3).toString();
        item.szDescription = query.value(4).toString();
        item.type = TreeItem::Leaf;
    }
    return item;
}

CFavoriteDatabase::Item CFavoriteDatabase::GetGroup(int id)
{
    auto f = GetNode(id);
    Item item;
    item.id = f.GetId();
    item.szName = f.GetName();
    item.type = f.GetType();
    return item;
}

QList<CFavoriteDatabase::Item> CFavoriteDatabase::GetChildren(int parentId)
{
    QList<Item> childs;
    auto folders = GetSubNodes(parentId);
    foreach(auto f, folders) {
        Item item;
        item.id = f.GetId();
        item.szName = f.GetName();
        item.type = f.GetType();
        childs << item;
    }
    auto leaves = GetLeaves(parentId);
    foreach(auto l, leaves) {
        Item item = GetFavorite(l.GetId());
        childs << item;
    }
    return childs;
}


bool CFavoriteDatabase::OnDeleteKey(int key)
{
    QSqlQuery query(GetDatabase());
    query.prepare(
        "DELETE FROM favorite "
        "WHERE id = :id"
        );
    query.bindValue(":id", key);
    qDebug(log) << "SQL:" << query.executedQuery();
    qDebug(log) << "Bound value:" << query.boundValues();
    bool ok = query.exec();
    if(!ok) {
        qCritical(log) << "Failed to delete favorite:"
                       << key << query.lastError().text();
        return false;
    }
    return ok;
}
