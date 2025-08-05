// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include <QMap>
#include <QWidget>
#include <QFileSystemModel>
#include "ListFileModel.h"
#include "RemoteFileSystemModel.h"

namespace Ui {
class CFrmFileTransfer;
}

class CFrmFileTransfer : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmFileTransfer(QWidget *parent = nullptr);
    virtual ~CFrmFileTransfer();

Q_SIGNALS:
    void sigUpload(const QString& source, const QString& destination);
    void sigCopyUrlToClipboard(const QString& szPath);

    void sigGetDir(CRemoteFileSystem*);
    void sigGetDir(CRemoteFileSystem*,
                   QVector<QSharedPointer<CRemoteFileSystem> > contents,
                   bool bEnd);

    void sigMakeDir(const QString &szPath);
    void sigRemoveDir(const QString &szPath);
    void sigRemoveFile(const QString& szFile);
    void sigRename(const QString& oldName, const QString& newName);

private:
    //! Set local root path
    int SetLocalRoot(const QString &root);
    //! Get local root path
    [[nodiscard]] QString GetLocalRoot() const;
    void SetRemoteConnecter(CRemoteFileSystemModel* p);

private Q_SLOTS:
    void on_cbLocal_editTextChanged(const QString &szPath);

    void slotTreeLocalClicked(const QModelIndex &index);
    void on_treeLocal_customContextMenuRequested(const QPoint &pos);
    void slotTreeLocalUpload();
    void slotTreeLocalAddToList();
    void slotTreeLocalOpen();
    void slotTreeLocalNew();
    void slotTreeLocalRename();
    void slotTreeLocalDelete();
    void slotTreeLocalCopyToClipboard();

    void on_tabLocal_customContextMenuRequested(const QPoint &pos);
    void slotTabLocalUpload();
    void slotTabLocalAddToList();
    void slotTabLocalOpen();
    void slotTabLocalEdit();
    void slotTabLocalRename();
    void slotTabLocalDelete();
    void slotTabLocalCopyToClipboard();

    void on_cbRemote_editTextChanged(const QString &szPath);
    void on_cbRemote_currentIndexChanged(int index);

    void on_treeRemote_clicked(const QModelIndex &index);
    void on_treeRemote_doubleClicked(const QModelIndex &index);
    void on_treeRemote_customContextMenuRequested(const QPoint &pos);
    void slotTreeRemoteDownload();
    void slotTreeRemoteAddToList();
    void slotTreeRemoteNew();
    void slotTreeRemoteDelete();
    void slotTreeRemoteRename();
    void slotTreeRemoteRefresh();
    void slotTreeRemoteCopyToClipboard();

    void on_tabRemote_customContextMenuRequested(const QPoint &pos);
    void slotTabRemoteDownload();
    void slotTabRemoteAddToList();
    void slotTabRemoteNew();
    void slotTabRemoteDelete();
    void slotTabRemoteRename();
    void slotTabRemoteCopyToClipboard();
    
    void on_tabList_customContextMenuRequested(const QPoint &pos);
    void slotTabListDelete();

private:
    Ui::CFrmFileTransfer *ui;

    QFileSystemModel* m_pModelLocalDir;
    QFileSystemModel* m_pModelLocalFile;

    CRemoteFileSystemModel* m_pModelRemoteDir;
    CRemoteFileSystemModel* m_pModelRemoteFile;

    CListFileModel* m_pListFileModel;
};
