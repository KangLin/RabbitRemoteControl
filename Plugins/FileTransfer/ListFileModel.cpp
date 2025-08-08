// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QAtomicInt>
#include <QLoggingCategory>
#include <QLocale>
#include "ListFileModel.h"
#include "ChannelSFTP.h"

static Q_LOGGING_CATEGORY(log, "FileTransfer.ListFileModel")

static QAtomicInt g_Id;

CFileTransfer::CFileTransfer(const QString& localFile,
                             const QString& remoteFile,
                             Direction dir)
    : QObject()
    , m_szLocalFile(localFile)
    , m_LocalPermission(0x0644)
    , m_szRemoteFile(remoteFile)
    , m_RemotePermission(0x0644)
    , m_nFileSize(0)
    , m_nTransferSize(0)
    , m_nLastSize(0)
    , m_Direction(dir)
    , m_State(State::No)
    , m_Priority(Priority::Normal)
    , m_FinishTime(QDateTime::currentDateTime())
{
    m_nId = g_Id++;
}

QString CFileTransfer::HeaderData(int section)
{
    switch((ColumnValue)section) {
    case ColumnValue::LocalFiles:
        return tr("Local files");
    case ColumnValue::Direction:
        return tr("Direction");
    case ColumnValue::RemoteFiles:
        return tr("Remote files");
    case ColumnValue::FileSize:
        return tr("File size");
    case ColumnValue::Speed:
        return tr("Speed");
    case ColumnValue::State:
        return tr("state");
    case ColumnValue::Time:
        return tr("Time");
    case ColumnValue::Explanation:
        return tr("Explanation");
    case ColumnValue::Priority:
        return tr("Priority");
    default:
        break;
    }
    return QString();
}

QVariant CFileTransfer::Data(int column, int role)
{
    if(Qt::TextAlignmentRole == role) {
        switch((ColumnValue)column) {
        case ColumnValue::Direction:
        case ColumnValue::State:
        case ColumnValue::Priority:
            return Qt::AlignHCenter;
        case ColumnValue::FileSize:
        case ColumnValue::Speed:
            return Qt::AlignRight;
        default:
            return Qt::AlignLeft;
        }
    }

    if(Qt::DisplayRole == role) {
        QString szData;
        switch ((ColumnValue)column) {
        case ColumnValue::LocalFiles:
            szData = GetLocalFile();
            break;
        case ColumnValue::RemoteFiles:
            szData = GetRemoteFile();
            break;
        case ColumnValue::FileSize: {
            szData = CChannel::GetSize(m_nTransferSize) + "/" + CChannel::GetSize(GetFileSize());
            break;
        }
        case ColumnValue::Direction:
            szData = GetDirectionName();
            break;
        case ColumnValue::State:
            szData = GetStateName();
            break;
        case ColumnValue::Priority:
            szData = GetPriority();
            break;
        case ColumnValue::Time:
            szData = GetFinishTime();
            break;
        case ColumnValue::Explanation:
            szData = GetExplanation();
            break;
        default:
            break;
        }
        return szData;
    }

    return QVariant();
}

int CFileTransfer::GetId()
{
    return m_nId;
}

QString CFileTransfer::GetDirectionName()
{
    switch ((Direction)GetDirection()) {
    case Direction::Upload:
        return "---->";
        return tr("Upload");
    case Direction::Download:
        return "<----";
        return tr("Download");
    }
    return QString();
}

CFileTransfer::Direction CFileTransfer::GetDirection()
{
    return m_Direction;
}

void CFileTransfer::SetDirection(Direction d)
{
    m_Direction = d;
}

QString CFileTransfer::GetLocalFile()
{
    return m_szLocalFile;
}

void CFileTransfer::SetLocaleFile(const QString &szFile)
{
    m_szLocalFile = szFile;
}

QString CFileTransfer::GetRemoteFile()
{
    return m_szRemoteFile;
}

void CFileTransfer::SetRemoteFile(const QString &szFile)
{
    m_szRemoteFile = szFile;
}

qint64 CFileTransfer::GetFileSize()
{
    return m_nFileSize;
}

void CFileTransfer::SetFileSize(qint64 size)
{
    m_nFileSize = size;
}

void CFileTransfer::slotTransferSize(int nAddSize)
{
    m_nTransferSize += nAddSize;
}

QString CFileTransfer::GetFinishTime()
{
    return m_FinishTime.toString(QLocale::system().dateTimeFormat(QLocale::ShortFormat));
}

void CFileTransfer::slotFinish()
{
    m_FinishTime = QDateTime::currentDateTime();
}

quint32 CFileTransfer::GetLocalPermission() const
{
    return m_LocalPermission;
}

void CFileTransfer::SetLocalPermission(quint32 newLocalPermission)
{
    m_LocalPermission = newLocalPermission;
}

quint32 CFileTransfer::GetRemotePermission() const
{
    return m_RemotePermission;
}

void CFileTransfer::SetRemotePermission(quint32 newRemotePermission)
{
    m_RemotePermission = newRemotePermission;
}

QString CFileTransfer::GetStateName()
{
    switch (GetState()) {
    case State::No:
        return QString();
    case State::Opening:
        return tr("Connecting");
    case State::Transferring:
        return tr("Transferring");
    case State::Closing:
        return tr("Disconnecting");
    case State::Stop:
        return tr("Stop");
    case State::Fail:
        return tr("Fail");
    case State::Finish:
        return tr("Finish");
    }
    return QString();
}
CFileTransfer::State CFileTransfer::GetState()
{
    return m_State;
}

void CFileTransfer::slotSetstate(State s)
{
    m_State = s;
}

QString CFileTransfer::GetPriority()
{
    switch((Priority)m_Priority) {
    case Priority::Height:
        return tr("Height");
    case Priority::Normal:
        return tr("Normal");
    case Priority::Lower:
        return tr("Lower");
    default:
        break;
    }
    return QString();
}

void CFileTransfer::slotSetPrority(Priority p)
{
    m_Priority = p;
}

QString CFileTransfer::GetExplanation()
{
    return m_szExplanation;
}

void CFileTransfer::slotSetExplanation(const QString &explanation)
{
    m_szExplanation = explanation;
}

CListFileModel::CListFileModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant CListFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug(log) << Q_FUNC_INFO << section << orientation << role;
    if(Qt::DisplayRole != role)
        return QVariant();
    if(Qt::Vertical == orientation) {
        if(m_lstFile.size() <= section || 0 > section)
            return QVariant();
        auto p = m_lstFile.at(section);
        if(p)
            return p->GetId();
        return QVariant();
    }
    return CFileTransfer::HeaderData(section);
}

int CListFileModel::rowCount(const QModelIndex &parent) const
{
    //qDebug(log) << Q_FUNC_INFO << parent;
    if (parent.isValid())
        return 0;
    return m_lstFile.size();
}

int CListFileModel::columnCount(const QModelIndex &parent) const
{
    //qDebug(log) << Q_FUNC_INFO << parent;
    if (parent.isValid())
        return 0;

    return (int)CFileTransfer::ColumnValue::End;
}

QVariant CListFileModel::data(const QModelIndex &index, int role) const
{
    //qDebug(log) << Q_FUNC_INFO << index << role;
    if (!index.isValid())
        return QVariant();
    
    int r = index.row();
    if(r > m_lstFile.size() || r < 0)
        return QVariant();
    
    auto p = m_lstFile.at(r);
    if(p)
        return p->Data(index.column(), role);
    
    return QVariant();
}

QModelIndex CListFileModel::AddFileTransfer(QSharedPointer<CFileTransfer> f)
{
    if(!f) return QModelIndex();
    int r = m_lstFile.size();
    beginInsertRows(QModelIndex(), r, r);
    m_lstFile.append(f);
    endInsertRows();
    emit dataChanged(index(r, 0), index(r, (int)CFileTransfer::ColumnValue::End));
    return index(r, 0);
}

int CListFileModel::RemoveFileTransfer(int id)
{
    int nRet = 0;
    for(int i = 0; i < m_lstFile.size(); i++) {
        auto p = m_lstFile.at(i);
        if(p) {
            if(p->GetId() == id) {
                beginRemoveRows(QModelIndex(), i, i);
                m_lstFile.removeAt(i);
                endRemoveRows();
                return 0;
            }
        }
    }
    return nRet;
}

int CListFileModel::RemoveFileTransfer(QList<int> ids)
{
    if(ids.isEmpty()) return 0;
    std::sort(ids.begin(), ids.end());
    beginResetModel();
    auto it = std::remove_if(m_lstFile.begin(), m_lstFile.end(),
                             [&](QSharedPointer<CFileTransfer> f){
                                 return ids.contains(f->GetId());
                             });
    m_lstFile.erase(it, m_lstFile.end());
    endResetModel();
    return 0;
}

QSharedPointer<CFileTransfer> CListFileModel::GetFileTransfer(const QModelIndex &index)
{
    if (!index.isValid())
        return QSharedPointer<CFileTransfer>();
    int r = index.row();
    if(0 > r || m_lstFile.size() <= r)
        return QSharedPointer<CFileTransfer>();
    return m_lstFile.at(r);
}

bool CListFileModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    m_lstFile.remove(row, count);
    endRemoveRows();
    return true;
}

void CListFileModel::UpdateFileTransfer(QSharedPointer<CFileTransfer> f)
{
    int r = m_lstFile.indexOf(f);
    QModelIndex topLeft = index(r, 0);
    QModelIndex bootRight = index(r, (int)CFileTransfer::ColumnValue::End);
    emit dataChanged(topLeft, bootRight);
}
