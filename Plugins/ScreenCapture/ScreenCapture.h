// Author: Kang Lin <kl222@126.com>

#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#pragma once

#include <QMediaCaptureSession>
#include <QScreenCapture>
#include <QWindowCapture>
#include <QImageCapture>
#include <QMediaRecorder>
#include <QVideoWidget>
#include <QMenu>

#include "Connecter.h"
#include "ParameterScreenCapture.h"

class CScreenCapture : public CConnecter
{
    Q_OBJECT
public:
    explicit CScreenCapture(CPluginClient *plugin);

    // CConnecter interface
public:
    virtual qint16 Version() override;
    virtual QWidget *GetViewer() override;

public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

private Q_SLOTS:
    virtual int slotStart();
    virtual int slotStop();

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

    QVideoWidget* m_pWidget;
    CParameterScreenCapture m_Parameter;
    QScreenCapture m_ScreenCapture;
    QWindowCapture m_WindowCapture;
    QMediaCaptureSession m_CaptureSessioin;
    QMediaRecorder m_Recorder;
    QImageCapture m_ImageCapture;
    QString m_szRecordFile;

};

#endif // SCREENCAPTURE_H
