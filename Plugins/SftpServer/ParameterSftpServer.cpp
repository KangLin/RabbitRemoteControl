// Author: Kang Lin <kl222@126.com>

#include "ParameterSftpServer.h"

CParameterSftpServer::CParameterSftpServer(QObject *parent, const QString &szPrefix)
    : CParameterServer{parent, szPrefix}
    , m_bAnonymousLogin(false)
    , m_bReadOnly(true)
{
#if defined(Q_OS_UNIX)
    m_Net.SetPort(2222);
    // TODO: modify host key file
    //m_szHostKeyFile = "/etc/ssh/ssh_host_rsa_key";
    SetHostKeyFile("/data/libssh/tests/keys/ssh_host_rsa_key");
#else
    m_Net.SetPort(22);
    SetHostKeyFile("D:\\source\\vcpkg\\buildtrees\\libssh\\src\\libssh-0-e5f4972781.clean\\tests\\keys\\ssh_host_rsa_key");
#endif
    m_Net.SetEnablleUI(CParameterNet::SHOW_UI::Port | CParameterNet::SHOW_UI::User);
}

int CParameterSftpServer::OnLoad(QSettings &set)
{
    int nRet = 0;
    nRet = CParameterServer::OnLoad(set);
    if(nRet) return nRet;
    SetAnonymousLogin(set.value("AnonemousLogin", GetAnonymousLogin()).toBool());
    SetReadOnly(set.value("ReadOnly", GetReadOnly()).toBool());
    SetRoot(set.value("Root", GetRoot()).toString());
    return nRet;
}

int CParameterSftpServer::OnSave(QSettings &set)
{
    int nRet = 0;
    nRet = CParameterServer::OnSave(set);
    if(nRet) return nRet;
    set.setValue("AnonemousLogin", GetAnonymousLogin());
    set.setValue("ReadOnly", GetReadOnly());
    set.setValue("Root", GetRoot());
    return nRet;
}

bool CParameterSftpServer::GetAnonymousLogin() const
{
    return m_bAnonymousLogin;
}

void CParameterSftpServer::SetAnonymousLogin(bool newAnonymousLogin)
{
    if(m_bAnonymousLogin == newAnonymousLogin)
        return;
    m_bAnonymousLogin = newAnonymousLogin;
    SetModified(true);
}

bool CParameterSftpServer::GetReadOnly() const
{
    return m_bReadOnly;
}

void CParameterSftpServer::SetReadOnly(bool newReadOnly)
{
    if(m_bReadOnly == newReadOnly)
        return;
    m_bReadOnly = newReadOnly;
    SetModified(true);
}

QString CParameterSftpServer::GetRoot() const
{
    return m_szRoot;
}

void CParameterSftpServer::SetRoot(const QString &newRoot)
{
    if(m_szRoot == newRoot)
        return;
    m_szRoot = newRoot;
    SetModified(true);
}