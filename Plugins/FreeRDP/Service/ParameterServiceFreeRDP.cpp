// Author: Kang Lin <kl222@126.com>

#include "ParameterServiceFreeRDP.h"
#include <QSettings>

CParameterServiceFreeRDP::CParameterServiceFreeRDP(QObject *parent)
    : CParameterService(parent),
      m_bTlsSecurity(true),
      m_bRdpSecurity(true),
      m_bNlaSecurity(false),
      m_bNlaExtSecurity(false),
      #ifdef WITH_SHADOW_X11
      m_bAuthentication(true),
      #else
      m_bAuthentication(false),
      #endif
      m_MayView(true),
      m_MayInteract(true)
{
    setPort(3389);
}

bool CParameterServiceFreeRDP::getTlsSecurity() const
{
    return m_bTlsSecurity;
}

void CParameterServiceFreeRDP::setTlsSecurity(bool newTlsSecurity)
{
    if (m_bTlsSecurity == newTlsSecurity)
        return;
    m_bTlsSecurity = newTlsSecurity;
    emit sigTlsSecurityChanged();
}

bool CParameterServiceFreeRDP::getRdpSecurity() const
{
    return m_bRdpSecurity;
}

void CParameterServiceFreeRDP::setRdpSecurity(bool newRdpSecurity)
{
    if (m_bRdpSecurity == newRdpSecurity)
        return;
    m_bRdpSecurity = newRdpSecurity;
    emit sigRdpSecurityChanged();
}

bool CParameterServiceFreeRDP::getNlaSecurity() const
{
    return m_bNlaSecurity;
}

void CParameterServiceFreeRDP::setNlaSecurity(bool newNlaSecurity)
{
    if (m_bNlaSecurity == newNlaSecurity)
        return;
    m_bNlaSecurity = newNlaSecurity;
    emit sigNlaecurityChanged();
}

bool CParameterServiceFreeRDP::getNlaExtSecurity() const
{
    return m_bNlaExtSecurity;
}

void CParameterServiceFreeRDP::setNlaExtSecurity(bool newNlaExtSecurity)
{
    if (m_bNlaExtSecurity == newNlaExtSecurity)
        return;
    m_bNlaExtSecurity = newNlaExtSecurity;
    emit sigNlaExtSecurityChanged();
}

bool CParameterServiceFreeRDP::getAuthentication() const
{
    return m_bAuthentication;
}

void CParameterServiceFreeRDP::setAuthentication(bool newAuthentication)
{
    if (m_bAuthentication == newAuthentication)
        return;
    m_bAuthentication = newAuthentication;
    emit sigAuthenticationChanged();
}

bool CParameterServiceFreeRDP::getMayView() const
{
    return m_MayView;
}

void CParameterServiceFreeRDP::setMayView(bool newMayView)
{
    if (m_MayView == newMayView)
        return;
    m_MayView = newMayView;
    emit sigMayViewChanged();
}

bool CParameterServiceFreeRDP::getMayInteract() const
{
    return m_MayInteract;
}

void CParameterServiceFreeRDP::setMayInteract(bool newMayInteract)
{
    if (m_MayInteract == newMayInteract)
        return;
    m_MayInteract = newMayInteract;
    emit sigMayInteractChanged();
}

const QString &CParameterServiceFreeRDP::getSamFile() const
{
    return m_szSamFile;
}

void CParameterServiceFreeRDP::setSamFile(const QString &newSamFile)
{
    if (m_szSamFile == newSamFile)
        return;
    m_szSamFile = newSamFile;
    emit sigSamFileChanged();
}

int CParameterServiceFreeRDP::Load(const QString &szFile)
{
    QSettings set(szFile, QSettings::IniFormat);
    
    setTlsSecurity(set.value("Security/Tls", getTlsSecurity()).toBool());
    setRdpSecurity(set.value("Security/Rdp", getRdpSecurity()).toBool());
    setNlaSecurity(set.value("Security/Nla", getNlaSecurity()).toBool());
    setNlaExtSecurity(set.value("Security/NlaExt", getNlaExtSecurity()).toBool());
    setSamFile(set.value("Security/Nla/SamFIle", getSamFile()).toString());
    
    setAuthentication(set.value("Server/Authentication", getAuthentication()).toBool());
    setMayView(set.value("Server/MayView", getMayView()).toBool());
    setMayInteract(set.value("Server/MayInteract", getMayInteract()).toBool());
    return CParameterService::Load(szFile);
}

int CParameterServiceFreeRDP::Save(const QString &szFile)
{
    QSettings set(szFile, QSettings::IniFormat);
    
    set.setValue("Security/Tls", getTlsSecurity());
    set.setValue("Security/Rdp", getRdpSecurity());
    set.setValue("Security/Nla", getNlaSecurity());
    set.setValue("Security/NlaExt", getNlaExtSecurity());
    set.setValue("Security/Nla/SamFIle", getSamFile());

    set.setValue("Server/Authentication", getAuthentication());
    set.setValue("Server/MayView", getMayView());
    set.setValue("Server/MayInteract", getMayInteract());
    
    return CParameterService::Save(szFile);
}
