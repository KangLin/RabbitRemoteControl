#include "ParameterNet.h"
#include "RabbitCommonTools.h"

CParameterNet::CParameterNet(CParameterConnecter* parent, const QString &szPrefix)
    : CParameterConnecter(parent, szPrefix),
    m_bShowServerName(true),
    m_nPort(0),
    m_bSavePassword(false)
{
    SetUser(RabbitCommon::CTools::GetCurrentUser());
}

int CParameterNet::onLoad(QSettings &set)
{
    set.beginGroup("Net");
    SetName(set.value("Name", GetName()).toString());
    SetServerName(set.value("ServerName", GetServerName()).toString());
    SetShowServerName(set.value("ShowServerName", GetShowServerName()).toBool());
    SetHost(set.value("Host", GetHost()).toString());
    SetPort(set.value("Port", GetPort()).toUInt());
    SetUser(set.value("User", GetUser()).toString());

    //! [Initialize parameter]
    bool bDefaultSavePassword = GetSavePassword();
    if(GetParameterClient())
        bDefaultSavePassword = GetParameterClient()->GetSavePassword();
    SetSavePassword(set.value("SavePassword", bDefaultSavePassword).toBool());
    //! [Initialize parameter]
    if(GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Password"), "Password", szPassword, set))
            SetPassword(szPassword);
    }
    set.endGroup();
    return 0;
}

int CParameterNet::onSave(QSettings &set)
{
    set.beginGroup("Net");
    set.setValue("Name", GetName());
    set.setValue("ServerName", GetServerName());
    set.setValue("ShowServerName", GetShowServerName());
    set.setValue("Host", GetHost());
    set.setValue("Port", GetPort());
    set.setValue("User", GetUser());
    SavePassword("Password", GetPassword(), set, GetSavePassword());
    set.endGroup();
    return 0;
}

const QString CParameterNet::GetName() const
{
    return m_szName;
}

void CParameterNet::SetName(const QString& szName)
{
    if(m_szName == szName)
        return;
    m_szName = szName;
    SetModified(true);
    emit sigNameChanged(m_szName);
}

const QString CParameterNet::GetServerName() const
{
    return m_szServerName;
}

void CParameterNet::SetServerName(const QString& szName)
{
    if(m_szServerName == szName)
        return;
    m_szServerName = szName;
    SetModified(true);
}

bool CParameterNet::GetShowServerName() const
{
    return m_bShowServerName;
}

void CParameterNet::SetShowServerName(bool NewShowServerName)
{
    if (m_bShowServerName == NewShowServerName)
        return;
    m_bShowServerName = NewShowServerName;
    SetModified(true);
    emit sigShowServerNameChanged();
}

void CParameterNet::SetHost(const QString& host)
{
    if (m_szHost == host)
        return;
    m_szHost = host;
    SetModified(true);
}

const QString CParameterNet::GetHost() const
{
    return m_szHost;
}

const quint16 CParameterNet::GetPort() const
{
    return m_nPort;
}

void CParameterNet::SetPort(quint16 port)
{
    if(m_nPort == port)
        return;
    m_nPort = port;
    SetModified(true);
}

const QString CParameterNet::GetUser() const
{
    return m_szUser;
}

void CParameterNet::SetUser(const QString &szUser)
{
    if (m_szUser == szUser)
        return;
    m_szUser = szUser;
    SetModified(true);
}

const QString CParameterNet::GetPassword() const
{
    return m_szPassword;
}

void CParameterNet::SetPassword(const QString &szPassword)
{
    if(m_szPassword == szPassword)
        return;
    m_szPassword = szPassword;
    SetModified(true);
}

const bool CParameterNet::GetSavePassword() const
{
    return m_bSavePassword;
}

void CParameterNet::SetSavePassword(bool save)
{
    if (m_bSavePassword == save)
        return;
    m_bSavePassword = save;
    SetModified(true);
}
