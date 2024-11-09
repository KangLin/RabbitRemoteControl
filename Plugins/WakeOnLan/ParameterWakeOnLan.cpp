// Author: Kang Lin <kl222@126.com>

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QRegExpValidator>
#include <QRegExp>
#else
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#endif
#include "ParameterWakeOnLan.h"

CParameterWakeOnLan::CParameterWakeOnLan(QObject *parent)
    : CParameterConnecter(parent)
    , m_Net(this)
    , m_bEnable(false)
    , m_HostState(HostState::Offline)
    , m_szBoardcastAddress("255.255.255.255")
    , m_nPort(9)
    , m_bSavePassword(false)
    , m_nRepeat(3)
    , m_nInterval(500)
    , m_nTimeOut(1000)
{}

int CParameterWakeOnLan::OnLoad(QSettings &set)
{
    int nRet = 0;
    set.beginGroup("WakeOnLan");
    SetEnable(set.value("Enable", GetEnable()).toBool());
    SetMac(set.value("Mac", GetMac()).toString());
    SetBroadcastAddress(
        set.value("BoardAddress", GetBroadcastAddress()).toString());
    SetNetworkInterface(set.value("NetworkInterface", GetNetworkInterface()).toString());
    SetPort(set.value("Port", GetPort()).toUInt());
    SetPassword(set.value("Password", GetPassword()).toString());
    SetSavePassword(set.value("Password/Save", GetSavePassword()).toBool());
    SetRepeat(set.value("Repeat", GetRepeat()).toInt());
    SetInterval(set.value("Interval", GetInterval()).toInt());
    SetTimeOut(set.value("TimeOut", GetTimeOut()).toInt());
    set.endGroup();
    return nRet;
}

int CParameterWakeOnLan::OnSave(QSettings &set)
{
    int nRet = 0;
    set.beginGroup("WakeOnLan");
    set.setValue("Enable", GetEnable());
    set.setValue("Mac", GetMac());
    set.setValue("BoardAddress", GetBroadcastAddress());
    set.setValue("NetworkInterface", GetNetworkInterface());
    set.setValue("Port", GetPort());
    set.setValue("Password", GetPassword());
    set.setValue("Password/Save", GetSavePassword());
    set.setValue("Repeat", GetRepeat());
    set.setValue("Interval", GetInterval());
    set.setValue("TimeOut", GetTimeOut());
    set.endGroup();
    return nRet;
}

bool CParameterWakeOnLan::OnCheckValidity()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp rxMac("^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegExpValidator macValidator(rxMac, this);
    QRegExp rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegExpValidator ipValidator(rxIP, this);
    QRegExp rxPassword("(.{6})?|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegExpValidator passwordValidator(rxPassword, this);
#else
    QRegularExpression rxMac("^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegularExpressionValidator macValidator(rxMac, this);
    QRegularExpression rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegularExpressionValidator ipValidator(rxIP, this);
    QRegularExpression rxPassword("(.{6})?|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegularExpressionValidator passwordValidator(rxPassword, this);
#endif
    if(!GetEnable())
        return false;
    QString szMac = GetMac();
    int pos = 0;
    if(QValidator::Acceptable != macValidator.validate(szMac, pos))
        return false;
    QString szBroadAddress = GetBroadcastAddress();
    if(QValidator::Acceptable != ipValidator.validate(szBroadAddress, pos))
        return false;
    QString szPassword = GetPassword();
    if(QValidator::Acceptable != passwordValidator.validate(szPassword, pos))
        return false;

    return true;
}

void CParameterWakeOnLan::slotSetParameterClient()
{
    if(GetParameterClient())
    {
        SetSavePassword(GetParameterClient()->GetSavePassword());
    }
}

const bool CParameterWakeOnLan::GetEnable() const
{
    return m_bEnable;
}

int CParameterWakeOnLan::SetEnable(bool bEnable)
{
    if(m_bEnable == bEnable)
        return 0;
    m_bEnable = bEnable;
    SetModified(true);
    return 0;
}

const QString CParameterWakeOnLan::GetMac() const
{
    return m_szMac;
}

int CParameterWakeOnLan::SetMac(const QString &szMac)
{
    if(m_szMac == szMac)
        return 0;
    m_szMac = szMac;
    SetModified(true);
    return 0;
}

const QString CParameterWakeOnLan::GetBroadcastAddress() const
{
    return m_szBoardcastAddress;
}

int CParameterWakeOnLan::SetBroadcastAddress(const QString &szBroadcastAddress)
{
    if(m_szBoardcastAddress == szBroadcastAddress)
        return 0;
    m_szBoardcastAddress = szBroadcastAddress;
    SetModified(true);
    return 0;
}

const QString CParameterWakeOnLan::GetNetworkInterface() const
{
    return m_szNetworkInteface;
}

int CParameterWakeOnLan::SetNetworkInterface(const QString& szName)
{
    if(m_szNetworkInteface == szName)
        return 0;
    m_szNetworkInteface = szName;
    SetModified(true);
    return 0;
}

const quint16 CParameterWakeOnLan::GetPort() const
{
    return m_nPort;
}

int CParameterWakeOnLan::SetPort(quint16 port)
{
    if(m_nPort == port)
        return 0;
    m_nPort = port;
    SetModified(true);
    return 0;
}

const QString CParameterWakeOnLan::GetPassword() const
{
    return m_szPassword;
}

int CParameterWakeOnLan::SetPassword(const QString &szPassword)
{
    if(m_szPassword == szPassword)
        return 0;
    m_szPassword = szPassword;
    SetModified(true);
    return 0;
}

const bool CParameterWakeOnLan::GetSavePassword() const
{
    return m_bSavePassword;
}

int CParameterWakeOnLan::SetSavePassword(bool save)
{
    if(m_bSavePassword == save)
        return 0;
    m_bSavePassword = save;
    SetModified(true);
    return 0;
}

const int CParameterWakeOnLan::GetRepeat() const
{
    return m_nRepeat;
}

int CParameterWakeOnLan::SetRepeat(int nRepeat)
{
    if(m_nRepeat == nRepeat)
        return 0;
    m_nRepeat = nRepeat;
    SetModified(true);
    return 0;
}

const int CParameterWakeOnLan::GetInterval() const
{
    return m_nInterval;
}

int CParameterWakeOnLan::SetInterval(int nInterval)
{
    if(m_nInterval == nInterval)
        return 0;
    m_nInterval = nInterval;
    SetModified(true);
    return 0;
}

const int CParameterWakeOnLan::GetTimeOut() const
{
    return m_nTimeOut;
}

int CParameterWakeOnLan::SetTimeOut(int nTimeout)
{
    if(m_nTimeOut == nTimeout)
        return 0;
    m_nTimeOut = nTimeout;
    SetModified(true);
    return 0;
}

CParameterWakeOnLan::HostState CParameterWakeOnLan::GetHostState() const
{
    return m_HostState;
}

void CParameterWakeOnLan::SetHostState(HostState newHostState)
{
    if (m_HostState == newHostState)
        return;
    m_HostState = newHostState;
    emit sigHostStateChanged();
}
