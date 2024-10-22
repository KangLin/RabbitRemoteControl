// Author: Kang Lin <kl222@126.com>

#ifndef PARAMETERPLAYER_H
#define PARAMETERPLAYER_H

#include "ParameterBase.h"
#include "ParameterRecord.h"

class CParameterPlayer : public CParameterBase
{
    Q_OBJECT
public:
    explicit CParameterPlayer(QObject *parent = nullptr);

    enum class TYPE {
        Camera,
        Url
    };
    Q_ENUM(TYPE)
    const TYPE GetType() const;
    int SetType(TYPE type);

    const QString GetUrl() const;
    int SetUrl(const QString &szUrl);

    const int GetCamera() const;
    int SetCamera(int nIndex);

    const int GetAudioInput() const;
    int SetAudioInput(int nIndex);
Q_SIGNALS:
    void sigAudioInput(int nIndex);

public:
    const bool GetEnableAudioInput() const;
    int SetEnableAudioInput(bool bEnable);
Q_SIGNALS:
    void sigEnableAudioInput(bool bEnable);

public:
    const bool GetAudioInputMuted() const;
    int SetAudioInputMuted(bool bMuted);
Q_SIGNALS:
    void sigAudioInputMuted(bool bMuted);

public:
    const float GetAudioInputVolume() const;
    int SetAudioInputVolume(float fVolume);
Q_SIGNALS:
    void sigAudioInputVolume(float fVolume);

public:
    const int GetAudioOutput() const;
    int SetAudioOutput(int nIndex);
Q_SIGNALS:
    void sigAudioOutput(int nIndex);

public:
    const bool GetEnableAudioOutput() const;
    int SetEnableAudioOutput(bool bEnable);
Q_SIGNALS:
    void sigEnableAudioOutput(bool bEnable);

public:
    const bool GetAudioOutputMuted() const;
    int SetAudioOutputMuted(bool bMuted);
Q_SIGNALS:
    void sigAudioOutputMuted(bool bMuted);

public:
    const float GetAudioOutputVolume() const;
    int SetAudioOutputVolume(float fVolume);
Q_SIGNALS:
    void sigAudioOutputVolume(float fVolume);

private:
    TYPE m_Type;
    QString m_szUrl;
    int m_nCamera;
    int m_nAudioInput;
    bool m_bEnableAudioInput;
    bool m_bAudioInputMuted;
    float m_fAudioInputVolume;
    int m_nAudioOutput;
    bool m_bEnableAudioOutput;
    bool m_bAudioOutputMuted;
    float m_fAudioOutputVolume;

public:
    const int GetScreen() const;
    int SetScreen(int nIndex);
private:
    int m_nScreen;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};

#endif // PARAMETERPLAYER_H
