// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "FavoriteModel.h"

static Q_LOGGING_CATEGORY(log, "App.Favorite.Model")
CFavoriteModel::CFavoriteModel(CFavoriteDatabase *pDatabase, QObject *parent)
    : QAbstractItemModel(parent)
    , m_pDatabase(pDatabase)
    , m_pRoot(nullptr)
{}

CFavoriteModel::~CFavoriteModel()
{
    if(m_pRoot) {
        clearTree(m_pRoot);
        delete m_pRoot;
        m_pRoot = nullptr;
    }
}

QModelIndex CFavoriteModel::index(int row, int column, const QModelIndex &parent) const
{
    //qDebug(log) << "index:" << parent;
    if(!hasIndex(row, column, parent)) return QModelIndex();
    if(0 > row || 0 != column) return QModelIndex();

    tree* parentItem = nullptr;
    if(parent.isValid())
        parentItem = (tree*)parent.internalPointer();
    else
        parentItem = m_pRoot;
    if(!parentItem)
        return QModelIndex();
    if(row < parentItem->children.size())
        return createIndex(row, column, parentItem->children[row]);

    return QModelIndex();
}

QModelIndex CFavoriteModel::parent(const QModelIndex &index) const
{
    //qDebug(log) << "parent:" << index;
    QModelIndex parentIndex;
    if(!index.isValid()) return QModelIndex();
    if(0 != index.column()) return QModelIndex();
    tree* item = (tree*)index.internalPointer();
    if(!item) return QModelIndex();
    tree* parentItem = item->parent;
    if(!parentItem || parentItem == m_pRoot)
        return QModelIndex();
    return createIndex(item->row, index.column(), parentItem);
}

int CFavoriteModel::rowCount(const QModelIndex &parent) const
{
    if(!m_pDatabase) return 0;
    if (!parent.isValid()) {
        if(!m_pRoot)
            return 0;
        else
            return m_pRoot->children.size();
    }
    if(parent.column() != 0) return 0;
    tree* item = (tree*)parent.internalPointer();
    if(!item)
        return 0;
    if(item->item.isFavorite()) return 0;
    if(item->children.size())
        return item->children.size();
    return m_pDatabase->GetCount(item->item.id);
}

int CFavoriteModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

bool CFavoriteModel::canFetchMore(const QModelIndex &parent) const
{
    //qDebug(log) << "canFatchMore:" << parent;
    if(!m_pDatabase) return false;
    int parentId = 0;
    if (parent.isValid()) {
        if(0 != parent.column()) return false;
        tree* parentItem = (tree*)parent.internalPointer();
        if(!parentItem)
            return false;
        if(parentItem->item.isFavorite())
            return false;
        parentId = parentItem->item.id;
        if(parentItem->children.isEmpty())
            return m_pDatabase->GetCount(parentId) > 0;
        else
            return false;
    }
    return true;
}

void CFavoriteModel::fetchMore(const QModelIndex &parent)
{
    qDebug(log) << "fetchMore:" << parent;
    if(!m_pDatabase) return;
    tree* parentItem = nullptr;
    if(parent.isValid())
        parentItem = static_cast<tree*>(parent.internalPointer());
    else {
        if(!m_pRoot) {
            m_pRoot = new tree();
            if(m_pRoot) {
                m_pRoot->item.type = TreeItem::Node;
                m_Folders[0] = m_pRoot;
            }
        }
        parentItem = m_pRoot;
    }
    if(!parentItem) return;
    if(parentItem->item.isFavorite()) return;
    if(!parentItem->children.isEmpty()) return;

    auto Nodes = m_pDatabase->GetChildren(parentItem->item.id);
    if(Nodes.isEmpty()) return;
    int row = 0;
    beginInsertRows(parent, 0, Nodes.size() - 1);
    foreach(auto n, Nodes) {
        tree* pTree = new tree();
        pTree->parent = parentItem;
        pTree->row = row++;
        pTree->item = n;
        if(n.isGroup())
            m_Folders[n.id] = pTree;
        parentItem->children.append(pTree);
    }
    endInsertRows();
}

QVariant CFavoriteModel::data(const QModelIndex &index, int role) const
{
    qDebug(log) << "date:" << index;
    if (!index.isValid())
        return QVariant();
    if(0 != index.column()) return QVariant();
    tree* ip = (tree*)index.internalPointer();
    if(!ip) return QVariant();
    auto item = ip->item;
    switch(role) {
    case Qt::DecorationRole:
        return item.GetIcon();
    case Qt::DisplayRole:
        return item.szName;
    case Qt::ToolTipRole:
        return item.szDescription;
    case CFavoriteModel::RoleFile:
        return item.szFile;
    case RoleNodeType:
        return item.type;
    case RoleItem:
        return QVariant::fromValue(item);
    default:
        break;
    }

    return QVariant();
}

bool CFavoriteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()) return false;
    if(0 != index.column()) return false;
    if (data(index, role) != value) {
        if(Qt::DisplayRole == role) {
            tree* ip = (tree*)index.internalPointer();
            if(!ip) return false;
            if(ip->item.isFavorite())
                m_pDatabase->UpdateFavorite(value.toString(), ip->item.id);
            else
                m_pDatabase->RenameNode(ip->item.id, value.toString());
        }
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags CFavoriteModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool CFavoriteModel::removeRows(int row, int count, const QModelIndex &parent)
{
    tree* ip = m_pRoot;
    if(parent.isValid())
        ip = (tree*)parent.internalPointer();
    if(!ip) return false;
    for(int i = row; i < row + count; i++) {
        auto t = ip->children[i];
        if(t->item.isGroup()) {
            m_pDatabase->DeleteNode(t->item.id, true);
            m_Folders.remove(t->item.id);
        } else {
            m_pDatabase->Delete(t->item.id, true);
        }
        clearTree(t);
    }
    beginRemoveRows(parent, row, row + count - 1);
    ip->children.remove(row, count);
    endRemoveRows();
    return true;
}

void CFavoriteModel::clearTree(tree* node)
{
    if(!node) return;
    foreach(auto child, node->children) {
        clearTree(child);
    }
    qDeleteAll(node->children);
    node->children.clear();
}

bool CFavoriteModel::AddFavorite(const QIcon &icon, const QString& szName,
                                 const QString &szFile, const QString szDescription,
                                 int parentId)
{
    if(!m_pDatabase || !m_Folders.contains(parentId)) return false;
    int ret = m_pDatabase->AddFavorite(icon, szName, szFile, szDescription, parentId);
    if(ret <= 0 ) return false;
    auto item = m_pDatabase->GetFavorite(ret);
    auto parent = m_Folders[parentId];

    auto t = new tree();
    if(t) {
        beginResetModel();
        t->parent = parent;
        t->row = parent->children.size();
        t->item = item;
        parent->children.append(t);
        endResetModel();
    }

    return true;
}

bool CFavoriteModel::AddNode(const QString& szName, int parentId)
{
    if(!m_pDatabase || !m_Folders.contains(parentId)) return false;
    int id = m_pDatabase->AddNode(szName, parentId);
    auto item = m_pDatabase->GetGroup(id);
    auto parent = m_Folders[parentId];

    auto t = new tree();
    if(t) {
        beginResetModel();
        t->parent = parent;
        t->row = parent->children.size();
        t->item = item;
        parent->children.append(t);
        endResetModel();
        m_Folders.insert(id, t);
    }
    return true;
}
