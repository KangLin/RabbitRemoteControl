// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include "ParameterFtpServer.h"

CParameterFtpServer::CParameterFtpServer(QObject *parent, const QString &szPrefix)
    : CParameterServer{parent, szPrefix}
{
#if defined(Q_OS_UNIX)
    m_Net.SetPort(2121);
#else
    m_Net.SetPort(21);
#endif
    m_Net.m_User.SetSavePassword(true);
    m_Net.SetEnablleUI(CParameterNet::SHOW_UI::Port | CParameterNet::SHOW_UI::User);
}

int CParameterFtpServer::OnLoad(QSettings &set)
{
    int nRet = CParameterServer::OnLoad(set);
    if(nRet) return nRet;
    return 0;
}

int CParameterFtpServer::OnSave(QSettings &set)
{
    int nRet = CParameterServer::OnSave(set);
    if(nRet) return nRet;
    return 0;
}
