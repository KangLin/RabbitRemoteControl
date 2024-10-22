#ifndef RECORDVIDEO_H
#define RECORDVIDEO_H

#include <QObject>
#include <QImage>
#include <QThread>
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    #include <QMediaCaptureSession>
    #include <QMediaRecorder>
    #include <QVideoFrameInput>
    #include <QAudioBufferInput>
    #include "ParameterRecord.h"
#endif

#include "FrmViewer.h"
#include "client_export.h"

/*!
 * \~chinese 录制视频线程
 * \note 当此线程退出后，会自动删除线程对象。
 *
 * \~english Record video thread
 * \note When the thread finished, the thread will be deleted.
 *
 * \~
 * \see CRecordVideo
 */
class CLIENT_EXPORT CRecordVideoThread : public QThread
{
    Q_OBJECT
public:
    explicit CRecordVideoThread();

    int SetFile(const QString& szFile);

Q_SIGNALS:
    void sigUpdate(QImage img);
    void sigStatusChanged(CFrmViewer::RecordVideoStatus status);
    void sigError(int nRet, QString szText);

protected:
    virtual void run() override;
private:
    QString m_szFile;
};

/*!
 * \brief Record video to file
 */
class CLIENT_EXPORT CRecordVideo : public QObject
{
    Q_OBJECT
public:
    explicit CRecordVideo(QObject *parent = nullptr);

public Q_SLOTS:
    int Start(const QString& szFile);
    int Stop();
    void slotUpdate(QImage img);

Q_SIGNALS:
    void sigStatusChanged(CFrmViewer::RecordVideoStatus status);
    void sigError(int nRet, QString szText);

private:
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    QMediaCaptureSession m_CaptureSession;
    QMediaRecorder m_Recorder;
    QVideoFrameInput m_VideoFrameInput;
    QAudioBufferInput m_AudioBufferInput;
    CParameterRecord m_Parameter;
#endif
};

#endif // RECORDVIDEO_H
