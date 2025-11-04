// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include "ParameterFileTransfer.h"

CParameterFileTransfer::CParameterFileTransfer(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_Net(this)
    , m_SSH(this)
    , m_Protocol(Protocol::SFTP)
{
    m_Net.SetPort(21);
}

CParameterFileTransfer::Protocol CParameterFileTransfer::GetProtocol() const
{
    return m_Protocol;
}

void CParameterFileTransfer::SetProtocol(Protocol newType)
{
    if(m_Protocol == newType)
        return;
    m_Protocol = newType;
    SetModified(true);
}

int CParameterFileTransfer::OnLoad(QSettings &set)
{
    int nRet = 0;
    SetProtocol((Protocol)(set.value("Type", (int)GetProtocol()).toInt()));
    return nRet;
}

int CParameterFileTransfer::OnSave(QSettings &set)
{
    int nRet = 0;
    set.setValue("Type", (int)GetProtocol());
    return nRet;
}
