// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include "ParameterFtpServer.h"

CParameterFtpServer::CParameterFtpServer(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_nPort(21)
    , m_bAnonymousLogin(true)
    , m_bReadOnly(true)
    , m_ConnectCount(-1)
    , m_bListenAll(true)
{}

uint16_t CParameterFtpServer::GetPort() const
{
    return m_nPort;
}

void CParameterFtpServer::SetPort(uint16_t newPort)
{
    if(m_nPort == newPort)
        return;
    m_nPort = newPort;
    SetModified(true);
}

QString CParameterFtpServer::GetUser() const
{
    return m_szUser;
}

void CParameterFtpServer::SetUser(const QString &newUser)
{
    if(m_szUser == newUser)
        return;
    m_szUser = newUser;
    SetModified(true);
}

QString CParameterFtpServer::GetPassword() const
{
    return m_szPassword;
}

void CParameterFtpServer::SetPassword(const QString &newPassword)
{
    if(m_szPassword == newPassword)
        return;
    m_szPassword = newPassword;
    SetModified(true);
}

bool CParameterFtpServer::GetAnonymousLogin() const
{
    return m_bAnonymousLogin;
}

void CParameterFtpServer::SetAnonymousLogin(bool newAnonymousLogin)
{
    if(m_bAnonymousLogin == newAnonymousLogin)
        return;
    m_bAnonymousLogin = newAnonymousLogin;
    SetModified(true);
}

bool CParameterFtpServer::GetReadOnly() const
{
    return m_bReadOnly;
}

void CParameterFtpServer::SetReadOnly(bool newReadOnly)
{
    if(m_bReadOnly == newReadOnly)
        return;
    m_bReadOnly = newReadOnly;
    SetModified(true);
}

QString CParameterFtpServer::GetRoot() const
{
    return m_szRoot;
}

void CParameterFtpServer::SetRoot(const QString &newRoot)
{
    if(m_szRoot == newRoot)
        return;
    m_szRoot = newRoot;
    SetModified(true);
}

int CParameterFtpServer::GetConnectCount() const
{
    return m_ConnectCount;
}

void CParameterFtpServer::SetConnectCount(int newConnectCount)
{
    if(m_ConnectCount == newConnectCount)
        return;
    m_ConnectCount = newConnectCount;
    SetModified(true);
}

bool CParameterFtpServer::GetListenAll() const
{
    return m_bListenAll;
}

void CParameterFtpServer::SetListenAll(bool newListenAll)
{
    if(m_bListenAll == newListenAll)
        return;
    m_bListenAll = newListenAll;
    SetModified(true);
}

QStringList CParameterFtpServer::GetListen() const
{
    return m_Listen;
}

void CParameterFtpServer::SetListen(const QStringList &newListen)
{
    if(m_Listen == newListen)
        return;
    m_Listen = newListen;
    SetModified(true);
}

QStringList CParameterFtpServer::GetWhitelist() const
{
    return m_Whitelist;
}

void CParameterFtpServer::SetWhitelist(const QStringList &newWhitelist)
{
    if(m_Whitelist == newWhitelist)
        return;
    m_Whitelist = newWhitelist;
    SetModified(true);
}

QStringList CParameterFtpServer::GetBlacklist() const
{
    return m_Blacklist;
}

void CParameterFtpServer::SetBlacklist(const QStringList &newBlacklist)
{
    if(m_Blacklist == newBlacklist)
        return;
    m_Blacklist = newBlacklist;
    SetModified(true);
}

int CParameterFtpServer::OnLoad(QSettings &set)
{
    SetPort(set.value("Port", GetPort()).toUInt());
    SetUser(set.value("UserName", GetUser()).toString());
    SetPassword(set.value("Password", GetPassword()).toString());
    SetRoot(set.value("Root", GetRoot()).toString());
    SetAnonymousLogin(set.value("AnonemousLogin", GetAnonymousLogin()).toBool());
    SetReadOnly(set.value("ReadOnly", GetReadOnly()).toBool());
    SetConnectCount(set.value("ConnectCount", GetConnectCount()).toInt());
    SetListenAll(set.value("ListenAll", GetListenAll()).toBool());
    SetListen(set.value("Listen", GetListen()).toStringList());
    SetWhitelist(set.value("List/White", GetWhitelist()).toStringList());
    SetBlacklist(set.value("List/Black", GetBlacklist()).toStringList());
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
    set.setValue("ConnectCount", GetConnectCount());
    set.setValue("ListenAll", GetListenAll());
    set.setValue("Listen", GetListen());
    set.setValue("List/White", GetWhitelist());
    set.setValue("List/Black", GetBlacklist());
    return 0;
}
