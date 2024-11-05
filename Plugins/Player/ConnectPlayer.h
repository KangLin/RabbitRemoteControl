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
    explicit CConnectPlayer(CConnecterPlayer* pConnecter);
    virtual ~CConnectPlayer();

public Q_SLOTS:
    virtual void slotStart();
    virtual void slotStop();
#if HAVE_QT6_RECORD
    virtual void slotRecord(bool bRecord) override;
#endif

    // CConnectDesktop interface
    virtual void slotClipBoardChanged() override;

    // CConnect interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;

private Q_SLOTS:
    void slotVideoFrameChanged(const QVideoFrame &frame);
    void slotEnableAudioInput(bool bEnable);
    void slotEnableAudioOutput(bool bEnable);
    void slotPositionChanged(qint64 pos);
    void slotDurationChanged(qint64 duration);
Q_SIGNALS:
    void sigPositionChanged(qint64 pos, qint64 duration);

private:
    QRect m_Video;
    QCamera* m_pCamera;
    QMediaPlayer m_Player;
    QVideoSink m_VideoSink;
    bool m_bScreenShot;

    QAudioOutput m_AudioOutput;
    QAudioInput m_AudioInput;

    CParameterPlayer* m_pParameters;
    qint64 m_nPosition;
    qint64 m_nDuration;

    QString m_szRecordFile;
};

#endif // CONNECTPLAYER_H
