// Author: Kang Lin <kl222@126.com>

#include "ParameterServiceRabbitVNC.h"
#include <QSettings>
#include "RabbitCommonLog.h"

CParameterServiceRabbitVNC::CParameterServiceRabbitVNC(QObject *parent)
    : CParameterService(parent)
{
    setPort(5900);
    m_bIce = false;
    m_nSignalPort = 5222;
    m_nStunPort = m_nTurnPort = 3478;
}

CParameterServiceRabbitVNC::~CParameterServiceRabbitVNC()
{
    LOG_MODEL_DEBUG("CParameterServiceTigerVNC", "CParameterServiceTigerVNC::~CParameterServiceTigerVNC");
}

int CParameterServiceRabbitVNC::OnLoad(const QString& szFile)
{
    int nRet = CParameterService::OnLoad(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    m_bIce = set.value("ICE/Enable", m_bIce).toBool();
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
    return nRet;
}

int CParameterServiceRabbitVNC::OnSave(const QString& szFile)
{
    int nRet = CParameterService::OnSave(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    set.setValue("ICE/Enable", m_bIce);
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
    set.sync();
    return nRet;
}

bool CParameterServiceRabbitVNC::getIce() const
{
    return m_bIce;
}

void CParameterServiceRabbitVNC::setIce(bool newBIce)
{
    if (m_bIce == newBIce)
        return;
    m_bIce = newBIce;
    emit sigIceChanged();
}

const QString &CParameterServiceRabbitVNC::getSignalServer() const
{
    return m_szSignalServer;
}

void CParameterServiceRabbitVNC::setSignalServer(const QString &newSzSignalServer)
{
    if (m_szSignalServer == newSzSignalServer)
        return;
    m_szSignalServer = newSzSignalServer;
    emit sigSignalServerChanged();
}

quint16 CParameterServiceRabbitVNC::getSignalPort() const
{
    return m_nSignalPort;
}

void CParameterServiceRabbitVNC::setSignalPort(quint16 newNSignalPort)
{
    if (m_nSignalPort == newNSignalPort)
        return;
    m_nSignalPort = newNSignalPort;
    emit sigSignalPortChanged();
}

const QString &CParameterServiceRabbitVNC::getSignalUser() const
{
    return m_szSignalUser;
}

void CParameterServiceRabbitVNC::setSignalUser(const QString &newSzSignalUser)
{
    if (m_szSignalUser == newSzSignalUser)
        return;
    m_szSignalUser = newSzSignalUser;
    emit sigSignalUserChanged();
}

const QString &CParameterServiceRabbitVNC::getSignalPassword() const
{
    return m_szSignalPassword;
}

void CParameterServiceRabbitVNC::setSignalPassword(const QString &newSzSignalPassword)
{
    if (m_szSignalPassword == newSzSignalPassword)
        return;
    m_szSignalPassword = newSzSignalPassword;
    emit sigSignalPasswordChanged();
}

const QString &CParameterServiceRabbitVNC::getStunServer() const
{
    return m_szStunServer;
}

void CParameterServiceRabbitVNC::setStunServer(const QString &newSzStunServer)
{
    if (m_szStunServer == newSzStunServer)
        return;
    m_szStunServer = newSzStunServer;
    emit sigStunServerChanged();
}

quint16 CParameterServiceRabbitVNC::getStunPort() const
{
    return m_nStunPort;
}

void CParameterServiceRabbitVNC::setStunPort(quint16 newNStunPort)
{
    if (m_nStunPort == newNStunPort)
        return;
    m_nStunPort = newNStunPort;
    emit sigStunPortChanged();
}

const QString &CParameterServiceRabbitVNC::getTurnServer() const
{
    return m_szTurnServer;
}

void CParameterServiceRabbitVNC::setTurnServer(const QString &newSzTurnServer)
{
    if (m_szTurnServer == newSzTurnServer)
        return;
    m_szTurnServer = newSzTurnServer;
    emit sigTurnServerChanged();
}

quint16 CParameterServiceRabbitVNC::getTurnPort() const
{
    return m_nTurnPort;
}

void CParameterServiceRabbitVNC::setTurnPort(quint16 newNTurnPort)
{
    if (m_nTurnPort == newNTurnPort)
        return;
    m_nTurnPort = newNTurnPort;
    emit sigTurnPortChanged();
}

const QString &CParameterServiceRabbitVNC::getTurnUser() const
{
    return m_szTurnUser;
}

void CParameterServiceRabbitVNC::setTurnUser(const QString &newSzTurnUser)
{
    if (m_szTurnUser == newSzTurnUser)
        return;
    m_szTurnUser = newSzTurnUser;
    emit sigTurnUserChanged();
}

const QString &CParameterServiceRabbitVNC::getTurnPassword() const
{
    return m_szTurnPassword;
}

void CParameterServiceRabbitVNC::setTurnPassword(const QString &newSzTurnPassword)
{
    if (m_szTurnPassword == newSzTurnPassword)
        return;
    m_szTurnPassword = newSzTurnPassword;
    emit sigTurnPasswordChanged();
}
