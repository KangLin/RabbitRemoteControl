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
    
    qint64 GetFileSize();
    void SetFileSize(qint64 size);

    enum class State{
        No,
        Connecting,
        Transferring,
        Disconnecting,
        Stop,
        Finish,
        Fail
    };
    Q_ENUM(State)
    State GetState();
    QString GetStateName();
    
    enum class Priority{
        Hight,
        Normal,
        Lower
    };
    Q_ENUM(Priority)
    QString GetPriority();

    QString GetExplanation();
    QString GetFinishTime();

Q_SIGNALS:
    void sigStart(CFileTransfer* pFile);
public Q_SLOTS:
    void slotSetstate(State s);
    void slotTransferSize(int nAddSize);
    void slotSetPrority(Priority p);
    void slotSetExplanation(const QString& explanation);
    void slotFinish();

private:
    int m_nId;
    QString m_szLocalFile;
    QString m_szRemoteFile;
    qint64 m_nFileSize;
    qint64 m_nTransferSize;
    qint64 m_nLastSize;
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
