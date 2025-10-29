// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include <QEvent>
#include "Backend.h"
#include "OperateFileTransfer.h"

#if HAVE_LIBSSH
#include "ChannelSFTP.h"
#endif

class CBackendFileTransfer : public CBackend
{
    Q_OBJECT

public:
    CBackendFileTransfer(COperateFileTransfer* pOperate);
    virtual ~CBackendFileTransfer();

    virtual bool event(QEvent *event) override;

protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;

private Q_SLOTS:
    void slotMakeDir(const QString& szDir);
    void slotRemoveDir(const QString& szDir);
    void slotGetDir(CRemoteFileSystem* pRemoteFileSystem);
    void slotRemoveFile(const QString& szFile);
    void slotRename(const QString& oldName, const QString& newName);
Q_SIGNALS:
    void sigGetDir(CRemoteFileSystem* pRemoteFileSystem,
                   QVector<QSharedPointer<CRemoteFileSystem> > contents,
                   bool bEnd);
private Q_SLOTS:
    void slotStartFileTransfer(QSharedPointer<CFileTransfer> f);
    void slotStopFileTransfer(QSharedPointer<CFileTransfer> f);
Q_SIGNALS:
    void sigFileTransferUpdate(QSharedPointer<CFileTransfer> f);

private:
    int SetConnect(COperateFileTransfer *pOperate);
    OnInitReturnValue InitSFTP();

private:
    COperateFileTransfer* m_pOperate;
#if HAVE_LIBSSH
    CChannelSFTP* m_pSFTP;
#endif
    CParameterFileTransfer* m_pPara;
};
