// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QThread>
#include <QImage>
#if HAVE_QT6_MULTIMEDIA
#include <QMediaCaptureSession>
#endif

#ifdef HAVE_QT6_RECORD
    #include <QVideoSink>
    #include <QAudioInput>
    #include <QAudioOutput>
    #include <QAudioBufferInput>
    #include <QAudioBufferOutput>
    #include <QVideoFrameInput>
#endif

#include "ParameterRecord.h"
#include "ParameterMediaDevices.h"
#include "ParameterWebBrowser.h"

class CMultimediaRecord : public QObject
{
    Q_OBJECT
public:
    explicit CMultimediaRecord(CParameterWebBrowser* pPara, QObject *parent = nullptr);

    enum RV {
        Success = 0,
        Fail = -1
    };
    Q_ENUM(RV)

public Q_SLOTS:
    void slotStart();
    void slotStop();
    void slotUpdateVideoFrame(const QImage image);

Q_SIGNALS:
    void sigRunning();
    void sigFinished();

private:
    CParameterRecord* m_pParaRecord;
    CParameterMediaDevices* m_pMediaDevices;
#if HAVE_QT6_MULTIMEDIA
    QMediaCaptureSession m_CaptureSession;
#endif
#ifdef HAVE_QT6_RECORD
    QAudioInput m_AudioInput;
    QVideoFrameInput m_VideoFrameInput;
    QAudioBufferInput m_AudioBufferInput;
    QAudioBufferOutput m_AudioBufferOutput;
    QMediaRecorder m_Recorder;
    qint64 m_VideoFrameStartTime;
private Q_SLOTS:
    void slotRecordStateChanged(QMediaRecorder::RecorderState state);
    void slotRecordError(QMediaRecorder::Error error, const QString &errorString);
#endif    
};

class CMultimediaRecordThread : public QThread
{
    Q_OBJECT
public:
    explicit CMultimediaRecordThread(CParameterWebBrowser* pPara, QObject *parent = nullptr);

public Q_SLOTS:
    void slotUpdateVideoFrame(const QImage image);
protected:
    virtual void run() override;
private:
    CParameterWebBrowser* m_pPara;
    CMultimediaRecord* m_pRecord;
};
