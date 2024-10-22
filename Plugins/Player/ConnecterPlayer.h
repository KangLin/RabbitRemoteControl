// Author: Kang Lin <kl222@126.com>

#ifndef CONNETERPLAYER_H
#define CONNETERPLAYER_H

#pragma once

#include <QMenu>

#if HAVE_QVideoWidget
#include "FrmPlayer.h"
#endif

#include "ConnecterThread.h"
#include "ParameterPlayer.h"

class CConneterPlayer : public CConnecterThread
{
    Q_OBJECT

public:
    explicit CConneterPlayer(CPluginClient *plugin);
    virtual ~CConneterPlayer();

    // CConnecter interface
public:
    virtual qint16 Version() override;
    // CConnecterConnect interface
    virtual CConnect *InstanceConnect() override;

Q_SIGNALS:
    void sigStart();
    void sigStop();

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CParameterPlayer m_Parameters;

#if HAVE_QVideoWidget
    QVideoWidget m_Video;
public:
    virtual QWidget *GetViewer() override;
    QVideoSink *GetVideoSink();
#endif

};

#endif // CONNETERPLAYER_H
