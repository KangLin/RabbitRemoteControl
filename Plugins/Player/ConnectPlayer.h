// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTPLAYER_H
#define CONNECTPLAYER_H

#pragma once

#include <QtGlobal>

#include <QMediaCaptureSession>
#include <QMediaPlayer>
#include <QVideoSink>

#include <QAudioOutput>
#include <QAudioInput>
#include <QCamera>

#include "ConnectDesktop.h"
#include "ConnecterPlayer.h"

class CConnectPlayer : public CConnectDesktop
{
    Q_OBJECT

public:
    explicit CConnectPlayer(CConneterPlayer* pConnecter);
    virtual ~CConnectPlayer();

public Q_SLOTS:
    virtual void slotStart();
    virtual void slotStop();

    // CConnectDesktop interface
    virtual void slotClipBoardChanged() override;

    // CConnect interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;

private Q_SLOTS:
    void slotEnableAudioOutput(bool bEnable);

private:
    QMediaPlayer m_Player;
    QMediaCaptureSession m_CaptureSessioin;
    QVideoSink m_VideoSink;
    QCamera* m_pCamera;
    QAudioOutput m_AudioOutput;
    QAudioInput m_AudioInput;

    CParameterPlayer* m_pParameters;
};

#endif // CONNECTPLAYER_H
