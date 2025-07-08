#include <QLoggingCategory>
#include "ListFileModel.h"

static Q_LOGGING_CATEGORY(log, "FileTransfer.ListFileModel")

CListFileModel::CListFileModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_Columns((int)ColumnValue::End)
    , m_Rows(0)
{}

QVariant CListFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    qDebug(log) << Q_FUNC_INFO << section << orientation << role;
    switch((ColumnValue)section) {
    case ColumnValue::LocalFiles: {
        switch(role) {
        case Qt::DisplayRole:
            return tr("Local files");
        }
    }
    case ColumnValue::Direction: {
        switch(role) {
        case Qt::DisplayRole:
            return tr("Direction");
        }
    }
    case ColumnValue::RemoteFiles: {
        switch(role) {
        case Qt::DisplayRole:
            return tr("Remote files");
        }
    }
    case ColumnValue::FileSize: {
        switch(role) {
        case Qt::DisplayRole:
            return tr("File size");
        }
    }
    case ColumnValue::Speed: {
        switch(role) {
        case Qt::DisplayRole:
            return tr("Speed");
        }
    }
    case ColumnValue::State: {
        switch(role) {
        case Qt::DisplayRole:
            return tr("state");
        }
    }
    case ColumnValue::Time: {
        switch(role) {
        case Qt::DisplayRole:
            return tr("Time");
        }
    }
    case ColumnValue::Explanation: {
        switch(role) {
        case Qt::DisplayRole:
            return tr("Explanation");
        }
    }
    case ColumnValue::Priority: {
        switch(role) {
        case Qt::DisplayRole:
            return tr("Priority");
        }
    }
    }
    return QVariant();
}

int CListFileModel::rowCount(const QModelIndex &parent) const
{
    qDebug(log) << Q_FUNC_INFO << parent;
    if (parent.isValid())
        return 0;
    
    return m_Rows;
}

int CListFileModel::columnCount(const QModelIndex &parent) const
{
    qDebug(log) << Q_FUNC_INFO << parent;
    if (parent.isValid())
        return 0;
    
    return m_Columns;
}

QVariant CListFileModel::data(const QModelIndex &index, int role) const
{
    qDebug(log) << Q_FUNC_INFO << index << role;
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
