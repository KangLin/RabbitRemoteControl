// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include "ParameterFtpServer.h"

CParameterFtpServer::CParameterFtpServer(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_nPort(21)
    , m_bAnonymousLogin(true)
    , m_bReadOnly(true)
    , m_ConnectCount(-1)
{}

uint16_t CParameterFtpServer::GetPort() const
{
    return m_nPort;
}

void CParameterFtpServer::SetPort(uint16_t newPort)
{
    m_nPort = newPort;
}

QString CParameterFtpServer::GetUser() const
{
    return m_szUser;
}

void CParameterFtpServer::SetUser(const QString &newUser)
{
    m_szUser = newUser;
}

QString CParameterFtpServer::GetPassword() const
{
    return m_szPassword;
}

void CParameterFtpServer::SetPassword(const QString &newPassword)
{
    m_szPassword = newPassword;
}

bool CParameterFtpServer::GetAnonymousLogin() const
{
    return m_bAnonymousLogin;
}

void CParameterFtpServer::SetAnonymousLogin(bool newAnonymousLogin)
{
    m_bAnonymousLogin = newAnonymousLogin;
}

bool CParameterFtpServer::GetReadOnly() const
{
    return m_bReadOnly;
}

void CParameterFtpServer::SetReadOnly(bool newReadOnly)
{
    m_bReadOnly = newReadOnly;
}

QString CParameterFtpServer::GetRoot() const
{
    return m_szRoot;
}

void CParameterFtpServer::SetRoot(const QString &newRoot)
{
    m_szRoot = newRoot;
}

int CParameterFtpServer::GetConnectCount() const
{
    return m_ConnectCount;
}

void CParameterFtpServer::SetConnectCount(int newConnectCount)
{
    m_ConnectCount = newConnectCount;
}

int CParameterFtpServer::OnLoad(QSettings &set)
{
    SetPort(set.value("Port", GetPort()).toUInt());
    SetUser(set.value("UserName", GetUser()).toString());
    SetPassword(set.value("Password", GetPassword()).toString());
    SetRoot(set.value("Root", GetRoot()).toString());
    SetAnonymousLogin(set.value("AnonemousLogin", GetAnonymousLogin()).toBool());
    SetReadOnly(set.value("ReadOnly", GetReadOnly()).toBool());
    return 0;
}

int CParameterFtpServer::OnSave(QSettings &set)
{
    set.setValue("Port", GetPort());
    set.setValue("UserName", GetUser());
    set.setValue("Password", GetPassword());
    set.setValue("Root", GetRoot());
    set.setValue("AnonemousLogin", GetAnonymousLogin());
    set.setValue("ReadOnly", GetReadOnly());
    return 0;
}
