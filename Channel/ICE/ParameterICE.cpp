#include "ParameterICE.h"

CParameterICE::CParameterICE(QObject *parent)
    : QObject{parent}
{
    m_bIce = false;
    m_bIceDebug = false;
    m_nSignalPort = 5222;
    m_nStunPort = m_nTurnPort = 3478;
}

int CParameterICE::Load(QSettings &set)
{
    int nRet = 0;

    m_szSignalServer = set.value("ICE/Signal/Server", m_szSignalServer).toString();
    m_nSignalPort = set.value("ICE/Signal/Port", m_nSignalPort).toUInt();
    m_szSignalUser = set.value("ICE/Signal/User", m_szSignalUser).toString();
    m_szSignalPassword = set.value("ICE/Signal/Password", m_szSignalPassword).toString();
    m_szStunServer = set.value("ICE/STUN/Server", m_szTurnServer).toString();
    m_nStunPort = set.value("ICE/STUN/Port", m_nStunPort).toUInt();
    m_szTurnServer = set.value("ICE/TURN/Server", m_szTurnServer).toString();
    m_nTurnPort = set.value("ICE/TURN/Port", m_nTurnPort).toUInt();
    m_szTurnUser = set.value("ICE/TURN/User", m_szTurnUser).toString();
    m_szTurnPassword = set.value("ICE/TURN/Password", m_szTurnPassword).toString();

    setIce(set.value("ICE/Enable", m_bIce).toBool());
    SetIceDebug(set.value("ICE/Enable/Debug", GetIceDebug()).toBool());
    return nRet;
}

int CParameterICE::Save(QSettings &set)
{
    int nRet = 0;
    set.setValue("ICE/Signal/Server", m_szSignalServer);
    set.setValue("ICE/Signal/Port", m_nSignalPort);
    set.setValue("ICE/Signal/User", m_szSignalUser);
    set.setValue("ICE/Signal/Password", m_szSignalPassword);
    set.setValue("ICE/STUN/Server", m_szStunServer);
    set.setValue("ICE/STUN/Port", m_nStunPort);
    set.setValue("ICE/TURN/Server", m_szTurnServer);
    set.setValue("ICE/TURN/Port", m_nTurnPort);
    set.setValue("ICE/TURN/User", m_szTurnUser);
    set.setValue("ICE/TURN/Password", m_szTurnPassword);
    set.setValue("ICE/Enable", m_bIce);
    set.setValue("ICE/Enable/Debug", GetIceDebug());
    return nRet;
}

bool CParameterICE::getIce() const
{
    return m_bIce;
}

void CParameterICE::setIce(bool newBIce)
{
    if (m_bIce == newBIce)
        return;
    m_bIce = newBIce;
    emit sigIceChanged();
}

bool CParameterICE::GetIceDebug() const
{
    return m_bIceDebug;
}

void CParameterICE::SetIceDebug(bool newIceDebug)
{
    if(m_bIceDebug == newIceDebug)
        return;
    m_bIceDebug = newIceDebug;
    emit sigIceDebugChanged(m_bIceDebug);
}

const QString &CParameterICE::getSignalServer() const
{
    return m_szSignalServer;
}

void CParameterICE::setSignalServer(const QString &newSzSignalServer)
{
    if (m_szSignalServer == newSzSignalServer)
        return;
    m_szSignalServer = newSzSignalServer;
    emit sigSignalServerChanged();
}

quint16 CParameterICE::getSignalPort() const
{
    return m_nSignalPort;
}

void CParameterICE::setSignalPort(quint16 newNSignalPort)
{
    if (m_nSignalPort == newNSignalPort)
        return;
    m_nSignalPort = newNSignalPort;
    emit sigSignalPortChanged();
}

const QString &CParameterICE::getSignalUser() const
{
    return m_szSignalUser;
}

void CParameterICE::setSignalUser(const QString &newSzSignalUser)
{
    if (m_szSignalUser == newSzSignalUser)
        return;
    m_szSignalUser = newSzSignalUser;
    emit sigSignalUserChanged();
}

const QString &CParameterICE::getSignalPassword() const
{
    return m_szSignalPassword;
}

void CParameterICE::setSignalPassword(const QString &newSzSignalPassword)
{
    if (m_szSignalPassword == newSzSignalPassword)
        return;
    m_szSignalPassword = newSzSignalPassword;
    emit sigSignalPasswordChanged();
}

const QString &CParameterICE::getStunServer() const
{
    return m_szStunServer;
}

void CParameterICE::setStunServer(const QString &newSzStunServer)
{
    if (m_szStunServer == newSzStunServer)
        return;
    m_szStunServer = newSzStunServer;
    emit sigStunServerChanged();
}

quint16 CParameterICE::getStunPort() const
{
    return m_nStunPort;
}

void CParameterICE::setStunPort(quint16 newNStunPort)
{
    if (m_nStunPort == newNStunPort)
        return;
    m_nStunPort = newNStunPort;
    emit sigStunPortChanged();
}

const QString &CParameterICE::getTurnServer() const
{
    return m_szTurnServer;
}

void CParameterICE::setTurnServer(const QString &newSzTurnServer)
{
    if (m_szTurnServer == newSzTurnServer)
        return;
    m_szTurnServer = newSzTurnServer;
    emit sigTurnServerChanged();
}

quint16 CParameterICE::getTurnPort() const
{
    return m_nTurnPort;
}

void CParameterICE::setTurnPort(quint16 newNTurnPort)
{
    if (m_nTurnPort == newNTurnPort)
        return;
    m_nTurnPort = newNTurnPort;
    emit sigTurnPortChanged();
}

const QString &CParameterICE::getTurnUser() const
{
    return m_szTurnUser;
}

void CParameterICE::setTurnUser(const QString &newSzTurnUser)
{
    if (m_szTurnUser == newSzTurnUser)
        return;
    m_szTurnUser = newSzTurnUser;
    emit sigTurnUserChanged();
}

const QString &CParameterICE::getTurnPassword() const
{
    return m_szTurnPassword;
}

void CParameterICE::setTurnPassword(const QString &newSzTurnPassword)
{
    if (m_szTurnPassword == newSzTurnPassword)
        return;
    m_szTurnPassword = newSzTurnPassword;
    emit sigTurnPasswordChanged();
}
