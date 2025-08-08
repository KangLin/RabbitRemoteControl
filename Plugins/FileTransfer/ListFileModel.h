// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include <QAbstractTableModel>
#include <QDateTime>

class CFileTransfer : public QObject
{
    Q_OBJECT
public:
    enum class Direction{
        Upload,
        Download
    };
    Q_ENUM(Direction)

    CFileTransfer(const QString& localFile, const QString& remoteFile, Direction dir = Direction::Download);
    
    int GetId();

    QString GetDirectionName();
    Direction GetDirection();
    void SetDirection(Direction d);

    enum class ColumnValue {
        LocalFiles = 0,
        Direction,
        RemoteFiles,
        FileSize,
        Speed,
        State,
        Time,
        Priority,
        Explanation,
        End
    };
    Q_ENUM(ColumnValue)
    static QString HeaderData(int section);
    QVariant Data(int column, int role);

    QString GetLocalFile();
    void SetLocaleFile(const QString& szFile);
    QString GetRemoteFile();
    void SetRemoteFile(const QString& szFile);
    
    quint64 GetFileSize();
    void SetFileSize(quint64 size);

    enum class State{
        No = 0x01,
        Opening = 0x02,
        Transferring = 0x04,
        Closing = 0x08,
        Stop = 0x10,
        Finish = 0x20,
        Fail = 0x40,
        Process = Opening | Transferring | Closing,
        CanStart = No | Stop | Fail
    };
    Q_ENUM(State)
    State GetState();
    QString GetStateName();
    
    enum class Priority{
        Height,
        Normal,
        Lower
    };
    Q_ENUM(Priority)
    QString GetPriority();

    QString GetExplanation();
    QString GetFinishTime();
    
    quint32 GetLocalPermission() const;
    void SetLocalPermission(quint32 newLocalPermission);
    quint32 GetRemotePermission() const;
    void SetRemotePermission(quint32 newRemotePermission);
    
Q_SIGNALS:
    void sigStart(CFileTransfer* pFile);
public Q_SLOTS:
    void slotSetstate(State s);
    void slotTransferSize(quint64 nAddSize);
    void slotSetPrority(Priority p);
    void slotSetExplanation(const QString& explanation);
    void slotFinish();

private:
    int m_nId;
    QString m_szLocalFile;
    quint32 m_LocalPermission;
    QString m_szRemoteFile;
    quint32 m_RemotePermission;
    quint64 m_nFileSize;
    quint64 m_nTransferSize;
    quint64 m_nLastSize;
    Direction m_Direction;
    State m_State;
    Priority m_Priority;
    QString m_szExplanation;
    QDateTime m_FinishTime;
};

class CListFileModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CListFileModel(QObject *parent = nullptr);
    
    QModelIndex AddFileTransfer(QSharedPointer<CFileTransfer> f);
    int RemoveFileTransfer(int id);
    int RemoveFileTransfer(QList<int> ids);
    QSharedPointer<CFileTransfer> GetFileTransfer(const QModelIndex& index);
    void UpdateFileTransfer(QSharedPointer<CFileTransfer> f);

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent) override;

private:
    QList<QSharedPointer<CFileTransfer> > m_lstFile;
};
