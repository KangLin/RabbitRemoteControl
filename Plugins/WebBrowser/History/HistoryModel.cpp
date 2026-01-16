// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QLocale>
#include "HistoryModel.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.History.Model")
CHistoryModel::CHistoryModel(CHistoryDatabase *pDatabase, CParameterWebBrowser *pPara, QObject *parent)
    : QAbstractTableModel(parent)
    , m_pDatabase(pDatabase)
    , m_pPara(pPara)
{
    qDebug(log) << Q_FUNC_INFO;
    // Call refresh() by user
    // if (m_pDatabase && m_pDatabase->isOpen()) {
    //     refresh();
    // }
}

CHistoryModel::~CHistoryModel()
{
    qDebug(log) << Q_FUNC_INFO;
}

QVariant CHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ColumnTitle: return tr("Title");
        case ColumnUrl: return tr("Url");
        case ColumnVisitTime: return tr("Visit Time");
        case ColumnVisitCount: return tr("Visit Count");
        case ColumnLastVisitTime: return tr("Last Visit Time");
        }
    }

    return QVariant();
}

int CHistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_historyItems.count();
}

int CHistoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant CHistoryModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid() || index.row() >= m_historyItems.count())
        return QVariant();

    if(!m_pDatabase) {
        qCritical(log) << "The m_pDatabase is nullptr";
        return QVariant();
    }

    const HistoryItem &item = m_historyItems.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case ColumnTitle:
            return item.title.isEmpty() ? tr("Untitled") : item.title;
        case ColumnUrl:
            return item.url;
        case ColumnVisitTime:
            return item.visitTime.toString(QLocale::system().dateFormat());
        case ColumnVisitCount:
            return item.visitCount;
        case ColumnLastVisitTime:
            return item.lastVisitTime.toString(QLocale::system().dateFormat());
        }
        break;

    case Qt::DecorationRole:
        if (index.column() == ColumnTitle) {
            return item.icon;
        }
        break;

    case Qt::ToolTipRole:
        return QString(tr("Title: %1\nUrl: %2\nVisit Time: %3")
            .arg(item.title)
            .arg(item.url)
            .arg(item.visitTime.toString(QLocale::system().dateFormat())));

    case Qt::UserRole:
        return item.url; // 返回URL用于导航
    }

    return QVariant();
}

void CHistoryModel::refresh()
{
    if(!m_pDatabase) {
        qCritical(log) << "The m_pDatabase is nullptr";
        return;
    }
    beginResetModel();
    int nLimit = 100;
    if(m_pPara)
        nLimit = m_pPara->GetDatabaseViewLimit();
    m_historyItems = m_pDatabase->getAllHistory(nLimit); // 最多500条
    endResetModel();
}

void CHistoryModel::refresh(const QDate &start, const QDate &end)
{
    if(!m_pDatabase) {
        qCritical(log) << "The m_pDatabase is nullptr";
        return;
    }
    beginResetModel();
    int nLimit = 100;
    if(m_pPara)
        nLimit = m_pPara->GetDatabaseViewLimit();
    m_historyItems = m_pDatabase->getHistoryByDate(start, end, nLimit); // 最多500条
    endResetModel();
}

bool CHistoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > m_historyItems.count())
        return false;

    if(!m_pDatabase) {
        qCritical(log) << "The m_pDatabase is nullptr";
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);

    for (int i = row + count - 1; i >= row; --i) {
        m_pDatabase->deleteHistoryEntry(m_historyItems.at(i).id);
        m_historyItems.removeAt(i);
    }

    endRemoveRows();
    return true;
}

HistoryItem CHistoryModel::getItem(const QModelIndex &index) const
{
    if (index.isValid() && index.row() < m_historyItems.count()) {
        return m_historyItems.at(index.row());
    }
    return HistoryItem();
}

bool CHistoryModel::removeDomainItems(const QString &szDomain)
{
    if(m_pDatabase) {
        m_pDatabase->deleteDomainEntries(szDomain);
        refresh();
        return true;
    }
    return false;
}

bool CHistoryModel::removeItems(const QString &url)
{
    if(m_pDatabase) {
        m_pDatabase->deleteHistoryEntry(url);
        refresh();
        return true;
    }
    return false;
}

bool CHistoryModel::importFromCSV(const QString &filename)
{
    if(!m_pDatabase) return false;
    return m_pDatabase->importFromCSV(filename);
}

bool CHistoryModel::exportToCSV(const QString &filename)
{
    if(!m_pDatabase) return false;
    return m_pDatabase->exportToCSV(filename);
}
