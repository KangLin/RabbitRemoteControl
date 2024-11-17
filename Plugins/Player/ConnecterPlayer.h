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
    virtual int Initial() override;
    virtual int Clean() override;

Q_SIGNALS:
    void sigStart(bool bStart);
    void sigPause(bool bPause);
    void sigChangePosition(qint64 pos);

Q_SIGNALS:
    void sigScreenShot();
public Q_SLOTS:
    virtual void slotScreenShot() override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CParameterPlayer m_Parameters;

#if HAVE_QVideoWidget
    CFrmPlayer m_Player;
public:
    virtual QWidget *GetViewer() override;
    QVideoSink *GetVideoSink();
public Q_SLOTS:
    void slotPositionChanged(qint64 pos, qint64 duration);
    void slotPlaybackStateChanged(QMediaPlayer::PlaybackState state);
#if HAVE_QT6_RECORD
    void slotRecordStateChanged(QMediaRecorder::RecorderState state);
#endif
#else
    QAction* m_pPause;
#endif

};

#endif // CONNETERPLAYER_H
