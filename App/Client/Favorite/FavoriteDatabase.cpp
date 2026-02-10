// Author: Kang Lin <kl222@126.com>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QLoggingCategory>
#include "FavoriteDatabase.h"
#include "IconUtils.h"

static Q_LOGGING_CATEGORY(log, "App.Favorite.Db")
CFavoriteDatabase::CFavoriteDatabase(QObject *parent)
    : CDatabaseTree{"favorite", parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

bool CFavoriteDatabase::OnInitializeSqliteDatabase()
{
    QSqlQuery query(GetDatabase());

    // Create favorite table
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

    // Create index
    success = query.exec("CREATE INDEX IF NOT EXISTS idx_favorite_file ON favorite(file)");
    if (!success) {
        qWarning(log) << "Failed to create idx_favorite_file." << query.lastError().text();
    }

    // Drop trigger if exists
    if (!query.exec("DROP TRIGGER IF EXISTS delete_icon_after_favorite")) {
        qDebug(log) << "Failed to drop trigger delete_icon_after_favorite:" << query.lastError().text();
    }
    // Create trigger
    QString szSql = R"(
        CREATE TRIGGER delete_icon_after_favorite
        AFTER DELETE ON favorite
        FOR EACH ROW
        BEGIN
            DELETE FROM icon
            WHERE id = OLD.icon
              AND OLD.icon != 0
              AND NOT EXISTS (
                  SELECT 1 FROM favorite WHERE icon = OLD.icon
              )
              AND NOT EXISTS (
                  SELECT 1 FROM recent WHERE icon = OLD.icon
              );
        END;
    )";
    success = query.exec(szSql);
    if (!success) {
        qWarning(log) << "Failed to create trigger delete_icon_after_favorite." << query.lastError().text();
    }
    success = CDatabaseTree::OnInitializeSqliteDatabase();
    return success;
}

bool CFavoriteDatabase::OnInitializeMySqlDatabase()
{
    bool success = false;
    QSqlQuery query(GetDatabase());

    // Create favorite table
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS favorite ("
        "    id INTEGER PRIMARY KEY AUTO_INCREMENT," // the id is the key of tree table
        "    name TEXT NOT NULL,"
        "    icon INTEGER DEFAULT 0,"
        "    file TEXT NOT NULL,"
        "    description TEXT,"
        "    UNIQUE KEY uk_favorite_file (file(255))"
        ")"
        );
    if (!success) {
        qCritical(log) << "Failed to create favorite table:"
                       << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }

    // Drop trigger if exists
    if (!query.exec("DROP TRIGGER IF EXISTS delete_icon_after_favorite")) {
        qDebug(log) << "Failed to drop trigger delete_icon_after_favorite:"
                    << query.lastError().text()
                    << "Sql:" << query.executedQuery();
    }
    // Create trigger
    QString szSql = R"(
        CREATE TRIGGER delete_icon_after_favorite
        AFTER DELETE ON favorite
        FOR EACH ROW
        BEGIN
            DECLARE favorite_count INT DEFAULT 0;
            DECLARE recent_count INT DEFAULT 0;
            
            IF OLD.icon != 0 THEN
                -- 统计favorite表中引用该icon的数量
                SELECT COUNT(*) INTO favorite_count 
                FROM favorite 
                WHERE icon = OLD.icon;
                
                -- 统计recent表中引用该icon的数量
                SELECT COUNT(*) INTO recent_count 
                FROM recent 
                WHERE icon = OLD.icon;
                
                -- 如果都没有引用，则删除icon
                IF favorite_count = 0 AND recent_count = 0 THEN
                    DELETE FROM icon WHERE id = OLD.icon;
                END IF;
            END IF;
        END
    )";
    success = query.exec(szSql);
    if (!success) {
        qWarning(log) << "Failed to create trigger delete_icon_after_favorite."
                      << query.lastError().text()
                      << "Sql:" << query.executedQuery();
    }
    
    return CDatabaseTree::OnInitializeMySqlDatabase();
}

bool CFavoriteDatabase::OnInitializeDatabase()
{
    bool bRet = false;
    bRet = CDatabaseTree::OnInitializeDatabase();
    if(!bRet) return false;

    m_IconDB.SetDatabase(GetDatabase(), m_pPara);
    bRet = m_IconDB.OnInitializeDatabase();
    return bRet;
}

int CFavoriteDatabase::AddFavorite(const QString &szFile,
    const QString &szName, const QIcon &icon,
    const QString szDescription, int parentId)
{
    int ret = 0;
    if (szName.trimmed().isEmpty()) {
        qWarning(log) << "Favorite name cannot be empty";
        return ret;
    }

    if (szFile.trimmed().isEmpty()) {
        qCritical(log) << "Favorite file cannot be empty";
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

bool CFavoriteDatabase::UpdateFavorite(
    int id, const QString& szName,
    const QIcon &icon, const QString szDescription)
{
    QSqlQuery query(GetDatabase());
    QString szSql;
    if(!szName.isEmpty())
        szSql += "name=\"" + szName + "\"";
    if(!icon.isNull()) {
        if(!szSql.isEmpty())
            szSql += ", ";
        szSql += "icon=" + QString::number(m_IconDB.GetIcon(icon));
    }
    if(!szDescription.isEmpty()) {
        if(!szSql.isEmpty())
            szSql += ", ";
        szSql += "description=\"" + szDescription + "\"";
    }

    szSql = "UPDATE favorite SET " + szSql + " WHERE id=" + QString::number(id);
    //qDebug(log) << "Sql:" << szSql;
    bool ok = query.exec(szSql);
    if(!ok)
        qCritical(log) << "Failed to update favorite:"
                       << id << query.lastError().text()
                       << "Sql:" << szSql;
    return ok;
}

bool CFavoriteDatabase::UpdateFavorite(
    const QString &szFile, const QString &szName,
    const QIcon &icon, const QString szDescription)
{
    QSqlQuery query(GetDatabase());
    QString szSql;
    if(!szName.isEmpty())
        szSql += "name=\"" + szName + "\"";
    if(!icon.isNull()) {
        if(!szSql.isEmpty())
            szSql += ", ";
        szSql += "icon=" + QString::number(m_IconDB.GetIcon(icon));
    }
    if(!szDescription.isEmpty()) {
        if(!szSql.isEmpty())
            szSql += ", ";
        szSql += "description=\"" + szDescription + "\"";
    }

    szSql = "UPDATE favorite SET " + szSql + " WHERE file=\"" + szFile + "\"";
    //qDebug(log) << "Sql:" << szSql;
    bool ok = query.exec(szSql);
    if(!ok)
        qCritical(log) << "Failed to update favorite:"
                       << szFile << query.lastError().text()
                       << "Sql:" << szSql;
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
    //qDebug(log) << "SQL:" << query.executedQuery();
    //qDebug(log) << "Bound value:" << query.boundValues();
    bool ok = query.exec();
    if(!ok) {
        qCritical(log) << "Failed to get favorite:"
                       << id << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return item;
    }
    if(query.next()) {
        item.id = leaf.GetId();
        item.parentId = leaf.GetParentId();
        item.szName = query.value(1).toString();
        item.icon = m_IconDB.GetIcon(query.value(2).toInt());
        item.szFile = query.value(3).toString();
        item.szDescription = query.value(4).toString();
        item.type = TreeItem::Leaf;
    }
    return item;
}

QList<CFavoriteDatabase::Item> CFavoriteDatabase::GetFavorite(const QString &szFile)
{
    QList<CFavoriteDatabase::Item> lstItems;
    Item item;
    item.type = TreeItem::Leaf;

    QSqlQuery query(GetDatabase());
    query.prepare(
        "SELECT id, name, icon, file, description FROM favorite "
        "WHERE file = :file"
        );
    query.bindValue(":file", szFile);
    //qDebug(log) << "SQL:" << query.executedQuery();
    //qDebug(log) << "Bound value:" << query.boundValues();
    bool ok = query.exec();
    if(!ok) {
        qCritical(log) << "Failed to get favorite:"
                       << szFile << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return lstItems;
    }
    if(query.next()) {
        item.szName = query.value(1).toString();
        item.icon = m_IconDB.GetIcon(query.value(2).toInt());
        item.szFile = query.value(3).toString();
        item.szDescription = query.value(4).toString();
        item.type = TreeItem::Leaf;

        auto leaf = GetLeavesByKey(query.value(0).toInt());
        foreach(auto l, leaf) {
            item.id = l.GetId();
            item.parentId = l.GetParentId();
            lstItems << item;
        }
    }
    return lstItems;
}

CFavoriteDatabase::Item CFavoriteDatabase::GetGroup(int id)
{
    auto f = GetNode(id);
    Item item;
    item.id = f.GetId();
    item.parentId = f.GetParentId();
    item.szName = f.GetName();
    item.type = f.GetType();
    return item;
}

QList<CFavoriteDatabase::Item> CFavoriteDatabase::GetChildren(int parentId)
{
    QList<Item> children;
    // Get nodes
    auto folders = GetSubNodes(parentId);
    foreach(auto f, folders) {
        Item item;
        item.id = f.GetId();
        item.parentId = f.GetParentId();
        item.szName = f.GetName();
        item.type = f.GetType();
        children << item;
    }
    // Get leaves
    auto leaves = GetLeaves(parentId);
    foreach(auto l, leaves) {
        Item item = GetFavorite(l.GetId());
        children << item;
    }
    return children;
}

bool CFavoriteDatabase::OnDeleteKey(int key)
{
    QSqlQuery query(GetDatabase());
    query.prepare(
        "DELETE FROM favorite "
        "WHERE id = :id"
        );
    query.bindValue(":id", key);
    //qDebug(log) << "SQL:" << query.executedQuery();
    //qDebug(log) << "Bound value:" << query.boundValues();
    bool ok = query.exec();
    if(!ok) {
        qCritical(log) << "Failed to delete favorite:"
                       << key << query.lastError().text()
                       << "Sql:" << query.executedQuery();
        return false;
    }
    return ok;
}

bool CFavoriteDatabase::ExportToJson(QJsonObject &obj)
{
    bool bRet = true;
    QJsonArray root;
    bRet = ExportToJson(0, root);
    if(bRet)
        obj.insert("favorite", root);
    return bRet;
}

bool CFavoriteDatabase::ImportFromJson(const QJsonObject &obj)
{
    QJsonArray favorites = obj["favorite"].toArray();
    if(favorites.isEmpty()) {
        qCritical(log) << "The file format is error. Json without favorite";
        return false;
    }

    return ImportFromJson(0, favorites);
}

bool CFavoriteDatabase::ImportFromJson(int parentId, const QJsonArray &obj)
{
    for(auto it = obj.begin(); it != obj.end(); it++) {
        QJsonObject itemObj = it->toObject();
        if(itemObj.isEmpty()) continue;
        if(TreeItem::Node == itemObj["type"].toInt()) {
            QString szName = itemObj["name"].toString();
            int id = AddNode(szName, parentId);
            QJsonArray items = itemObj[szName].toArray();
            if(items.isEmpty()) continue;
            ImportFromJson(id, items);
            continue;
        }

        QString szFile;
        bool bRet = CDatabaseFile::ImportFileFromJson(itemObj, szFile);
        if(!bRet) continue;
        QIcon icon;
        bRet = CDatabaseIcon::ImportIconFromJson(itemObj, icon);
        if(!bRet) continue;

        AddFavorite(szFile, itemObj["name"].toString(), icon, itemObj["description"].toString(), parentId);
    }

    return true;
}

bool CFavoriteDatabase::ExportToJson(int parentId, QJsonArray &obj)
{
    auto items = GetChildren(parentId);
    foreach(auto item, items) {
        QJsonObject oItem;
        if(item.isFolder()) {
            QJsonArray aItem;
            bool bRet = ExportToJson(item.id, aItem);
            if(!bRet) continue;

            oItem.insert("type", TreeItem::Node);
            oItem.insert("name", item.szName);
            oItem.insert(item.szName, aItem);

        } else {
            oItem.insert("type", item.type);
            oItem.insert("name", item.szName);

            // File
            bool bRet = CDatabaseFile::ExportFileToJson(item.szFile, oItem);
            if(!bRet) continue;

            // Icon
            bRet = CDatabaseIcon::ExportIconToJson(item.GetIcon(), oItem);
            if(!bRet) continue;

            oItem.insert("description", item.szDescription);
        }

        obj.append(oItem);

    }
    return true;
}
