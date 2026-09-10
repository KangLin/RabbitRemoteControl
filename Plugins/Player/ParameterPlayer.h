// Author: Kang Lin <kl222@126.com>

#ifndef PARAMETERPLAYER_H
#define PARAMETERPLAYER_H

#include "ParameterDesktop.h"

class CParameterPlayer : public CParameterDesktop
{
    Q_OBJECT
public:
    explicit CParameterPlayer(QObject *parent = nullptr);

    enum class TYPE {
        Camera,
        Url
    };
    Q_ENUM(TYPE)
    [[nodiscard]] const TYPE GetType() const;
    int SetType(TYPE type);

    [[nodiscard]] const QString GetUrl() const;
    int SetUrl(const QString &szUrl);

    [[nodiscard]] const int GetCamera() const;
    int SetCamera(int nIndex);

    [[nodiscard]] const int GetAudioInput() const;
    int SetAudioInput(int nIndex);
Q_SIGNALS:
    void sigAudioInput(int nIndex);

public:
    [[nodiscard]] const bool GetEnableAudioInput() const;
    int SetEnableAudioInput(bool bEnable);
Q_SIGNALS:
    void sigEnableAudioInput(bool bEnable);

public:
    [[nodiscard]] const bool GetAudioInputMuted() const;
    int SetAudioInputMuted(bool bMuted);
Q_SIGNALS:
    void sigAudioInputMuted(bool bMuted);

public:
    [[nodiscard]] const float GetAudioInputVolume() const;
    int SetAudioInputVolume(float fVolume);
Q_SIGNALS:
    void sigAudioInputVolume(float fVolume);

public:
    [[nodiscard]] const int GetAudioOutput() const;
    int SetAudioOutput(int nIndex);
Q_SIGNALS:
    void sigAudioOutput(int nIndex);

public:
    [[nodiscard]] const bool GetEnableAudioOutput() const;
    int SetEnableAudioOutput(bool bEnable);
Q_SIGNALS:
    void sigEnableAudioOutput(bool bEnable);

public:
    [[nodiscard]] const bool GetAudioOutputMuted() const;
    int SetAudioOutputMuted(bool bMuted);
Q_SIGNALS:
    void sigAudioOutputMuted(bool bMuted);

public:
    [[nodiscard]] const float GetAudioOutputVolume() const;
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
    [[nodiscard]] bool GetSubtitle() const;
    void SetSubtitle(bool subtitle);
private:
    bool m_bSubtitle;

public:
    [[nodiscard]] int GetScreen() const;
    int SetScreen(int nIndex);
private:
    int m_nScreen;

public:
    [[nodiscard]] const int GetHideToolBar() const;
    int SetHideToolBar(int nTime);
private:
    int m_nHideToolBar;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};

#endif // PARAMETERPLAYER_H
