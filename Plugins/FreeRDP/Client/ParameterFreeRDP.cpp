#include "ParameterFreeRDP.h"

#if HAVE_OPENSSL
#include <openssl/tls1.h>
#endif

#include <QSettings>
#include <QScreen>
#include <QApplication>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "FreeRDP.Parameter")
CParameterFreeRDP::CParameterFreeRDP(QObject *parent)
    : CParameterBase(parent),
    m_nWidth(GetScreenGeometry().width()),
    m_nHeight(GetScreenGeometry().height()),
    m_nColorDepth(32),
    m_bUseMultimon(false),
    m_nReconnectInterval(0),
    m_bShowVerifyDiaglog(true),
    m_bRedirectionPrinter(false),
    m_nRedirectionSound(RedirecionSoundType::Disable),
    m_bRedirectionMicrophone(false),
    m_bNegotiateSecurityLayer(true),
    m_Security((Security)(Security::RDP|Security::TLS|Security::NLA)),
    m_tlsVersion(TLS1_VERSION),
    m_ConnectType(CONNECTION_TYPE_AUTODETECT),
    m_PerformanceFlags(PERF_FLAG_NONE)
{
    m_Net.SetPort(3389);
    
    m_Proxy.SetType(
        QList<CParameterProxy::TYPE>() << CParameterProxy::TYPE::None
                                       << CParameterProxy::TYPE::SockesV5
                                       << CParameterProxy::TYPE::Http
                                       << CParameterProxy::TYPE::SSHTunnel);
    
    m_szRedirectionSoundParameters = 
        #if defined (Q_OS_WINDOWS) || defined(Q_OS_WIN) || defined(Q_OS_WIN32) || defined(Q_OS_WINRT)
            "sys:winmm"
        #elif defined(Q_OS_IOS)
            "sys:ios"
        #elif defined (Q_OS_ANDROID)
            "sys:opensles" 
        #elif defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
            "sys:alsa"
        #endif
            ;
    m_szRedirectionMicrophoneParameters = m_szRedirectionSoundParameters;
}

int CParameterFreeRDP::OnLoad(QSettings &set)
{
    CParameterBase::OnLoad(set);
    
    set.beginGroup("FreeRDP");
    SetDomain(set.value("Domain", GetDomain()).toString());
    SetDesktopWidth(set.value("Width", GetDesktopWidth()).toInt());
    SetDesktopHeight(set.value("Height", GetDesktopHeight()).toInt());
    SetColorDepth(set.value("ColorDepth", GetColorDepth()).toInt());
    SetUseMultimon(set.value("UseMultimon", GetUseMultimon()).toBool());
    SetReconnectInterval(set.value("ReconnectionInterval",
                                   GetReconnectInterval()).toInt());
    
    SetShowVerifyDiaglog(set.value("ShowVerifyDiaglog",
                                   GetShowVerifyDiaglog()).toBool());

    SetRedirectionPrinter(set.value("Redirection/Printer",
                                   GetRedirectionPrinter()).toBool());
    SetRedirectionSound(static_cast<RedirecionSoundType>(set.value("Redirection/Sound",
                        static_cast<int>(GetRedirectionSound())).toInt()));
    SetRedirectionSoundParameters(set.value("Redirection/Sound/Parameters",
                                   GetRedirectionSoundParameters()).toString());
    SetRedirectionMicrophone(set.value("Redirection/Microphone",
                                   GetRedirectionMicrophone()).toBool());
    SetRedirectionMicrophoneParameters(set.value("Redirection/Microphone/Parameters",
                                   GetRedirectionMicrophoneParameters()).toString());
    SetRedirectionDrives(set.value("Redirection/Drive").toStringList());
    
    SetNegotiateSecurityLayer(set.value("Security/Enable", GetNegotiateSecurityLayer()).toBool());
    SetSecurity((Security)set.value("Security/Type", GetSecurity()).toUInt());
    SetTlsVersion(set.value("Security/Tls/Version", GetTlsVersion()).toUInt());

    set.beginGroup("Performance");
    SetConnectType(set.value("Connect/Type", GetConnectType()).toUInt());
    SetPerformanceFlags(set.value("Flags", GetPerformanceFlags()).toUInt());
    set.endGroup();

    set.endGroup();

    return 0;
}

int CParameterFreeRDP::OnSave(QSettings &set)
{
    CParameterBase::OnSave(set);

    set.beginGroup("FreeRDP");
    set.setValue("Domain", GetDomain());
    set.setValue("Width", GetDesktopWidth());
    set.setValue("Height", GetDesktopHeight());
    set.setValue("ColorDepth", GetColorDepth());
    set.setValue("UseMultimon", GetUseMultimon());

    set.setValue("ReconnectionInterval", GetReconnectInterval());
    set.setValue("ShowVerifyDiaglog", GetShowVerifyDiaglog());

    set.setValue("Redirection/Printer", GetRedirectionPrinter());
    set.setValue("Redirection/Sound", static_cast<int>(GetRedirectionSound()));
    set.setValue("Redirection/Sound/Parameters", GetRedirectionSoundParameters());
    set.setValue("Redirection/Microphone", GetRedirectionMicrophone());
    set.setValue("Redirection/Microphone/Parameters", GetRedirectionMicrophoneParameters());
    set.setValue("Redirection/Drive", GetRedirectionDrives());

    set.setValue("Security/Enable", GetNegotiateSecurityLayer());
    set.setValue("Security/Type", (uint)GetSecurity());
    set.setValue("Security/Tls/Version", GetTlsVersion());

    set.beginGroup("Performance");
    set.setValue("Connect/Type", GetConnectType());
    set.setValue("Flags", GetPerformanceFlags());
    set.endGroup();

    set.endGroup();

    return 0;
}

UINT32 CParameterFreeRDP::GetDesktopWidth() const
{
    return m_nWidth;
}

int CParameterFreeRDP::SetDesktopWidth(UINT32 nWidth)
{
    if(m_nWidth == nWidth)
        return 0;
    m_nWidth = nWidth;
    SetModified(true);
    return 0;
}

UINT32 CParameterFreeRDP::GetDesktopHeight() const
{
    return m_nHeight;
}

int CParameterFreeRDP::SetDesktopHeight(UINT32 nHeight)
{
    if(m_nHeight == nHeight)
        return 0;
    m_nHeight = nHeight;
    SetModified(true);
    return 0;
}

UINT32 CParameterFreeRDP::GetColorDepth() const
{
    return m_nColorDepth;
}

int CParameterFreeRDP::SetColorDepth(UINT32 color)
{
    if(m_nColorDepth == color)
        return 0;
    m_nColorDepth = color;
    SetModified(true);
    return 0;
}

bool CParameterFreeRDP::GetUseMultimon() const
{
    return m_bUseMultimon;
}

int CParameterFreeRDP::SetUseMultimon(bool bUse)
{
    if(m_bUseMultimon == bUse)
        return 0;
    m_bUseMultimon = bUse;
    SetModified(true);
    return 0;
}

UINT CParameterFreeRDP::GetReconnectInterval() const
{
    return m_nReconnectInterval;
}

void CParameterFreeRDP::SetReconnectInterval(UINT newReconnectInterval)
{
    if (m_nReconnectInterval == newReconnectInterval)
        return;

    m_nReconnectInterval = newReconnectInterval;
    SetModified(true);
    emit sigReconnectIntervalChanged();
}

bool CParameterFreeRDP::GetShowVerifyDiaglog() const
{
    return m_bShowVerifyDiaglog;
}

void CParameterFreeRDP::SetShowVerifyDiaglog(bool bShow)
{
    if(m_bShowVerifyDiaglog == bShow)
        return;
    m_bShowVerifyDiaglog = bShow;
    SetModified(true);
}

CParameterFreeRDP::RedirecionSoundType CParameterFreeRDP::GetRedirectionSound() const
{
    return m_nRedirectionSound;
}

void CParameterFreeRDP::SetRedirectionSound(RedirecionSoundType newRedirectionSound)
{
    if (m_nRedirectionSound == newRedirectionSound)
        return;
    m_nRedirectionSound = newRedirectionSound;
    SetModified(true);
    emit sigRedirectionSoundChanged(m_nRedirectionSound);
}

bool CParameterFreeRDP::GetRedirectionMicrophone() const
{
    return m_bRedirectionMicrophone;
}

void CParameterFreeRDP::SetRedirectionMicrophone(bool newRedirectionMicrophone)
{
    if (m_bRedirectionMicrophone == newRedirectionMicrophone)
        return;
    m_bRedirectionMicrophone = newRedirectionMicrophone;
    SetModified(true);
    emit sigRedirectionMicrophoneChanged(m_bRedirectionMicrophone);
}

QStringList CParameterFreeRDP::GetRedirectionDrives() const
{
    return m_lstRedirectionDrives;
}

void CParameterFreeRDP::SetRedirectionDrives(const QStringList &newRedirectionDrive)
{
    if (m_lstRedirectionDrives == newRedirectionDrive)
        return;
    m_lstRedirectionDrives = newRedirectionDrive;
    SetModified(true);
    emit sigRedirectionDrivesChanged(m_lstRedirectionDrives);
}

bool CParameterFreeRDP::GetRedirectionPrinter() const
{
    return m_bRedirectionPrinter;
}

void CParameterFreeRDP::SetRedirectionPrinter(bool newRedirectionPrinter)
{
    if (m_bRedirectionPrinter == newRedirectionPrinter)
        return;
    m_bRedirectionPrinter = newRedirectionPrinter;
    SetModified(true);
    emit sigRedirectionPrinterChanged(m_bRedirectionPrinter);
}

const QString &CParameterFreeRDP::GetRedirectionSoundParameters() const
{
    return m_szRedirectionSoundParameters;
}

void CParameterFreeRDP::SetRedirectionSoundParameters(const QString &newRedirectionSoundParameters)
{
    if (m_szRedirectionSoundParameters == newRedirectionSoundParameters)
        return;
    m_szRedirectionSoundParameters = newRedirectionSoundParameters;
    SetModified(true);
    emit sigRedirectionSoundParametersChanged();
}

const QString &CParameterFreeRDP::GetRedirectionMicrophoneParameters() const
{
    return m_szRedirectionMicrophoneParameters;
}

void CParameterFreeRDP::SetRedirectionMicrophoneParameters(const QString &newRedirectionMicrophoneParameters)
{
    if (m_szRedirectionMicrophoneParameters == newRedirectionMicrophoneParameters)
        return;
    m_szRedirectionMicrophoneParameters = newRedirectionMicrophoneParameters;
    SetModified(true);
    emit sigRedirectionMicrophoneParametersChanged();
}

UINT16 CParameterFreeRDP::GetTlsVersion() const
{
    return m_tlsVersion;
}

void CParameterFreeRDP::SetTlsVersion(UINT16 newTlsVersion)
{
    if(m_tlsVersion == newTlsVersion)
        return;
    m_tlsVersion = newTlsVersion;
    SetModified(true);
}

UINT32 CParameterFreeRDP::GetConnectType() const
{
    return m_ConnectType;
}

void CParameterFreeRDP::SetConnectType(UINT32 newConnectType)
{
    if(m_ConnectType == newConnectType)
        return;
    m_ConnectType = newConnectType;
    SetModified(true);
}

UINT32 CParameterFreeRDP::GetPerformanceFlags() const
{
    return m_PerformanceFlags;
}

void CParameterFreeRDP::SetPerformanceFlags(UINT32 newPerformanceFlags)
{
    if(m_PerformanceFlags == newPerformanceFlags)
        return;
    m_PerformanceFlags = newPerformanceFlags;
    SetModified(true);
}

bool CParameterFreeRDP::GetNegotiateSecurityLayer() const
{
    return m_bNegotiateSecurityLayer;
}

void CParameterFreeRDP::SetNegotiateSecurityLayer(bool newNegotiateSecurityLayer)
{
    if(m_bNegotiateSecurityLayer == newNegotiateSecurityLayer)
        return;
    m_bNegotiateSecurityLayer = newNegotiateSecurityLayer;
    SetModified(true);
}

CParameterFreeRDP::Security CParameterFreeRDP::GetSecurity() const
{
    return m_Security;
}

void CParameterFreeRDP::SetSecurity(Security newSecurity)
{
    if(m_Security == newSecurity)
        return;
    m_Security = newSecurity;
    SetModified(true);
}

void CParameterFreeRDP::SetDomain(const QString& szDomain)
{
    if(m_szDomain == szDomain)
        return;
    m_szDomain = szDomain;
    SetModified(true);
}

const QString CParameterFreeRDP::GetDomain() const
{
    return m_szDomain;
}

QRect CParameterFreeRDP::GetScreenGeometry()
{
    QRect r;
    QScreen* pScreen = QApplication::primaryScreen();
    r = pScreen->geometry();
    return r;
}
