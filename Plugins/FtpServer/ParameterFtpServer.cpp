// Author: Kang Lin <kl222@126.com>

#include "ParameterFtpServer.h"

CParameterFtpServer::CParameterFtpServer(QObject *parent, const QString &szPrefix)
    : CParameterServer{parent, szPrefix}
    , m_bAnonymousLogin(false)
    , m_bReadOnly(true)
{
#if defined(Q_OS_UNIX)
    m_Net.SetPort(2121);
#else
    m_Net.SetPort(21);
#endif
    m_Net.SetEnablleUI(CParameterNet::SHOW_UI::Port | CParameterNet::SHOW_UI::User);
}

int CParameterFtpServer::OnLoad(QSettings &set)
{
    int nRet = 0;
    nRet = CParameterServer::OnLoad(set);
    if(nRet) return nRet;
    SetAnonymousLogin(set.value("AnonemousLogin", GetAnonymousLogin()).toBool());
    SetReadOnly(set.value("ReadOnly", GetReadOnly()).toBool());
    SetRoot(set.value("Root", GetRoot()).toString());
    return nRet;
}

int CParameterFtpServer::OnSave(QSettings &set)
{
    int nRet = 0;
    nRet = CParameterServer::OnSave(set);
    if(nRet) return nRet;
    set.setValue("AnonemousLogin", GetAnonymousLogin());
    set.setValue("ReadOnly", GetReadOnly());
    set.setValue("Root", GetRoot());
    return nRet;
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
