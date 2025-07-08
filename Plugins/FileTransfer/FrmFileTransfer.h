// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>
#pragma once

#include <QWidget>
#include <QFileSystemModel>
#include "ListFileModel.h"

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

private:
    //! Set local root path
    int SetLocalRoot(const QString &root);
    //! Get local root path
    [[nodiscard]] QString GetLocalRoot() const;

private Q_SLOTS:
    void on_cbLocal_editTextChanged(const QString &arg1);

    void slotTreeLocalClicked(const QModelIndex &index);
    void on_treeLocal_customContextMenuRequested(const QPoint &pos);
    void slotTreeLocalUpload();
    void slotTreeLocalAddToList();
    void slotTreeLocalOpen();
    void slotTreeLocalNew();
    void slotTreeLocalRename();
    void slotTreeLocalDelete();

    void on_tabLocal_customContextMenuRequested(const QPoint &pos);
    void slotTabLocalUpload();
    void slotTabLocalAddToList();
    void slotTabLocalOpen();
    void slotTabLocalEdit();
    void slotTabLocalRename();
    void slotTabLocalDelete();
    
    void on_cbRemote_editTextChanged(const QString &arg1);
    
    void on_treeRemote_customContextMenuRequested(const QPoint &pos);
    void slotTreeRemoteDownload();
    void slotTreeRemoteAddToList();
    void slotTreeRemoteNew();
    void slotTreeRemoteDelete();
    void slotTreeRemoteRename();
    void slotTreeRemoteCopyToClipboard();

    void on_tabRemote_customContextMenuRequested(const QPoint &pos);
    void slotTabRemoteDownload();
    void slotTabRemoteAddToList();
    void slotTabRemoteNew();
    void slotTabRemoteDelete();
    void slotTabRemoteRename();
    void slotTabRemoteCopyToClipboard();

private:
    Ui::CFrmFileTransfer *ui;
    QFileSystemModel* m_pModelLocalDir;
    QFileSystemModel* m_pModelLocalFile;
    
    CListFileModel* m_pListFileModel;
};
