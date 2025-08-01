// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include <QEvent>
#include "Backend.h"
#include "OperateFileTransfer.h"
#include "ChannelSFTP.h"

class CBackendFieTransfer : public CBackend
{
    Q_OBJECT

public:
    CBackendFieTransfer(COperateFileTransfer* pOperate);
    virtual ~CBackendFieTransfer();

    virtual bool event(QEvent *event) override;

protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;

private Q_SLOTS:
    void slotMakeDir(const QString& szDir);
    void slotRemoveDir(const QString& szDir);
    void slotGetDir(CRemoteFileSystem*);
    void slotRemoveFile(const QString& szFile);
Q_SIGNALS:
    void sigGetDir(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> > contents, bool bEnd);

private:
    int SetConnect(COperateFileTransfer *pOperate);
    OnInitReturnValue InitSFTP();

private:
    COperateFileTransfer* m_pOperate;
    CChannelSFTP* m_pSFTP;
};
