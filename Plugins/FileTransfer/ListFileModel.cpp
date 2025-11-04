// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QAtomicInt>
#include <QLoggingCategory>
#include <QLocale>
#include "ListFileModel.h"
#include "Stats.h"

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
    , m_Direction(dir)
    , m_State(State::No)
    , m_Priority(Priority::Normal)
    , m_nLastSize(0)
    , m_fSpeed(0)
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
            szData = CStats::Convertbytes(GetTransferSize()) + "/" + CStats::Convertbytes(GetFileSize());
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
        case ColumnValue::Speed:
            szData = CStats::Convertbytes(GetSpeed());
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
        return "→";
        return tr("Upload");
    case Direction::Download:
        return "←";
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

quint64 CFileTransfer::GetFileSize()
{
    return m_nFileSize;
}

void CFileTransfer::SetFileSize(quint64 size)
{
    m_nFileSize = size;
}

quint64 CFileTransfer::GetTransferSize()
{
    return m_nTransferSize;
}

void CFileTransfer::slotTransferSize(quint64 nAddSize)
{
    m_nTransferSize += nAddSize;
    QDateTime tmCur = QDateTime::currentDateTime();
    int nSec = m_tmLast.secsTo(tmCur);
    if(nSec < 1) return;
    m_fSpeed = ((float_t)(m_nTransferSize - m_nLastSize)) / (float_t)nSec;
    m_tmLast = tmCur;
    m_nLastSize = m_nTransferSize;
}

float_t CFileTransfer::GetSpeed()
{
    return m_fSpeed;
}

QString CFileTransfer::GetFinishTime()
{
    return m_FinishTime.toString(QLocale::system().dateTimeFormat(QLocale::ShortFormat));
}

void CFileTransfer::slotFinish()
{
    m_FinishTime = QDateTime::currentDateTime();
}

void CFileTransfer::SetFinishTime(QString szTime) {
    if(szTime.isEmpty()) return;
    m_FinishTime = QDateTime::fromString(szTime, QLocale::system().dateTimeFormat(QLocale::ShortFormat));
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
        return tr("Opening");
    case State::Transferring:
        return tr("Transferring");
    case State::Closing:
        return tr("Closing");
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
    if(State::Opening == s)
        m_tmLast = QDateTime::currentDateTime();
    m_State = s;
}

QString CFileTransfer::GetPriority()
{
    switch((Priority)m_Priority) {
    case Priority::Height:
        return tr("High");
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_lstFile.remove(row, count);
#else
    for(int i = row + count - 1; i >= row && i < m_lstFile.size();) {
        m_lstFile.removeAt(i--);
    }
#endif
    endRemoveRows();
    return true;
}

void CListFileModel::UpdateFileTransfer(QSharedPointer<CFileTransfer> f)
{
    if(!f) return;
    int r = m_lstFile.indexOf(f);
    if(-1 == r)
        return;
    //qDebug(log) << "Find:" << r;
    QModelIndex topLeft = index(r, 0);
    QModelIndex bootRight = index(r, (int)CFileTransfer::ColumnValue::End - 1);
    emit dataChanged(topLeft, bootRight);
}

int CListFileModel::Load(QSettings &set)
{
    int nCount = 0;
    beginResetModel();
    nCount = set.value("Model/ListFile/Count").toInt();
    for(int i = 0; i < nCount; i++) {
        set.beginGroup("Model/ListFile/FileTransfer" + QString::number(i));
        QString szLocal = set.value("Local/File").toString();
        CFileTransfer::Direction direction =
            (CFileTransfer::Direction)set.value("Direction", (int)CFileTransfer::Direction::Upload).toInt();
        QString szRemoteFile = set.value("Remote/File").toString();
        QSharedPointer<CFileTransfer> f(
            new CFileTransfer(szLocal, szRemoteFile, direction));
        f->SetRemotePermission(set.value("Remote/Permission", f->GetRemotePermission()).toULongLong());
        f->SetFileSize(set.value("File/Size", f->GetFileSize()).toULongLong());
        f->slotTransferSize(set.value("File/Size/Transfer", f->GetTransferSize()).toULongLong());
        CFileTransfer::State state = (CFileTransfer::State)set.value("State", (int)f->GetState()).toInt();
        if(!(CFileTransfer::State::Finish == state || CFileTransfer::State::Fail == state))
            state = CFileTransfer::State::No;
        f->slotSetstate(state);
        //f->slotSetPrority((CFileTransfer::Priority)set.value("Prioity", f->GetPriority()).toInt());
        f->slotSetExplanation(set.value("Explanation", f->GetExplanation()).toString());
        f->SetFinishTime(set.value("FinishTime", f->GetFinishTime()).toString());
        set.endGroup();
        m_lstFile.append(f);
    }
    endResetModel();
    return 0;
}

int CListFileModel::Save(QSettings &set)
{
    set.setValue("Model/ListFile/Count", m_lstFile.size());
    for(int i = 0; i < m_lstFile.size(); i++) {
        auto f = m_lstFile.at(i);
        set.beginGroup("Model/ListFile/FileTransfer" + QString::number(i));
        set.setValue("Local/File", f->GetLocalFile());
        set.setValue("Direction", (int)f->GetDirection());
        set.setValue("Remote/File", f->GetRemoteFile());
        set.setValue("Remote/Permission", f->GetRemotePermission());
        set.setValue("File/Size", f->GetFileSize());
        set.setValue("File/Size/Transfer", f->GetTransferSize());
        CFileTransfer::State state = f->GetState();
        if(!(CFileTransfer::State::Finish == state || CFileTransfer::State::Fail == state))
            state = CFileTransfer::State::No;
        set.setValue("State", (int)state);
        //set.setValue("Prioity", f->GetPriority());
        set.setValue("Explanation", f->GetExplanation());
        set.setValue("FinishTime", f->GetFinishTime());
        set.endGroup();
    }
    return 0;
}
