#include "ParameterFreeRDP.h"
#include <QSettings>

CParameterFreeRDP::CParameterFreeRDP(QObject *parent) : CParameterConnecter(parent),
    m_nReconnectInterval(5),
    m_bShowVerifyDiaglog(true),
    m_bRedirectionPrinter(false),
    m_nRedirectionSound(RedirecionSoundType::Local),
    m_bRedirectionMicrophone(false)
{
    m_szRedirectionSoundParamters = 
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
    m_szRedirectionMicrophoneParamters = m_szRedirectionSoundParamters;
}

int CParameterFreeRDP::Load(QSettings &set)
{
    CParameterConnecter::Load(set);
    
    QString szDomain = set.value("FreeRDP/Domain").toString();
    freerdp_settings_set_string(
                m_pSettings, FreeRDP_Domain, szDomain.toStdString().c_str());
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

    SetReconnectInterval(set.value("FreeRDP/ReconnectionInterval",
                                   GetReconnectInterval()).toInt());
    
    SetShowVerifyDiaglog(set.value("FreeRDP/ShowVerifyDiaglog",
                                   GetShowVerifyDiaglog()).toBool());

    SetRedirectionPrinter(set.value("FreeRDP/Redirection/Printer",
                                   GetRedirectionPrinter()).toBool());
    SetRedirectionSound(static_cast<RedirecionSoundType>(set.value("FreeRDP/Redirection/Sound",
                        static_cast<int>(GetRedirectionSound())).toInt()));
    SetRedirectionSoundParamters(set.value("FreeRDP/Redirection/Sound/Paramters",
                                   GetRedirectionSoundParamters()).toString());
    SetRedirectionMicrophone(set.value("FreeRDP/Redirection/Microphone",
                                   GetRedirectionMicrophone()).toBool());
    SetRedirectionMicrophoneParamters(set.value("FreeRDP/Redirection/Microphone/Paramters",
                                   GetRedirectionMicrophoneParamters()).toString());
    
    QStringList lstDrive;
    int count = set.value("FreeRDP/Redirection/Drive/count", 0).toInt();
    for(int i = 0; i < count; i++)
    {
        QString drive = set.value("FreeRDP/Redirection/Drive/" + QString::number(i), "").toString();
        if(!drive.isEmpty())
            lstDrive.append(drive);
    }
    if(!lstDrive.isEmpty())
        SetRedirectionDrives(lstDrive);

    return 0;
}

int CParameterFreeRDP::Save(QSettings &set)
{
    CParameterConnecter::Save(set);

    set.setValue("FreeRDP/Domain", freerdp_settings_get_string(
                     m_pSettings, FreeRDP_Domain));
    set.setValue("FreeRDP/Width", freerdp_settings_get_uint32(
                     m_pSettings, FreeRDP_DesktopWidth));
    set.setValue("FreeRDP/Height", freerdp_settings_get_uint32(
                     m_pSettings, FreeRDP_DesktopHeight));
    set.setValue("FreeRDP/ColorDepth", freerdp_settings_get_uint32(
                     m_pSettings, FreeRDP_ColorDepth));

    set.setValue("FreeRDP/ReconnectionInterval", GetReconnectInterval());
    set.setValue("FreeRDP/ShowVerifyDiaglog", GetShowVerifyDiaglog());

    set.setValue("FreeRDP/Redirection/Printer", GetRedirectionPrinter());
    set.setValue("FreeRDP/Redirection/Sound", static_cast<int>(GetRedirectionSound()));
    set.setValue("FreeRDP/Redirection/Sound/Paramters", GetRedirectionSoundParamters());
    set.setValue("FreeRDP/Redirection/Microphone", GetRedirectionMicrophone());
    set.setValue("FreeRDP/Redirection/Microphone/Paramters", GetRedirectionMicrophoneParamters());

    QStringList lstDrive = GetRedirectionDrives();
    if(!lstDrive.isEmpty()) {
        set.setValue("FreeRDP/Redirection/Drive/count", lstDrive.size());
        for(int i = 0; i < lstDrive.size(); i++)
            set.setValue("FreeRDP/Redirection/Drive/" + QString::number(i),
                      lstDrive[i]);
    }

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
    if(m_nReconnectInterval)
        freerdp_settings_set_bool(m_pSettings,
                                  FreeRDP_AutoReconnectionEnabled, true);
    else
        freerdp_settings_set_bool(m_pSettings,
                                  FreeRDP_AutoReconnectionEnabled, false);
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

const QString &CParameterFreeRDP::GetRedirectionSoundParamters() const
{
    return m_szRedirectionSoundParamters;
}

void CParameterFreeRDP::SetRedirectionSoundParamters(const QString &newRedirectionSoundParamters)
{
    if (m_szRedirectionSoundParamters == newRedirectionSoundParamters)
        return;
    m_szRedirectionSoundParamters = newRedirectionSoundParamters;
    SetModified(true);
    emit sigRedirectionSoundParamtersChanged();
}

const QString &CParameterFreeRDP::GetRedirectionMicrophoneParamters() const
{
    return m_szRedirectionMicrophoneParamters;
}

void CParameterFreeRDP::SetRedirectionMicrophoneParamters(const QString &newRedirectionMicrophoneParamters)
{
    if (m_szRedirectionMicrophoneParamters == newRedirectionMicrophoneParamters)
        return;
    m_szRedirectionMicrophoneParamters = newRedirectionMicrophoneParamters;
    SetModified(true);
    emit sigRedirectionMicrophoneParamtersChanged();
}
