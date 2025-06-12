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

#include "Operate.h"
#include "ParameterScreenCapture.h"

class CScreenCapture : public COperate
{
    Q_OBJECT
public:
    explicit CScreenCapture(CPlugin *plugin);
    virtual ~CScreenCapture();

    // COperate interface
public:
    [[nodiscard]] virtual QWidget *GetViewer() override;
    [[nodiscard]] virtual const qint16 Version() const override;
protected:
    virtual int Initial() override;
    virtual int Clean() override;

public Q_SLOTS:
    virtual int Start() override;
    virtual int Stop() override;

private Q_SLOTS:
    virtual int slotStart();
    virtual int slotStop();

private:
    [[nodiscard]] virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

    QVideoWidget* m_pWidget;
    CParameterScreenCapture m_Parameter;
    QScreenCapture m_ScreenCapture;
    QWindowCapture m_WindowCapture;
    QMediaCaptureSession m_CaptureSessioin;
    QMediaRecorder m_Recorder;
    QImageCapture m_ImageCapture;
    QString m_szRecordFile;

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    virtual int SetParameterPlugin(CParameterPlugin *pPara) override;
};

#endif // SCREENCAPTURE_H
