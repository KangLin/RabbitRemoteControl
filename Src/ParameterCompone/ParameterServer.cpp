// Author: Kang Lin <kl222@126.com>

#include "ParameterServer.h"

CParameterServer::CParameterServer(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_Net(this)
    , m_tmAuthenticate(600000)
    , m_AuthAttempts(3)
    , m_ConnectCount(-1)
    , m_bListenAll(true)
{
#if defined(Q_OS_UNIX)
    m_Net.SetPort(2222);
    // TODO: modify host key file
    //m_szHostKeyFile = "/etc/ssh/ssh_host_rsa_key";
    m_szHostKeyFile = "/data/libssh/tests/keys/ssh_host_rsa_key";
#else
    m_Net.SetPort(22);
    m_szHostKeyFile = "D:\\source\\vcpkg\\buildtrees\\libssh\\src\\libssh-0-8f8fad1532.clean\\tests\\keys\\ssh_host_rsa_key";
#endif
    m_Net.SetEnablleUI(CParameterNet::SHOW_UI::Port | CParameterNet::SHOW_UI::User);
}

int CParameterServer::OnLoad(QSettings &set)
{
    int nRet = 0;
    SetRoot(set.value("Root", GetRoot()).toString());
    SetHostKeyFile(set.value("HostKeyFile", GetHostKeyFile()).toString());
    SetAuthenticateTime(set.value("AuthenticateTime", GetAuthenticateTime()).toDouble());
    SetAuthenticateAttempts(set.value("AuthenticateAttempts", GetAuthenticateAttempts()).toInt());
    SetConnectCount(set.value("ConnectCount", GetConnectCount()).toInt());
    SetListenAll(set.value("ListenAll", GetListenAll()).toBool());
    SetListen(set.value("Listen", GetListen()).toStringList());
    SetWhitelist(set.value("List/White", GetWhitelist()).toStringList());
    SetBlacklist(set.value("List/Black", GetBlacklist()).toStringList());
    return nRet;
}

int CParameterServer::OnSave(QSettings &set)
{
    int nRet = 0;
    set.setValue("Root", GetRoot());
    set.setValue("HostKeyFile", GetHostKeyFile());
    set.setValue("AuthenticateTime", GetAuthenticateTime());
    set.setValue("AuthenticateAttempts", GetAuthenticateAttempts());
    set.setValue("ConnectCount", GetConnectCount());
    set.setValue("ListenAll", GetListenAll());
    set.setValue("Listen", GetListen());
    set.setValue("List/White", GetWhitelist());
    set.setValue("List/Black", GetBlacklist());
    return nRet;
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

QStringList CParameterServer::GetWhitelist() const
{
    return m_Whitelist;
}

void CParameterServer::SetWhitelist(const QStringList &newWhitelist)
{
    if(m_Whitelist == newWhitelist)
        return;
    m_Whitelist = newWhitelist;
    SetModified(true);
}

QStringList CParameterServer::GetBlacklist() const
{
    return m_Blacklist;
}

void CParameterServer::SetBlacklist(const QStringList &newBlacklist)
{
    if(m_Blacklist == newBlacklist)
        return;
    m_Blacklist = newBlacklist;
    SetModified(true);
}
