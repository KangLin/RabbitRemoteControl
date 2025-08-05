// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once
#include <QAbstractItemModel>
#include <QDateTime>
#include "ChannelSFTP.h"

class CRemoteFileSystem : public QObject
{
    Q_OBJECT
public:
    enum class TYPE {
        NO = 0x00,
        FILE = 0x01,
        DRIVE = 0x02,
        DIR = 0x04,
        SYMLINK = 0x08,
        SPECIAL = 0x10,
        DIRS = DRIVE | DIR | SYMLINK | SPECIAL,
        ALL = DRIVE | DIR | FILE | SYMLINK | SPECIAL
    };
    Q_ENUM(TYPE)
    Q_DECLARE_FLAGS(TYPES, TYPE)
    Q_FLAG(TYPES)

    explicit CRemoteFileSystem(const QString& szPath, TYPES type);
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

    [[nodiscard]] int ChildCount();
    [[nodiscard]] int ColumnCount();

    void SetParent(CRemoteFileSystem* pParent);
    /*!
     * \brief Append child
     * \note Must set all the properties before call them.
     */
    int AppendChild(CRemoteFileSystem* pChild);
    int RemoveChild(int index);
    [[nodiscard]] CRemoteFileSystem* GetChild(int nIndex);
    [[nodiscard]] CRemoteFileSystem* GetParent();
    [[nodiscard]] int IndexOf(CRemoteFileSystem* pChild);
    [[nodiscard]] int IndexOf(const QString& szPath);
    [[nodiscard]] int IndexOfParent();

    enum class State{
        No,
        Getting,
        Ok
    };
    Q_ENUM(State)
    [[nodiscard]] const State GetState() const;
    void SetState(State s);

    [[nodiscard]] QString GetPath();
    [[nodiscard]] QString GetName();

    [[nodiscard]] qint64 GetSize();
    void SetSize(qint64 size);

    [[nodiscard]] TYPES GetType();
    [[nodiscard]] bool IsDir();
    [[nodiscard]] QIcon Icon();

    [[nodiscard]] QDateTime GetCreateTime();
    void SetCreateTime(const QDateTime &date);
    [[nodiscard]] QDateTime GetLastModified();
    void SetLastModified(const QDateTime& date);

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

private:
    CRemoteFileSystem* m_pParent;
    QVector<CRemoteFileSystem*> m_vChild;
    QString m_szPath;
    qint64 m_nSize;
    TYPES m_Type;
    QDateTime m_createTime;
    QDateTime m_lastModifed;
    Permissions m_Permissions;
    QString m_szOwner;
    State m_State;
};

class CRemoteFileSystemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit CRemoteFileSystemModel(
        QObject *parent = nullptr,
        CRemoteFileSystem::TYPES filter = CRemoteFileSystem::TYPE::ALL
        );
    virtual ~CRemoteFileSystemModel();

    QModelIndex SetRootPath(const QString &szPath);
    [[nodiscard]] CRemoteFileSystem* GetRoot();
    [[nodiscard]] CRemoteFileSystem* GetRemoteFileSystemFromIndex(const QModelIndex &index) const;
    [[nodiscard]] CRemoteFileSystem::TYPES GetFilter();

    void CreateDir(QModelIndex index, const QString& dir);
    void RemoveDir(QModelIndex index);

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    [[nodiscard]] QModelIndex index(CRemoteFileSystem *node, int column = 0) const;
    [[nodiscard]] QModelIndex index(const QString& szPath) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual void fetchMore(const QModelIndex &parent) override;
    virtual bool canFetchMore(const QModelIndex &parent) const override;

Q_SIGNALS:
    void sigGetDir(CRemoteFileSystem* p);
    void sigMakeDir(const QString& dir);
    void sigRemoveDir(const QString& szPath);
    void sigRemoveFile(const QString& szFile);
    void sigRename(const QString& oldName, const QString& newName);
public Q_SLOTS:
    void slotGetDir(CRemoteFileSystem* p,
                       QVector<QSharedPointer<CRemoteFileSystem> > contents,
                       bool bEnd);

private:
    CRemoteFileSystem* m_pRoot;
    CRemoteFileSystem::TYPES m_Filter;
    QList<CRemoteFileSystem*> m_GetFolder;

private:
    void DeleteRemoteFileSystem(CRemoteFileSystem* p);
};
