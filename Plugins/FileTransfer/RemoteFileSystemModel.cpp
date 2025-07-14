// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>


#include <QStyle>
#include <QApplication>
#include <QIcon>
#include <QFileInfo>
#include <QLoggingCategory>

#if defined(Q_OS_LINUX)
#include <sys/stat.h>
#endif

#include "RemoteFileSystemModel.h"

static Q_LOGGING_CATEGORY(log, "RemoteFileSystem.Model")

CRemoteFileSystem::CRemoteFileSystem(
    const QString& szPath, TYPE type)
    : QObject()
    , m_pParent(nullptr)
    , m_szPath(szPath)
    , m_nSize(0)
    , m_Type(type)
    , m_Permissions(Permission::No)
    , m_State(State::No)
    , m_DirCount(0)
    , m_FileCount(0)
{
}

CRemoteFileSystem::~CRemoteFileSystem()
{
    qDebug(log) << Q_FUNC_INFO << m_szPath;
}

#if defined(Q_OS_LINUX)
void uint32_to_permstr(uint32_t mode, char *str) {
    str[0] = S_ISDIR(mode) ? 'd' :
                 S_ISLNK(mode) ? 'l' :
                 S_ISCHR(mode) ? 'c' :
                 S_ISBLK(mode) ? 'b' :
                 S_ISFIFO(mode) ? 'p' :
                 S_ISSOCK(mode) ? 's' : '-';
    
    // User
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? ((mode & S_ISUID) ? 's' : 'x') : ((mode & S_ISUID) ? 'S' : '-');
    // Group
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? ((mode & S_ISGID) ? 's' : 'x') : ((mode & S_ISGID) ? 'S' : '-');
    // Others
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? ((mode & S_ISVTX) ? 't' : 'x') : ((mode & S_ISVTX) ? 'T' : '-');
    str[10] = '\0';
}
#endif

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
        if(GetType() & TYPE::SYMLINK)
            return tr("Symlink");
        if(GetType() & TYPE::SPECIAL)
            return tr("Special");
        break;
    }
    case ColumnValue::LastModified: {
        return GetLastModified();
    }
    case ColumnValue::Permission: {
#if defined(Q_OS_LINUX)
        quint32 premissions = (quint32)GetPermissions();
        char buf[11];
        uint32_to_permstr(premissions, buf);
        return QString(buf);
#endif
        break;
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
    case ColumnValue::Permission:
        return tr("Permissions");
    case ColumnValue::Owner:
        return tr("Owner/Group");
    default:
        break;
    }
    return QString();
}

int CRemoteFileSystem::ColumnCount()
{
#if defined(Q_OS_WIN)
    return (int)ColumnValue::Permission;
#else
    return (int)ColumnValue::End;
#endif
}

int CRemoteFileSystem::ChildCount(TYPES filter)
{
    int nCount = 0;
    if(filter & TYPE::FILE)
        nCount += m_FileCount;
    if(filter & TYPE::DIR)
        nCount += m_DirCount;
    return nCount;
}

void CRemoteFileSystem::SetParent(CRemoteFileSystem *pParent)
{
    m_pParent = pParent;
}

CRemoteFileSystem* CRemoteFileSystem::GetParent()
{
    return m_pParent;
}

int CRemoteFileSystem::AppendChild(CRemoteFileSystem *pChild)
{
    Q_ASSERT_X(pChild->GetType(), "AppendChild", "Must set all the properties before call them");

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
        m_DirCount++;
    if(pChild->GetType() & TYPE::SYMLINK)
        m_FileCount++;
    if(pChild->GetType() & TYPE::SPECIAL)
        m_FileCount++;
    Q_ASSERT(m_vChild.size() == m_FileCount + m_DirCount);
    pChild->SetParent(this);
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
    QString szName;
    QString szPath = GetPath();
    if(szPath == '/')
        return szPath;
    int nIndex = szPath.lastIndexOf('/');
    if(-1 == nIndex)
        return QString();
    szName = szPath.right(szPath.size() - nIndex - 1);
    if(GetState() == State::Getting)
        szName += "(" + tr("getting") + " ......)";
    return szName;
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

CRemoteFileSystem::Permissions CRemoteFileSystem::GetPermissions()
{
    return m_Permissions;
}

void CRemoteFileSystem::SetPermissions(Permissions privileges)
{
    m_Permissions = privileges;
}

QString CRemoteFileSystem::GetOwner()
{
    return m_szOwner;
}

void CRemoteFileSystem::SetOwner(QString szOwner)
{
    m_szOwner = szOwner;
}

void CRemoteFileSystem::SetState(State a)
{
    m_State = a;
}

const CRemoteFileSystem::State CRemoteFileSystem::GetState() const
{
    return m_State;
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

CRemoteFileSystem* CRemoteFileSystemModel::GetRoot()
{
    return m_pRoot;
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
    if(Qt::ToolTipRole == role)
        return pItem->GetPath();
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
    int r = 0;
    qDebug(log) << Q_FUNC_INFO << szPath;
    QModelIndex idxParent;
    QModelIndex idx = index(0, 0);
    while(idx.isValid()) {
        idx = index(r++, 0, idxParent);
        CRemoteFileSystem* pRemoteFileSystem = GetRemoteFileSystem(idx);
        QString szDir = pRemoteFileSystem->GetPath();
        qDebug(log) << szDir << szPath;
        if(szDir == szPath)
            return idx;
        if(pRemoteFileSystem->GetType() & CRemoteFileSystem::TYPE::FILE) {
            qDebug(log) << szDir << "Is file:";
            continue;
        }
        if(szPath.left(szDir.size()) == szDir) {
            qDebug(log) << "Contain:" << szPath << szDir;
            idxParent = idx;
            r = 0;
            continue;
        }
    }
    return QModelIndex();
}
