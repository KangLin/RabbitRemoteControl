// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once
#include <QAbstractItemModel>
#include <QDateTime>

class CRemoteFileSystem : public QObject
{
    Q_OBJECT
public:
    enum class TYPE {
        NO = 0x00,
        DRIVE = 0x01,
        DIR = 0x02,
        FILE = 0x04,
        ALL = DRIVE | DIR | FILE
    };
    Q_ENUM(TYPE)
    Q_DECLARE_FLAGS(TYPES, TYPE)
    Q_FLAG(TYPES)
    
    explicit CRemoteFileSystem(const QString& szPath, TYPE type,
                               QObject* parent = nullptr);
    virtual ~CRemoteFileSystem();

    enum class ColumnValue {
        Name = 0,
        Size,
        Type,
        LastModified,
        Privileges,
        Owner,
        End
    };
    Q_ENUM(ColumnValue)
    [[nodiscard]] static QString HeaderData(int section);
    [[nodiscard]] QVariant Data(int column);

    [[nodiscard]] int ChildCount(TYPES filter = TYPE::ALL);
    [[nodiscard]] int ColumnCount();

    int RemoveChild(int index);
    [[nodiscard]] CRemoteFileSystem* GetChild(int nIndex);
    [[nodiscard]] CRemoteFileSystem* GetParent();
    [[nodiscard]] int IndexOf(CRemoteFileSystem* pChild);

    [[nodiscard]] QString GetPath();
    [[nodiscard]] QString GetName();

    [[nodiscard]] qint64 GetSize();
    void SetSize(qint64 size);

    [[nodiscard]] TYPES GetType();
    [[nodiscard]] bool IsDir();
    [[nodiscard]] QIcon Icon();

    [[nodiscard]] QDateTime GetLastModified();
    void SetLastModified(QDateTime date);

    enum class Privileges {
        No = 0x00,
        Read = 0x01,
        Write = 0x02,
        Exec = 0x04
    };
    Q_ENUM(Privileges)
    void SetPrivileges(Privileges privileges);
    [[nodiscard]] Privileges GetPrivileges();

    [[nodiscard]] QString GetOwner();
    void SetOwner(QString szOwner);

    enum class State{
        No,
        Getting,
        Ok
    };
    Q_ENUM(State)
    const State GetState() const;
    void SetState(State s);

private:
    int AppendChild(CRemoteFileSystem* pChild);
    
private:
    QVector<CRemoteFileSystem*> m_vChild;
    QString m_szPath;
    qint64 m_nSize;
    TYPES m_Type;
    QDateTime m_lastModifed;
    Privileges m_Privileges;
    QString m_szOwner;
    State m_State;
    int m_DriveCount;
    int m_DirCount;
    int m_FileCount;
};

class CRemoteFileSystemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit CRemoteFileSystemModel(QObject *parent = nullptr);
    
    QModelIndex SetRoot(CRemoteFileSystem* root);
    CRemoteFileSystem* GetRemoteFileSystem(const QModelIndex &index) const;
    QModelIndex index(const QString& szPath) const;
    void SetFilter(CRemoteFileSystem::TYPES filter);
    CRemoteFileSystem::TYPES GetFilter();

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
private:
    CRemoteFileSystem* m_pRoot;
    CRemoteFileSystem::TYPES m_Filter;
};
