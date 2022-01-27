#include "ParameterFreeRDP.h"
#include <QSettings>

CParameterFreeRDP::CParameterFreeRDP(QObject *parent) : CParameter(parent),
    m_nReconnectInterval(5)
{}

int CParameterFreeRDP::Load(QSettings &set)
{
    CParameter::Load(set);
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
    return 0;    
}

int CParameterFreeRDP::Save(QSettings &set)
{
    CParameter::Save(set);
    set.setValue("FreeRDP/Domain", freerdp_settings_get_string(
                     m_pSettings, FreeRDP_Domain));
    set.setValue("FreeRDP/Width", freerdp_settings_get_uint32(
                     m_pSettings, FreeRDP_DesktopWidth));
    set.setValue("FreeRDP/Height", freerdp_settings_get_uint32(
                     m_pSettings, FreeRDP_DesktopHeight));
    set.setValue("FreeRDP/ColorDepth", freerdp_settings_get_uint32(
                     m_pSettings, FreeRDP_ColorDepth));
    set.setValue("FreeRDP/ReconnectionInterval", GetReconnectInterval());
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
    emit sigReconnectIntervalChanged();
}
