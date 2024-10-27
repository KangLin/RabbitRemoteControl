// Author: Kang Lin <kl222@126.com>

#ifndef CONNETERPLAYER_H
#define CONNETERPLAYER_H

#pragma once

#include <QMenu>
#include <QMediaPlayer>
#include <QMediaRecorder>
#if HAVE_QVideoWidget
#include "FrmPlayer.h"
#endif

#include "ConnecterThread.h"
#include "ParameterPlayer.h"

class CConnecterPlayer : public CConnecterThread
{
    Q_OBJECT

public:
    explicit CConnecterPlayer(CPluginClient *plugin);
    virtual ~CConnecterPlayer();

    // CConnecter interface
public:
    virtual qint16 Version() override;
    // CConnecterConnect interface
    virtual CConnect *InstanceConnect() override;

Q_SIGNALS:
    void sigStart(bool bStart);
    void sigPause(bool bPause);
    void sigRecordPause(bool bPause);
    void sigPositionChanged(qint64 pos, qint64 duration);
    void sigChangePosition(qint64 pos);
    void sigPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void sigRecordStateChanged(QMediaRecorder::RecorderState state);

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CParameterPlayer m_Parameters;

#if HAVE_QVideoWidget
    CFrmPlayer m_Player;
public:
    virtual QWidget *GetViewer() override;
    QVideoSink *GetVideoSink();
#endif

};

#endif // CONNETERPLAYER_H
