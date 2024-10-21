// Author: Kang Lin <kl222@126.com>

#include "ParameterPlayer.h"

CParameterPlayer::CParameterPlayer(QObject *parent)
    : CParameterBase{parent}
    , m_Type(TYPE::Camera)
    , m_nCamera(-1)
    , m_nAudioInput(-1)
    , m_bEnableAudioInput(true)
    , m_bAudioInputMuted(false)
    , m_fAudioInputVolume(100)
    , m_nAudioOutput(-1)
    , m_bEnableAudioOutput(true)
    , m_bAudioOutputMuted(false)
    , m_fAudioOutputVolume(100)
{}

const CParameterPlayer::TYPE CParameterPlayer::GetType() const
{
    return m_Type;
}

int CParameterPlayer::SetType(TYPE type)
{
    if(m_Type == type)
        return 0;
    m_Type = type;
    SetModified(true);
    return 0;
}

const QString CParameterPlayer::GetUrl() const
{
    return m_szUrl;
}

int CParameterPlayer::SetUrl(const QString& szUrl)
{
    if(m_szUrl == szUrl)
        return 0;
    SetModified(true);
    m_szUrl = szUrl;
    return 0;
}

const int CParameterPlayer::GetCamera() const
{
    return m_nCamera;
}

int CParameterPlayer::SetCamera(int nIndex)
{
    if(m_nCamera == nIndex)
        return 0;
    m_nCamera = nIndex;
    SetModified(true);
    return 0;
}

const int CParameterPlayer::GetAudioInput() const
{
    return m_nAudioInput;
}

int CParameterPlayer::SetAudioInput(int nIndex)
{
    if(m_nAudioInput == nIndex)
        return 0;
    m_nAudioInput = nIndex;
    SetModified(true);
    emit sigAudioInput(m_nAudioInput);
    return 0;
}

const bool CParameterPlayer::GetEnableAudioInput() const
{
    return m_bEnableAudioInput;
}

int CParameterPlayer::SetEnableAudioInput(bool bEnable)
{
    if(m_bEnableAudioInput == bEnable)
        return 0;
    m_bEnableAudioInput = bEnable;
    SetModified(true);
    emit sigEnableAudioInput(m_bEnableAudioInput);
    return 0;
}

const bool CParameterPlayer::GetAudioInputMuted() const
{
    return m_bAudioInputMuted;
}

int CParameterPlayer::SetAudioInputMuted(bool bMuted)
{
    if(m_bAudioInputMuted == bMuted)
        return 0;
    m_bAudioInputMuted = bMuted;
    SetModified(true);
    emit sigAudioInputMuted(m_bAudioInputMuted);
    return 0;
}

const float CParameterPlayer::GetAudioInputVolume() const
{
    return m_fAudioInputVolume;
}

int CParameterPlayer::SetAudioInputVolume(float fVolume)
{
    if(m_fAudioInputVolume == fVolume)
        return 0;
    m_fAudioInputVolume = fVolume;
    SetModified(true);
    emit sigAudioInputVolume(m_fAudioInputVolume);
    return 0;
}

const int CParameterPlayer::GetAudioOutput() const
{
    return m_nAudioOutput;
}

int CParameterPlayer::SetAudioOutput(int nIndex)
{
    if(m_nAudioOutput == nIndex)
        return 0;
    m_nAudioOutput = nIndex;
    SetModified(true);
    emit sigAudioOutput(m_nAudioOutput);
    return 0;
}

const bool CParameterPlayer::GetEnableAudioOutput() const
{
    return m_bEnableAudioOutput;
}

int CParameterPlayer::SetEnableAudioOutput(bool bEnable)
{
    if(m_bEnableAudioOutput == bEnable)
        return 0;
    m_bEnableAudioOutput = bEnable;
    SetModified(true);
    emit sigEnableAudioOutput(bEnable);
    return 0;
}

const bool CParameterPlayer::GetAudioOutputMuted() const
{
    return m_bAudioOutputMuted;
}

int CParameterPlayer::SetAudioOutputMuted(bool bMuted)
{
    if(m_bAudioOutputMuted == bMuted)
        return 0;
    m_bAudioOutputMuted = bMuted;
    SetModified(true);
    emit sigAudioOutputMuted(m_bAudioOutputMuted);
    return 0;
}

const float CParameterPlayer::GetAudioOutputVolume() const
{
    return m_fAudioOutputVolume;
}

int CParameterPlayer::SetAudioOutputVolume(float fVolume)
{
    if(m_fAudioOutputVolume == fVolume)
        return 0;
    m_fAudioOutputVolume = fVolume;
    SetModified(true);
    emit sigAudioOutputVolume(m_fAudioOutputVolume);
    return 0;
}
int CParameterPlayer::OnLoad(QSettings &set)
{
    set.beginGroup("Player");
    SetType((TYPE)set.value("Type", (int)GetType()).toInt());
    SetUrl(set.value("Url", GetUrl()).toString());
    SetCamera(set.value("Camera", GetCamera()).toInt());

    set.beginGroup("Audio/Input");
    SetAudioInput(set.value("Device", GetAudioInput()).toInt());
    SetEnableAudioInput(set.value("Enable", GetEnableAudioInput()).toBool());
    SetAudioInputMuted(set.value("Muted", GetAudioInputMuted()).toBool());
    SetAudioInputVolume(set.value("Volume", GetAudioInputVolume()).toBool());
    set.endGroup();

    set.beginGroup("Audio/Output");
    SetAudioOutput(set.value("Device", GetAudioOutput()).toInt());
    SetEnableAudioOutput(set.value("Enable", GetEnableAudioOutput()).toBool());
    SetAudioOutputMuted(set.value("Muted", GetAudioOutputMuted()).toBool());
    SetAudioOutputVolume(set.value("Volume", GetAudioOutputVolume()).toFloat());
    set.endGroup();

    set.endGroup();
    return 0;
}

int CParameterPlayer::OnSave(QSettings &set)
{
    set.beginGroup("Player");
    set.setValue("Type", (int)GetType());
    set.setValue("Url", GetUrl());
    set.setValue("Camera", GetCamera());

    set.beginGroup("Audio/Input");
    set.setValue("Device", GetAudioInput());
    set.setValue("Enable", GetEnableAudioInput());
    set.setValue("Muted", GetAudioInputMuted());
    set.setValue("Volume", GetAudioInputVolume());
    set.endGroup();

    set.beginGroup("Audio/Output");
    set.setValue("Device", GetAudioOutput());
    set.setValue("Enable", GetEnableAudioOutput());
    set.setValue("Muted", GetAudioOutputMuted());
    set.setValue("Volume", GetAudioOutputVolume());
    set.endGroup();

    set.endGroup();
    return 0;
}
