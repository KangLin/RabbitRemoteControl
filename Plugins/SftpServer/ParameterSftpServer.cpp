// Author: Kang Lin <kl222@126.com>

#include "ParameterSftpServer.h"

CParameterSftpServer::CParameterSftpServer(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_Net(this)
    , m_tmAuthenticate(600000)
    , m_AuthAttempts(3)
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
}

int CParameterSftpServer::OnLoad(QSettings &set)
{
    int nRet = 0;
    SetRoot(set.value("Root", GetRoot()).toString());
    SetHostKeyFile(set.value("HostKeyFile", GetHostKeyFile()).toString());
    SetAuthenticateTime(set.value("AuthenticateTime", GetAuthenticateTime()).toDouble());
    SetAuthenticateAttempts(set.value("AuthenticateAttempts", GetAuthenticateAttempts()).toInt());
    return nRet;
}

int CParameterSftpServer::OnSave(QSettings &set)
{
    int nRet = 0;
    set.setValue("Root", GetRoot());
    set.setValue("HostKeyFile", GetHostKeyFile());
    set.setValue("AuthenticateTime", GetAuthenticateTime());
    set.setValue("AuthenticateAttempts", GetAuthenticateAttempts());
    return nRet;
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

QString CParameterSftpServer::GetHostKeyFile() const
{
    return m_szHostKeyFile;
}

void CParameterSftpServer::SetHostKeyFile(const QString &key)
{
    if(m_szHostKeyFile == key)
        return;
    m_szHostKeyFile = key;
    SetModified(true);
}

qint64 CParameterSftpServer::GetAuthenticateTime() const
{
    return m_tmAuthenticate;
}

void CParameterSftpServer::SetAuthenticateTime(qint64 tm)
{
    if(m_tmAuthenticate == tm)
        return;
    m_tmAuthenticate = tm;
    SetModified(true);
}

int CParameterSftpServer::GetAuthenticateAttempts() const
{
    return m_AuthAttempts;
}

void CParameterSftpServer::SetAuthenticateAttempts(int attempts)
{
    if(m_AuthAttempts == attempts)
        return;
    m_AuthAttempts = attempts;
    SetModified(true);
}
