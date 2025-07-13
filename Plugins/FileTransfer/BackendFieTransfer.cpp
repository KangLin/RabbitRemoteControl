// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "BackendFieTransfer.h"

static Q_LOGGING_CATEGORY(log, "FileTransfer.Backend")
CBackendFieTransfer::CBackendFieTransfer(COperateFileTransfer *pOperate): CBackend(pOperate)
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackendFieTransfer::~CBackendFieTransfer()
{
    qDebug(log) << Q_FUNC_INFO;
}

bool CBackendFieTransfer::event(QEvent *event)
{
    return CBackend::event(event);
}

CBackend::OnInitReturnValue CBackendFieTransfer::OnInit()
{
    return OnInitReturnValue::UseOnProcess;
}

int CBackendFieTransfer::OnClean()
{
    int nRet = 0;

    return nRet;
}

int CBackendFieTransfer::OnProcess()
{
    int nRet = 0;

    return nRet;
}
