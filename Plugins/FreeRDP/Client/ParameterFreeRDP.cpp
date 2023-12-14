#include "ParameterFreeRDP.h"
#include <QSettings>

CParameterFreeRDP::CParameterFreeRDP(QObject *parent) : CParameterConnecter(parent),
    m_pSettings(nullptr),
    m_nReconnectInterval(0),
    m_bShowVerifyDiaglog(true),
    m_bRedirectionPrinter(false),
    m_nRedirectionSound(RedirecionSoundType::Disable),
    m_bRedirectionMicrophone(false)
{
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
    
    m_lstDesktopSizes <<"640×480"
                      <<"800×600"
                      <<"1024×600"
                      <<"1024×768"
                      <<"1280×720"
                      <<"1280×854"
                      <<"1280×960"
                      <<"1280×1024"
                      <<"1366×768"
                      <<"1400×1050"
                      <<"1440×900"
                      <<"1600×900"
                      <<"1600×1024"
                      <<"1600×1200"
                      <<"1680×1050"
                      <<"1920×1080"
                      <<"1920×1200";
}

int CParameterFreeRDP::Load(QSettings &set)
{
    CParameterConnecter::Load(set);

    Q_ASSERT(m_pSettings);

    SetDomain(set.value("FreeRDP/Domain", GetDomain()).toString());
    UINT32 width, height, colorDepth;
    width  = set.value("FreeRDP/Width", freerdp_settings_get_uint32(
                           m_pSettings, FreeRDP_DesktopWidth)).toInt();
    height = set.value("FreeRDP/Height", freerdp_settings_get_uint32(
                           m_pSettings, FreeRDP_DesktopHeight)).toInt();
    colorDepth = set.value("FreeRDP/ColorDepth", freerdp_settings_get_uint32(
                               m_pSettings, FreeRDP_ColorDepth)).toInt();
    freerdp_settings_set_uint32(m_pSettings, FreeRDP_DesktopWidth, width);
    freerdp_settings_set_uint32(m_pSettings, FreeRDP_DesktopHeight, height);
    freerdp_settings_set_uint32(m_pSettings, FreeRDP_ColorDepth, colorDepth);
    bool bUseMultimon = set.value("FreeRDP/UseMultimon", freerdp_settings_get_bool(
                                    m_pSettings, FreeRDP_UseMultimon)).toBool();
    freerdp_settings_set_bool(m_pSettings, FreeRDP_UseMultimon, bUseMultimon);
    
    SetDesktopSizes(set.value("FreeRDP/DesktopSizes",
                              GetDesktopSizes()).toStringList());
    SetReconnectInterval(set.value("FreeRDP/ReconnectionInterval",
                                   GetReconnectInterval()).toInt());
    
    SetShowVerifyDiaglog(set.value("FreeRDP/ShowVerifyDiaglog",
                                   GetShowVerifyDiaglog()).toBool());

    SetRedirectionPrinter(set.value("FreeRDP/Redirection/Printer",
                                   GetRedirectionPrinter()).toBool());
    SetRedirectionSound(static_cast<RedirecionSoundType>(set.value("FreeRDP/Redirection/Sound",
                        static_cast<int>(GetRedirectionSound())).toInt()));
    SetRedirectionSoundParameters(set.value("FreeRDP/Redirection/Sound/Parameters",
                                   GetRedirectionSoundParameters()).toString());
    SetRedirectionMicrophone(set.value("FreeRDP/Redirection/Microphone",
                                   GetRedirectionMicrophone()).toBool());
    SetRedirectionMicrophoneParameters(set.value("FreeRDP/Redirection/Microphone/Parameters",
                                   GetRedirectionMicrophoneParameters()).toString());
    SetRedirectionDrives(set.value("FreeRDP/Redirection/Drive").toStringList());
    return 0;
}

int CParameterFreeRDP::Save(QSettings &set)
{
    CParameterConnecter::Save(set);

    set.setValue("FreeRDP/Domain", GetDomain());
    set.setValue("FreeRDP/Width", freerdp_settings_get_uint32(
                     m_pSettings, FreeRDP_DesktopWidth));
    set.setValue("FreeRDP/Height", freerdp_settings_get_uint32(
                     m_pSettings, FreeRDP_DesktopHeight));
    set.setValue("FreeRDP/ColorDepth", freerdp_settings_get_uint32(
                     m_pSettings, FreeRDP_ColorDepth));
    set.setValue("FreeRDP/UseMultimon", freerdp_settings_get_bool(
                                            m_pSettings, FreeRDP_UseMultimon));
    
    set.setValue("FreeRDP/DesktopSizes", GetDesktopSizes());
    set.setValue("FreeRDP/ReconnectionInterval", GetReconnectInterval());
    set.setValue("FreeRDP/ShowVerifyDiaglog", GetShowVerifyDiaglog());

    set.setValue("FreeRDP/Redirection/Printer", GetRedirectionPrinter());
    set.setValue("FreeRDP/Redirection/Sound", static_cast<int>(GetRedirectionSound()));
    set.setValue("FreeRDP/Redirection/Sound/Parameters", GetRedirectionSoundParameters());
    set.setValue("FreeRDP/Redirection/Microphone", GetRedirectionMicrophone());
    set.setValue("FreeRDP/Redirection/Microphone/Parameters", GetRedirectionMicrophoneParameters());
    set.setValue("FreeRDP/Redirection/Drive", GetRedirectionDrives());

    return 0;
}

const QStringList CParameterFreeRDP::GetDesktopSizes() const
{
    return m_lstDesktopSizes;
}

int CParameterFreeRDP::SetDesktopSizes(QStringList lstSize)
{
    m_lstDesktopSizes = lstSize;
    return 0;
}

UINT CParameterFreeRDP::GetReconnectInterval() const
{
    return m_nReconnectInterval;
}

void CParameterFreeRDP::SetReconnectInterval(UINT newReconnectInterval)
{
    Q_ASSERT(newReconnectInterval >= 0);
    if(newReconnectInterval)
        freerdp_settings_set_bool(m_pSettings,
                                  FreeRDP_AutoReconnectionEnabled, true);
    else
        freerdp_settings_set_bool(m_pSettings,
                                  FreeRDP_AutoReconnectionEnabled, false);

    if (m_nReconnectInterval == newReconnectInterval)
        return;

    m_nReconnectInterval = newReconnectInterval;
    freerdp_settings_set_uint32(m_pSettings,
                                FreeRDP_AutoReconnectMaxRetries,
                                m_nReconnectInterval);

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

void CParameterFreeRDP::SetClipboard(bool c)
{
    Q_ASSERT(m_pSettings);
    freerdp_settings_set_bool(m_pSettings, FreeRDP_RedirectClipboard, c);
    CParameterConnecter::SetClipboard(c);
}

void CParameterFreeRDP::SetOnlyView(bool bOnly)
{
#if FreeRDP_VERSION_MAJOR >= 3
    freerdp_settings_set_bool(m_pSettings, FreeRDP_SuspendInput, bOnly);
#endif
    return CParameterConnecter::SetOnlyView(bOnly);
}

void CParameterFreeRDP::SetHost(const QString &szHost)
{
    Q_ASSERT(m_pSettings);
    freerdp_settings_set_string(m_pSettings,
                                FreeRDP_ServerHostname,
                                szHost.toStdString().c_str());
    CParameterConnecter::SetHost(szHost);
}

void CParameterFreeRDP::SetPort(quint16 port)
{
    Q_ASSERT(m_pSettings);
    freerdp_settings_set_uint32(m_pSettings, FreeRDP_ServerPort, port);
    CParameterConnecter::SetPort(port);
}

void CParameterFreeRDP::SetUser(const QString &szUser)
{
    Q_ASSERT(m_pSettings);
    if(!szUser.isEmpty())
        freerdp_settings_set_string(m_pSettings,
                                    FreeRDP_Username,
                                    szUser.toStdString().c_str());
    CParameterConnecter::SetUser(szUser);
}

void CParameterFreeRDP::SetPassword(const QString &szPassword)
{
    Q_ASSERT(m_pSettings);
    if(!szPassword.isEmpty())
        freerdp_settings_set_string(m_pSettings,
                                    FreeRDP_Password,
                                    szPassword.toStdString().c_str());
    CParameterConnecter::SetPassword(szPassword);
}

void CParameterFreeRDP::SetDomain(const QString& szDomain)
{
    Q_ASSERT(m_pSettings);
    freerdp_settings_set_string(
        m_pSettings, FreeRDP_Domain, szDomain.toStdString().c_str());
}

const QString CParameterFreeRDP::GetDomain() const
{
    Q_ASSERT(m_pSettings);
    return freerdp_settings_get_string(m_pSettings, FreeRDP_Domain);
}
