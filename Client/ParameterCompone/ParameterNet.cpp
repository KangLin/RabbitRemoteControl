#include "ParameterNet.h"
#include "RabbitCommonTools.h"

//! [Constructor]
CParameterNet::CParameterNet(CParameterConnecter* parent, const QString &szPrefix)
    : CParameterConnecter(parent, szPrefix)
    , m_nPort(0)
    , m_User(this)
    , m_szPrompt(tr("The host is empty. please set it"))
    , m_WakeOnLan(this)
{}
//! [Constructor]

int CParameterNet::OnLoad(QSettings &set)
{
    set.beginGroup("Net");
    SetHost(set.value("Host", GetHost()).toString());
    SetPort(set.value("Port", GetPort()).toUInt());
    set.endGroup();
    return 0;
}

int CParameterNet::OnSave(QSettings &set)
{
    set.beginGroup("Net");
    set.setValue("Host", GetHost());
    set.setValue("Port", GetPort());
    set.endGroup();
    return 0;
}

const QString CParameterNet::GetHost() const
{
    return m_szHost;
}

void CParameterNet::SetHost(const QString& host)
{
    if (m_szHost == host)
        return;
    m_szHost = host;
    SetModified(true);
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

int CParameterNet::SetPrompt(const QString szPrompt)
{
    m_szPrompt = szPrompt;
    return 0;
}

QString CParameterNet::GetPrompt()
{
    return m_szPrompt;
}
