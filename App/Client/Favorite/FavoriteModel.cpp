// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "FavoriteModel.h"

static Q_LOGGING_CATEGORY(log, "App.Favorite.Model")
CFavoriteModel::CFavoriteModel(CFavoriteDatabase *pDatabase, QObject *parent)
    : QAbstractItemModel(parent)
    , m_pDatabase(pDatabase)
    , m_pRoot(nullptr)
{
    // 初始化根节点
    m_pRoot = new tree();
    if(m_pRoot) {
        m_pRoot->item.type = TreeItem::TYPE::Node;
        m_pRoot->item.id = 0;
        m_pRoot->item.szName = "Root";
        m_Folders[0] = m_pRoot;
    }
}

CFavoriteModel::~CFavoriteModel()
{
    if(m_pRoot) {
        ClearTree(m_pRoot);
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
    return createIndex(item->GetRow(), index.column(), parentItem);
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
        tree* parentItem = static_cast<tree*>(parent.internalPointer());
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
        pTree->SetRow(row++);
        pTree->item = n;
        if(n.isFolder())
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
    tree* ip = static_cast<tree*>(index.internalPointer());
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
        if(Qt::EditRole == role) {
            tree* ip = (tree*)index.internalPointer();
            if(!ip) return false;
            if(ip->item.isFavorite())
                m_pDatabase->UpdateFavorite(ip->item.id, value.toString());
            else
                m_pDatabase->RenameNode(ip->item.id, value.toString());
            ip->item.szName = value.toString();
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
    tree* pItem = GetTree(parent);
    if(!pItem || row < 0 || row + count > pItem->ChildCount())
        return false;

    for(int i = row; i < row + count; i++) {
        auto t = pItem->children[i];
        if(t->item.isFolder()) {
            m_pDatabase->DeleteNode(t->item.id, true);
            m_Folders.remove(t->item.id);
        } else {
            m_pDatabase->Delete(t->item.id, true);
        }
        ClearTree(t);
        delete t;
    }

    beginRemoveRows(parent, row, row + count - 1);
    pItem->children.remove(row, count);
    endRemoveRows();
    return true;
}

bool CFavoriteModel::AddFavorite(
    const QString &szFile, const QString& szName, const QIcon &icon,
    const QString szDescription, int parentId)
{
    if(!m_pDatabase || !m_Folders.contains(parentId)) return false;
    int id = 0;
    CFavoriteDatabase::Item item;
    item.type = TreeItem::Leaf;
    // Check if it already exists
    auto items = m_pDatabase->GetFavorite(szFile);
    if(items.isEmpty()) {
        // Add
        item.id = id;
        item.parentId = parentId;
        item.szName = szName;
        item.szFile = szFile;
        item.icon = icon;
        item.szDescription = szDescription;

        AddTree(item, parentId);
        id = m_pDatabase->AddFavorite(szFile, szName, icon, szDescription, parentId);
        if(id <= 0 ) return false;
    } else {
        // Update
        auto it = items.at(0);
        if(it.id <= 0) return false;
        bool ok = MoveTree(it.id, parentId);
        if(!ok) return false;
        m_pDatabase->Move(it.id, parentId);
    }

    return true;
}

bool CFavoriteModel::UpdateFavorite(
    const QString &szFile, const QString &szName,
    const QString &szDescription, const QIcon &icon)
{
    if(!m_pDatabase) return false;
    m_pDatabase->UpdateFavorite(szFile, szName, icon, szDescription);
    UpdateTree(szFile);
    return true;
}

CFavoriteDatabase::Item CFavoriteModel::GetFavorite(const QString &szFile)
{
    CFavoriteDatabase::Item item;
    if(!m_pDatabase) return item;
    auto items = m_pDatabase->GetFavorite(szFile);
    if(!items.isEmpty()) {
        item = items.at(0);
    }
    return item;
}

void CFavoriteModel::Refresh()
{
    beginResetModel();
    ClearTree(m_pRoot);
    m_Folders.clear();
    m_Folders[0] = m_pRoot;
    endResetModel();
}

bool CFavoriteModel::AddNode(const QString& szName, int parentId)
{
    if(!m_pDatabase || !m_Folders.contains(parentId)) return false;

    int id = m_pDatabase->AddNode(szName, parentId);

    auto item = m_pDatabase->GetGroup(id);
    auto parent = GetTree(parentId);
    if(!parent) return false;

    auto t = new tree();
    if(!t) return false;

    QModelIndex index = CreateIndex(parent);
    t->item = item;
    int pos = parent->GetInserIndex(t);
    beginInsertRows(index, pos, pos);
    parent->InsertChild(pos, t);
    m_Folders[id] = t;
    endInsertRows();
    emit dataChanged(index, index);

    return true;
}

void CFavoriteModel::ClearTree(tree* node)
{
    if(!node) return;
    foreach(auto child, node->children) {
        ClearTree(child);
    }
    node->children.clear();
}

CFavoriteModel::tree* CFavoriteModel::GetTree(int id) const
{
    if(0 == id) return m_pRoot;
    if(m_Folders.contains(id))
        return m_Folders[id];
    return nullptr;
}

CFavoriteModel::tree* CFavoriteModel::GetTree(QModelIndex index) const
{
    tree* ip = nullptr;
    if(index.isValid())
        ip = static_cast<tree*>(index.internalPointer());
    else
        ip = m_pRoot;
    return ip;
}

QModelIndex CFavoriteModel::CreateIndex(tree* t) const
{
    if(!t)
        return QModelIndex();

    // 如果是根节点，返回QModelIndex()
    if(t == m_pRoot)
        return QModelIndex();

    // 确保父节点存在且行号有效
    if(!t->parent)
        return QModelIndex();

    return createIndex(t->GetRow(), 0, t);
}

bool CFavoriteModel::AddTree(const CFavoriteDatabase::Item &item, int parentId)
{
    if (item.id == 0) return false;

    auto parent = GetTree(parentId);
    if(!parent) {
        qCritical(log) << QString("m_Folders[%1] is nullptr").arg(parentId);
        return false;
    }

    auto t = new tree();
    if (!t) {
        qCritical(log) << "Failed to allocate memory for tree node";
        return false;
    }

    t->item = item;
    t->parent = parent;

    int pos = parent->GetInserIndex(t);

    QModelIndex parentIndex = CreateIndex(parent);

    beginInsertRows(parentIndex, pos, pos);

    parent->InsertChild(pos, t);

    if (item.isFolder()) {
        m_Folders[item.id] = t;
    }

    endInsertRows();

    emit dataChanged(parentIndex, parentIndex);

    return true;
}

bool CFavoriteModel::UpdateTree(const QString &szFile)
{
    auto items = m_pDatabase->GetFavorite(szFile);
    foreach(auto it, items) {
        if(it.id <= 0) continue;
        auto leaf = m_pDatabase->GetLeaf(it.id);
        if(leaf.GetId() == 0)
            continue;
        int parentId = leaf.GetParentId();
        tree* parent = GetTree(parentId);
        if(!parent) continue;
        auto c = parent->FindChild(it.id);
        if(!c) continue;
        c->item = it;
        QModelIndex changedIndex = CreateIndex(parent);
        if (changedIndex.isValid()) {
            emit dataChanged(changedIndex, changedIndex);
        }
    }
    return true;
}

bool CFavoriteModel::MoveTree(int id, int newParentId)
{
    auto leaf = m_pDatabase->GetLeaf(id);
    if(leaf.GetId() == 0)
        return false;

    int parentId = leaf.GetParentId();
    if(parentId == newParentId)
        return true;

    tree* parent = GetTree(parentId);
    if(!parent) return false;

    tree* newParent = GetTree(newParentId);
    if(!newParent) return false;

    tree* cur = parent->FindChild(id);
    int sourcePos = parent->children.indexOf(cur);
    if(sourcePos < 0)
        return false;
    int desPos = parent->GetInserIndex(cur);
    QModelIndex index = CreateIndex(parent);
    QModelIndex newIndex = CreateIndex(newParent);
    beginMoveRows(index, sourcePos, sourcePos, newIndex, desPos);
    parent->RemoveChild(cur);
    newParent->AddChild(cur);
    endMoveRows();
    return true;
}

CFavoriteModel::tree::~tree()
{
    qDeleteAll(children);
}

int CFavoriteModel::tree::GetRow() const
{
    return m_row;
}

void CFavoriteModel::tree::SetRow(int r)
{
    m_row = r;
}

bool CFavoriteModel::tree::IsFolder() const
{
    return item.isFolder();
}

bool CFavoriteModel::tree::IsFavorite() const
{
    return item.isFavorite();
}

int CFavoriteModel::tree::ChildCount() const
{
    return children.size();
}

CFavoriteModel::tree* CFavoriteModel::tree::ChildAt(int index) const
{
    if (index < 0 || index >= children.size()) {
        return nullptr;
    }
    return children[index];
}

int CFavoriteModel::tree::GetInserIndex(tree *child)
{
    int index = 0;
    if(child->IsFavorite()) {
        index = children.size();
    } else {
        foreach (const auto &c, children) {
            if (c->item.isFolder()) {
                index++;
            }
        }
    }
    return index;
}

bool CFavoriteModel::tree::AddChild(tree *child)
{
    if (!child) return false;
    child->parent = this;
    return InsertChild(GetInserIndex(child), child);
}

bool CFavoriteModel::tree::InsertChild(int index, tree *child)
{
    if(0 > index || ChildCount() < index)
        return false;
    children.insert(index, child);
    child->parent = this;
    // 更新剩余子节点的行号
    for (int i = index; i < ChildCount(); ++i) {
        children[i]->SetRow(i);
    }
    return true;
}

bool CFavoriteModel::tree::RemoveChild(tree *child)
{
    if (!child) return false;

    int index = children.indexOf(child);
    if (index == -1) return false;

    children.remove(index);

    // 更新剩余子节点的行号
    for (int i = index; i < children.size(); ++i) {
        children[i]->SetRow(i);
    }

    return true;
}

CFavoriteModel::tree* CFavoriteModel::tree::FindChild(int id) const
{
    for (auto child : children) {
        if (child->item.id == id) {
            return child;
        }
    }
    return nullptr;
}

CFavoriteModel::tree* CFavoriteModel::tree::FindRecursive(int id) const
{
    if (item.id == id) {
        return const_cast<tree*>(this);
    }

    for (auto child : children) {
        tree* found = child->FindRecursive(id);
        if (found) {
            return found;
        }
    }

    return nullptr;
}
