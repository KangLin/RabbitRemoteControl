// Author: Kang Lin <kl222@126.com>

#include "ParameterNet.h"
#include "RabbitCommonTools.h"

Q_DECLARE_METATYPE(CParameterNet::SHOW_UIS)

//! [Constructor]
CParameterNet::CParameterNet(CParameterOperate* parent, const QString &szPrefix)
    : CParameterOperate(parent, szPrefix)
    , m_nPort(0)
    , m_User(this)
    , m_szPrompt(tr("The host is empty. please set it"))
    , m_ShowUI(SHOW_UI::All)
    , m_EnableUI(SHOW_UI::All)
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
    emit sigHostChanged(m_szHost);
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

CParameterNet& CParameterNet::operator =(const CParameterNet& in)
{
    m_szHost = in.m_szHost;
    m_nPort = in.m_nPort;
    m_User = in.m_User;
    m_szPrompt = in.m_szPrompt;
    return *this;
}

CParameterNet::SHOW_UIS CParameterNet::GetShowUI()
{
    return m_ShowUI;
}

CParameterNet::SHOW_UIS CParameterNet::SetShowUI(SHOW_UIS ui)
{
    SHOW_UIS uis = m_ShowUI;
    if(m_ShowUI != ui)
        m_ShowUI = ui;
    return uis;
}

CParameterNet::SHOW_UIS CParameterNet::GetEnableUI()
{
    return m_EnableUI;
}

CParameterNet::SHOW_UIS CParameterNet::SetEnablleUI(SHOW_UIS ui)
{
    SHOW_UIS uis = m_EnableUI;
    if(m_EnableUI != ui)
        m_EnableUI = ui;
    return uis;
}
