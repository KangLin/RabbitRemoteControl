// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QStyle>
#include <QApplication>
#include <QIcon>
#include <QFileInfo>
#include <QLoggingCategory>
#include "RemoteFileSystemModel.h"

static Q_LOGGING_CATEGORY(log, "RemoteFileSystem.Model")

CRemoteFileSystem::CRemoteFileSystem(
    const QString& szPath, TYPE type, QObject* parent)
    : QObject(parent)
    , m_szPath(szPath)
    , m_nSize(0)
    , m_Type(type)
    , m_Privileges(Privileges::No)
    , m_DriveCount(0)
    , m_DirCount(0)
    , m_FileCount(0)
{
    CRemoteFileSystem* pParent = qobject_cast<CRemoteFileSystem*>(parent);
    if(pParent) {
        pParent->AppendChild(this);
    }
}

CRemoteFileSystem::~CRemoteFileSystem()
{}

QVariant CRemoteFileSystem::Data(int column)
{
    switch((ColumnValue)column) {
    case ColumnValue::Name: {
        return GetName();
    }
    case ColumnValue::Size: {
        qint64 size = GetSize();
        if(1024 >= size)
            return QString("%1 B").arg(size);
        else if(1024* 1024 >= size)
            return QString::number(size / 1024.0, 'f', 2) + QString(" KB");
        else if(1024 * 1024 * 1024 >= size)
            return QString::number(size / (1024.0 * 1024.0), 'f', 2) + QString(" MB");
        else
            return QString::number(size / (1024.0 * 1024.0 * 1024.0), 'f', 2) + QString(" GB");
    }
    case ColumnValue::Type: {
        if(GetType() & TYPE::FILE)
            return tr("File");
        if(GetType() & TYPE::DIR)
            return tr("Folder");
        if(GetType() & TYPE::DRIVE)
            return tr("Driver");
        break;
    }
    case ColumnValue::LastModified: {
        return GetLastModified();
    }
    case ColumnValue::Owner:
        return GetOwner();
    default:
        break;
    }
    return QVariant();
}

QString CRemoteFileSystem::HeaderData(int section)
{
    switch ((ColumnValue)section) {
    case ColumnValue::Name:
        return tr("File name");
    case ColumnValue::Size:
        return tr("File size");
    case ColumnValue::Type:
        return tr("File type");
    case ColumnValue::LastModified:
        return tr("File last modified time");
    case ColumnValue::Privileges:
        return tr("Privileges");
    case ColumnValue::Owner:
        return tr("Owner/Group");
    default:
        break;
    }
    return QString();
}

int CRemoteFileSystem::ColumnCount()
{
    return (int)ColumnValue::End;
}

int CRemoteFileSystem::ChildCount(TYPES filter)
{
    int nCount = 0;
    if(filter & TYPE::FILE)
        nCount += m_FileCount;
    if(filter & TYPE::DIR)
        nCount += m_DirCount;
    if(filter & TYPE::DRIVE)
        nCount += m_DriveCount;
    return nCount;
}

int CRemoteFileSystem::AppendChild(CRemoteFileSystem *pChild)
{

    Q_ASSERT_X(pChild->GetType() != TYPE::NO, "AppendChild", "Need set type etc");

    if(m_vChild.contains(pChild)) {
        qDebug(log) << pChild->GetName() << "is exist";
        return 0;
    }
    m_vChild.append(pChild);
    if(pChild->GetType() & TYPE::FILE)
        m_FileCount++;
    if(pChild->GetType() & TYPE::DIR)
        m_DirCount++;
    if(pChild->GetType() & TYPE::DRIVE)
        m_DriveCount++;
    Q_ASSERT(m_vChild.size() == m_FileCount + m_DirCount + m_DriveCount);
    pChild->setParent(this);
    return 0;
}

int CRemoteFileSystem::RemoveChild(int index)
{
    m_vChild.removeAt(index);
    return 0;
}

CRemoteFileSystem* CRemoteFileSystem::GetChild(int nIndex)
{
    if(nIndex < 0 || nIndex >= m_vChild.size())
        return nullptr;
    return m_vChild.at(nIndex);
}

CRemoteFileSystem* CRemoteFileSystem::GetParent()
{
    return qobject_cast<CRemoteFileSystem*>(parent());
}

int CRemoteFileSystem::IndexOf(CRemoteFileSystem* pChild)
{
    return m_vChild.indexOf(pChild);
}

QString CRemoteFileSystem::GetPath()
{
    return m_szPath;
}

QString CRemoteFileSystem::GetName()
{
    QFileInfo fi(GetPath());
    if(GetPath() == '/')
        return GetPath();
    return fi.baseName();
}

qint64 CRemoteFileSystem::GetSize()
{
    return m_nSize;
}

void CRemoteFileSystem::SetSize(qint64 size)
{
    m_nSize = size;
}

bool CRemoteFileSystem::IsDir()
{
    return !(GetType() & TYPE::FILE);
}

QIcon CRemoteFileSystem::Icon()
{
    if(GetType() & TYPE::DIR)
        return QIcon::fromTheme("folder-remote");
    if(GetType() & TYPE::FILE)
        return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    if(GetType() & TYPE::DRIVE)
        return QIcon::fromTheme("drive-harddisk");
    return QIcon();
}

CRemoteFileSystem::TYPES CRemoteFileSystem::GetType()
{
    return m_Type;
}

QDateTime CRemoteFileSystem::GetLastModified()
{
    return m_lastModifed;
}

void CRemoteFileSystem::SetLastModified(QDateTime date)
{
    m_lastModifed = date;
}

CRemoteFileSystem::Privileges CRemoteFileSystem::GetPrivileges()
{
    return m_Privileges;
}

void CRemoteFileSystem::SetPrivileges(Privileges privileges)
{
    m_Privileges = privileges;
}

QString CRemoteFileSystem::GetOwner()
{
    return m_szOwner;
}

void CRemoteFileSystem::SetOwner(QString szOwner)
{
    m_szOwner = szOwner;
}

CRemoteFileSystemModel::CRemoteFileSystemModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_pRoot(nullptr)
    , m_Filter(CRemoteFileSystem::TYPE::ALL)
{}

QModelIndex CRemoteFileSystemModel::SetRoot(CRemoteFileSystem *root)
{
    beginResetModel();
    QModelIndex index;
    m_pRoot = root;
    if(m_pRoot)
        index = createIndex(0, 0, m_pRoot);
    //qDebug(log) << Q_FUNC_INFO << index;
    endResetModel();
    return index;
}

CRemoteFileSystem* CRemoteFileSystemModel::GetRemoteFileSystem(const QModelIndex &index) const
{
    if(index.isValid())
        return static_cast<CRemoteFileSystem*>(index.internalPointer());
    return nullptr;
}

void CRemoteFileSystemModel::SetFilter(CRemoteFileSystem::TYPES filter)
{
    m_Filter = filter;
}

CRemoteFileSystem::TYPES CRemoteFileSystemModel::GetFilter()
{
    return m_Filter;
}

QVariant CRemoteFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(Qt::Vertical == orientation) {
        if(Qt::DisplayRole == role)
            return QString::number(section + 1);
    } else {
        if(Qt::DisplayRole == role)
            return CRemoteFileSystem::HeaderData(section);
    }
    return QVariant();
}

int CRemoteFileSystemModel::rowCount(const QModelIndex &parent) const
{
    CRemoteFileSystem* pItem = nullptr;
    if (parent.isValid())
        pItem = GetRemoteFileSystem(parent);
    else
        pItem = m_pRoot;
    if(!pItem)
        return 0;
    return pItem->ChildCount(m_Filter);
}

int CRemoteFileSystemModel::columnCount(const QModelIndex &parent) const
{
    CRemoteFileSystem* pItem = nullptr;
    if (parent.isValid())
        pItem = GetRemoteFileSystem(parent);
    else
        pItem = m_pRoot;
    if(pItem)
        return pItem->ColumnCount();
    return 0;
}

QVariant CRemoteFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole && role != Qt::DecorationRole)
        return QVariant();
    //qDebug(log) << Q_FUNC_INFO << index;
    CRemoteFileSystem* pItem = GetRemoteFileSystem(index);
    if(!pItem)
        return QVariant();
    if(!(pItem->GetType() & m_Filter))
        return QVariant();
    if(Qt::DecorationRole == role && index.column() == 0)
        return pItem->Icon();
    if(Qt::DisplayRole == role)
        return pItem->Data(index.column());
    return QVariant();
}

QModelIndex CRemoteFileSystemModel::index(int row, int column, const QModelIndex &parent) const
{
    //qDebug(log) << Q_FUNC_INFO << parent << row << column;
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    CRemoteFileSystem* pItem = nullptr;
    if (parent.isValid())
        pItem = GetRemoteFileSystem(parent);
    else
        pItem = m_pRoot;
    if(!pItem)
        return QModelIndex();
    pItem = pItem->GetChild(row);
    if(pItem)
        return createIndex(row, column, pItem);
    else
        return QModelIndex();
}

QModelIndex CRemoteFileSystemModel::parent(const QModelIndex &child) const
{
    //qDebug(log) << Q_FUNC_INFO << child;
    if (!child.isValid() || !m_pRoot)
        return QModelIndex();
    CRemoteFileSystem* pItem = GetRemoteFileSystem(child);
    if(!pItem)
        return QModelIndex();
    CRemoteFileSystem* pItemParent = pItem->GetParent();
    if(pItemParent)
        return createIndex(pItemParent->IndexOf(pItem), 0, pItemParent);
    return QModelIndex();
}

QModelIndex CRemoteFileSystemModel::index(const QString& szPath) const
{
    return QModelIndex();
}
