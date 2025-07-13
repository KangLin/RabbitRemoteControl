// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include "Backend.h"
#include "OperateFileTransfer.h"

class CBackendFieTransfer : public CBackend
{
    Q_OBJECT
public:
    CBackendFieTransfer(COperateFileTransfer* pOperate);
    virtual ~CBackendFieTransfer();

    // QObject interface
public:
    virtual bool event(QEvent *event) override;

    // CBackend interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;
};
