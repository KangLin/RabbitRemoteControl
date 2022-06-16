#include "ParameterSignal.h"

CParameterSignal::CParameterSignal(QObject *parent)
    : QObject{parent}
{
    SetIce(false);
    SetSignalPort(5222);
    SetStunPort(3748);
    SetTurnPort(3748);
}

int CParameterSignal::Load(QSettings &set)
{
    int nRet = 0;
    
    SetIce(set.value("ICE/Enable", GetIce()).toBool());
    SetSignalServer(set.value("ICE/Signal/Server",
                              GetSignalServer()).toString());
    SetSignalPort(set.value("ICE/Signal/Port", GetSignalPort()).toUInt());
    SetSignalUser(set.value("ICE/Signal/User", GetSignalUser()).toString());
    SetSignalPassword(set.value("ICE/Signal/password", GetSignalPassword()).toString());
    
    SetPeerUser(set.value("ICE/Peer/User", GetPeerUser()).toString());
    SetStunServer(set.value("ICE/Stun/Server", GetStunServer()).toString());
    SetStunPort(set.value("ICE/Stun/Port", GetStunPort()).toUInt());
    SetTurnServer(set.value("ICE/Turn/Server", GetTurnServer()).toString());
    SetTurnPort(set.value("ICE/Turn/Port", GetTurnPort()).toUInt());
    SetTurnUser(set.value("ICE/Turn/User", GetTurnUser()).toString());
    SetTurnPassword(set.value("ICE/Turn/password", GetTurnPassword()).toString());

    return nRet;
}

int CParameterSignal::Save(QSettings &set)
{
    int nRet = 0;
    set.setValue("ICE/Enable", GetIce());
    set.setValue("ICE/Signal/Server", GetSignalServer());
    set.setValue("ICE/Signal/Port", GetSignalPort());
    set.setValue("ICE/Signal/User", GetSignalUser());
    set.setValue("ICE/Signal/password", GetSignalPassword());
    set.setValue("ICE/Peer/User", GetPeerUser());
    set.setValue("ICE/Stun/Server", GetStunServer());
    set.setValue("ICE/Stun/Port", GetStunPort());
    set.setValue("ICE/Turn/Server", GetTurnServer());
    set.setValue("ICE/Turn/Port", GetTurnPort());
    set.setValue("ICE/Turn/User", GetTurnUser());
    set.setValue("ICE/Turn/password", GetTurnPassword());
    return nRet;
}


bool CParameterSignal::GetIce() const
{
    return m_bIce;
}

void CParameterSignal::SetIce(bool newIce)
{
    m_bIce = newIce;
}

const QString &CParameterSignal::GetSignalServer() const
{
    return m_szSignalServer;
}

void CParameterSignal::SetSignalServer(const QString &newSignalServer)
{
    m_szSignalServer = newSignalServer;
}

quint16 CParameterSignal::GetSignalPort() const
{
    return m_nSignalPort;
}

void CParameterSignal::SetSignalPort(quint16 newSignalPort)
{
    m_nSignalPort = newSignalPort;
}

const QString &CParameterSignal::GetSignalUser() const
{
    return m_szSignalUser;
}

void CParameterSignal::SetSignalUser(const QString &newSignalUser)
{
    m_szSignalUser = newSignalUser;
}

const QString &CParameterSignal::GetSignalPassword() const
{
    return m_szSignalPassword;
}

void CParameterSignal::SetSignalPassword(const QString &newSignalPassword)
{
    m_szSignalPassword = newSignalPassword;
}

const QString &CParameterSignal::GetPeerUser() const
{
    return m_szPeerUser;
}

void CParameterSignal::SetPeerUser(const QString &newPeerUser)
{
    m_szPeerUser = newPeerUser;
}

const QString &CParameterSignal::GetStunServer() const
{
    return m_szStunServer;
}

void CParameterSignal::SetStunServer(const QString &newStunServer)
{
    m_szStunServer = newStunServer;
}

quint16 CParameterSignal::GetStunPort() const
{
    return m_nStunPort;
}

void CParameterSignal::SetStunPort(quint16 newStunPort)
{
    m_nStunPort = newStunPort;
}

const QString &CParameterSignal::GetTurnServer() const
{
    return m_szTurnServer;
}

void CParameterSignal::SetTurnServer(const QString &newTurnServer)
{
    m_szTurnServer = newTurnServer;
}

quint16 CParameterSignal::GetTurnPort() const
{
    return m_nTurnPort;
}

void CParameterSignal::SetTurnPort(quint16 newTurnPort)
{
    m_nTurnPort = newTurnPort;
}

const QString &CParameterSignal::GetTurnUser() const
{
    return m_szTurnUser;
}

void CParameterSignal::SetTurnUser(const QString &newTurnUser)
{
    m_szTurnUser = newTurnUser;
}

const QString &CParameterSignal::GetTurnPassword() const
{
    return m_szTurnPassword;
}

void CParameterSignal::SetTurnPassword(const QString &newTurnPassword)
{
    m_szTurnPassword = newTurnPassword;
}
