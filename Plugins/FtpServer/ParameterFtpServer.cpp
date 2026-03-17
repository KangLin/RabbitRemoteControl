// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include "ParameterFtpServer.h"

CParameterFtpServer::CParameterFtpServer(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_Net(this)
    , m_bAnonymousLogin(false)
    , m_bReadOnly(true)
    , m_ConnectCount(2)
    , m_bListenAll(true)
{
#if defined(Q_OS_UNIX)
    m_Net.SetPort(2121);
#else
    m_Net.SetPort(21);
#endif
    m_Net.m_User.SetSavePassword(true);
    m_Net.SetEnablleUI(CParameterNet::SHOW_UI::Port | CParameterNet::SHOW_UI::User);
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
