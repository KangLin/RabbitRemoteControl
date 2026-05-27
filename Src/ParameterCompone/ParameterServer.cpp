// Author: Kang Lin <kl222@126.com>

#include "ParameterServer.h"

CParameterServer::CParameterServer(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_Net(this)
    , m_WhiteFilter(this, "WhiteFilter")
    , m_BlackFilter(this, "BlackFilter")
    , m_bAnonymousLogin(false)
    , m_bReadOnly(true)
    , m_tmAuthenticate(600000)
    , m_AuthAttempts(3)
    , m_ConnectCount(-1)
    , m_bListenAll(true)
{
    m_szHostKeyFile = "ssh_host_rsa_key";
}

int CParameterServer::OnLoad(QSettings &set)
{
    int nRet = 0;
    SetAnonymousLogin(set.value("AnonemousLogin", GetAnonymousLogin()).toBool());
    SetReadOnly(set.value("ReadOnly", GetReadOnly()).toBool());
    SetRoot(set.value("Root", GetRoot()).toString());
    SetHostKeyFile(set.value("HostKeyFile", GetHostKeyFile()).toString());
    SetAuthenticateTime(set.value("AuthenticateTime", GetAuthenticateTime()).toDouble());
    SetAuthenticateAttempts(set.value("AuthenticateAttempts", GetAuthenticateAttempts()).toInt());
    SetConnectCount(set.value("ConnectCount", GetConnectCount()).toInt());
    SetListenAll(set.value("ListenAll", GetListenAll()).toBool());
    SetListen(set.value("Listen", GetListen()).toStringList());
    return nRet;
}

int CParameterServer::OnSave(QSettings &set)
{
    int nRet = 0;
    set.setValue("AnonemousLogin", GetAnonymousLogin());
    set.setValue("ReadOnly", GetReadOnly());
    set.setValue("Root", GetRoot());
    set.setValue("HostKeyFile", GetHostKeyFile());
    set.setValue("AuthenticateTime", GetAuthenticateTime());
    set.setValue("AuthenticateAttempts", GetAuthenticateAttempts());
    set.setValue("ConnectCount", GetConnectCount());
    set.setValue("ListenAll", GetListenAll());
    set.setValue("Listen", GetListen());
    return nRet;
}

bool CParameterServer::GetAnonymousLogin() const
{
    return m_bAnonymousLogin;
}

void CParameterServer::SetAnonymousLogin(bool newAnonymousLogin)
{
    if(m_bAnonymousLogin == newAnonymousLogin)
        return;
    m_bAnonymousLogin = newAnonymousLogin;
    SetModified(true);
}

bool CParameterServer::GetReadOnly() const
{
    return m_bReadOnly;
}

void CParameterServer::SetReadOnly(bool newReadOnly)
{
    if(m_bReadOnly == newReadOnly)
        return;
    m_bReadOnly = newReadOnly;
    SetModified(true);
}

QString CParameterServer::GetRoot() const
{
    return m_szRoot;
}

void CParameterServer::SetRoot(const QString &newRoot)
{
    if(m_szRoot == newRoot)
        return;
    m_szRoot = newRoot;
    SetModified(true);
}

QString CParameterServer::GetHostKeyFile() const
{
    return m_szHostKeyFile;
}

void CParameterServer::SetHostKeyFile(const QString &key)
{
    if(m_szHostKeyFile == key)
        return;
    m_szHostKeyFile = key;
    SetModified(true);
}

qint64 CParameterServer::GetAuthenticateTime() const
{
    return m_tmAuthenticate;
}

void CParameterServer::SetAuthenticateTime(qint64 tm)
{
    if(m_tmAuthenticate == tm)
        return;
    m_tmAuthenticate = tm;
    SetModified(true);
}

int CParameterServer::GetAuthenticateAttempts() const
{
    return m_AuthAttempts;
}

void CParameterServer::SetAuthenticateAttempts(int attempts)
{
    if(m_AuthAttempts == attempts)
        return;
    m_AuthAttempts = attempts;
    SetModified(true);
}

int CParameterServer::GetConnectCount() const
{
    return m_ConnectCount;
}

void CParameterServer::SetConnectCount(int newConnectCount)
{
    if(m_ConnectCount == newConnectCount)
        return;
    m_ConnectCount = newConnectCount;
    SetModified(true);
}

bool CParameterServer::GetListenAll() const
{
    return m_bListenAll;
}

void CParameterServer::SetListenAll(bool newListenAll)
{
    if(m_bListenAll == newListenAll)
        return;
    m_bListenAll = newListenAll;
    SetModified(true);
}

QStringList CParameterServer::GetListen() const
{
    return m_Listen;
}

void CParameterServer::SetListen(const QStringList &newListen)
{
    if(m_Listen == newListen)
        return;
    m_Listen = newListen;
    SetModified(true);
}

void CParameterServer::slotSetPluginParameters()
{
    m_Net.m_User.SetSavePassword(true);
    m_Net.m_User.SetSavePassphrase(true);
}
