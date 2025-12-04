#include "ParameterMediaDevices.h"

CParameterMediaDevices::CParameterMediaDevices(QObject *parent, const QString &szPrefix)
    : CParameter{parent, szPrefix}
{}

CParameterMediaDevices& CParameterMediaDevices::operator=(const CParameterMediaDevices &devices)
{
    m_Para = devices.m_Para;
    return *this;
}

int CParameterMediaDevices::OnLoad(QSettings &set)
{
    set.beginGroup("Media/Devices");
    m_Para.m_Camera = set.value("Camera", m_Para.m_Camera).toByteArray();
    m_Para.m_AudioInput = set.value("Audio/Input", m_Para.m_AudioInput).toByteArray();
    m_Para.m_AudioOutput = set.value("Audio/Output", m_Para.m_AudioOutput).toByteArray();
    m_Para.m_VolumeInput = set.value("Audio/Input/Volume", m_Para.m_VolumeInput).toReal();
    m_Para.m_VolumeOutput = set.value("Audio/Output/Volume", m_Para.m_VolumeOutput).toReal();
    set.endGroup();
    return 0;
}

int CParameterMediaDevices::OnSave(QSettings &set)
{
    set.beginGroup("Media/Devices");
    set.setValue("Camera",  m_Para.m_Camera);
    set.setValue("Audio/Input", m_Para.m_AudioInput);
    set.setValue("Audio/Output", m_Para.m_AudioOutput);
    set.setValue("Audio/Input/Volume", m_Para.m_VolumeInput);
    set.setValue("Audio/Output/Volume", m_Para.m_VolumeOutput);
    set.endGroup();
    return 0;
}
