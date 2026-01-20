// Author: Kang Lin <kl222@126.com>

#include <QFileInfo>
#include <QLoggingCategory>
#include "RecentModel.h"
#include "ParameterApp.h"

static Q_LOGGING_CATEGORY(log, "App.Recent.Model")
CRecentModel::CRecentModel(CParameterApp *pPara, CRecentDatabase *pDb, QObject *parent)
    : QAbstractTableModel(parent)
    , m_pDatabase(pDb)
{
}

Qt::ItemFlags CRecentModel::flags(const QModelIndex &index) const
{
    auto f = QAbstractTableModel::flags(index);
    if (!index.isValid())
        return f;

    if(index.row() >= m_Items.count())
        return f;

    auto item = m_Items.at(index.row());
    QFileInfo fi(item.szFile);
    if(!item.szFile.isEmpty() && fi.exists())
        f |= Qt::ItemIsEnabled;
    else
        f &= ~Qt::ItemIsEnabled;
    return f;
}

QVariant CRecentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name: return tr("Name");
        case Protocol: return tr("Protocol");
        case Type: return tr("Type");
        case Time: return tr("Time");
        case ID: return tr("ID");
        case File: return tr("File");
        }
    }

    return QVariant();
}

int CRecentModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_Items.count();
}

int CRecentModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

QVariant CRecentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() >= m_Items.count())
        return QVariant();

    auto item = m_Items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case Name:
            return item.szName;
        case Protocol:
            return item.szProtocol;
        case Type:
            return item.szType;
        case Time:
            return item.time.toString(QLocale::system().dateTimeFormat());
        case ID:
            return item.szOperateId;
        case File:
            return item.szFile;
        }
        break;

    case Qt::DecorationRole:
        if (index.column() == Name) {
            return item.icon;
        }
        break;

    case Qt::ToolTipRole:
        return item.szDescription;
    }

    return QVariant();
}

bool CRecentModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > m_Items.count())
        return false;

    if(!m_pDatabase) {
        qCritical(log) << "The m_pDatabase is nullptr";
        return false;
    }

    for (int i = row + count - 1; i >= row; i--) {
        m_pDatabase->deleteRecent(m_Items.at(i).id);
        m_Items.removeAt(i);
    }

    return true;
}

void CRecentModel::refresh(int limit)
{
    if(!m_pDatabase) {
        m_pDatabase = new CRecentDatabase(this);
        if(!m_pDatabase->openDatabase())
            return;
    }
    if(!m_pDatabase->isOpen())
        return;

    beginResetModel();
    m_Items.clear();
    auto items = m_pDatabase->getRecents(limit, 0);
    foreach(auto it, items) {
        m_Items.append(it);
    }
    endResetModel();
}

CRecentDatabase::RecentItem CRecentModel::getItem(const QModelIndex &index)
{
    CRecentDatabase::RecentItem item;
    if (!index.isValid())
        return item;

    if(index.row() >= m_Items.count())
        return item;

    item = m_Items.at(index.row());
    return item;
}

bool CRecentModel::addItem(const CRecentDatabase::RecentItem &item)
{
    if(!m_pDatabase) return false;
    return m_pDatabase->addRecent(item);
}
