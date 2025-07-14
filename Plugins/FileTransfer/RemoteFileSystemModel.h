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
        SYMLINK = 0x08,
        SPECIAL = 0x10,
        ALL = DRIVE | DIR | FILE | SYMLINK | SPECIAL
    };
    Q_ENUM(TYPE)
    Q_DECLARE_FLAGS(TYPES, TYPE)
    Q_FLAG(TYPES)

    explicit CRemoteFileSystem(const QString& szPath, TYPE type);
    virtual ~CRemoteFileSystem();

    enum class ColumnValue {
        Name = 0,
        Size,
        Type,
        LastModified,
        Permission,
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

    enum class Permission {
        No = 0x00,
        Exec = 0x01,
        Write = 0x02,
        Read = 0x04
    };
    Q_ENUM(Permission)
    Q_DECLARE_FLAGS(Permissions, Permission)
    Q_FLAG(Permissions)
    void SetPermissions(Permissions privileges);
    [[nodiscard]] Permissions GetPermissions();

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
    
    void SetParent(CRemoteFileSystem* pParent);
    /*!
     * \brief Append child
     * \note Must set all the properties before call them.
     */
    int AppendChild(CRemoteFileSystem* pChild);

private:
    CRemoteFileSystem* m_pParent;
    QVector<CRemoteFileSystem*> m_vChild;
    QString m_szPath;
    qint64 m_nSize;
    TYPES m_Type;
    QDateTime m_lastModifed;
    Permissions m_Permissions;
    QString m_szOwner;
    State m_State;
    int m_DirCount;
    int m_FileCount;
};

class CRemoteFileSystemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit CRemoteFileSystemModel(QObject *parent = nullptr);

    QModelIndex SetRoot(CRemoteFileSystem* root);
    CRemoteFileSystem* GetRoot();
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
    CRemoteFileSystem* m_pRoot; //TODO: delete it!
    CRemoteFileSystem::TYPES m_Filter;
};
