#include <QLoggingCategory>
#include <QUrl>
#include <QVideoFrame>
#include "RecordVideo.h"

static Q_LOGGING_CATEGORY(log, "Record.Video")
static Q_LOGGING_CATEGORY(logThread, "Record.Video.Thread")

CRecordVideoThread::CRecordVideoThread()
    /*! \note that the parent object pointer cannot be set here.
     * If set the parent, the object is also deleted
     * when the parent object (CConnecterThread) is destroyed.
     * Because it is deleted when it is finished.
     */
    : QThread()
{
    bool check = connect(this, SIGNAL(finished()),
                         this, SLOT(deleteLater()));
    Q_ASSERT(check);
}

int CRecordVideoThread::SetFile(const QString &szFile)
{
    m_szFile = szFile;
    return 0;
}

void CRecordVideoThread::run()
{
    bool check = false;
    qDebug(logThread) << "Record video thread start ...";
    CRecordVideo record;
    int nRet = record.Start(m_szFile);
    if(nRet) {
        qCritical(logThread) << "Start record fail:" << nRet;
        emit sigStatusChanged(CFrmViewer::RecordVideoStatus::Error);
        return;
    }
    check = connect(&record, SIGNAL(sigStatusChanged(CFrmViewer::RecordVideoStatus)),
                    this, SIGNAL(sigStatusChanged(CFrmViewer::RecordVideoStatus)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdate(QImage)),
                    &record, SLOT(slotUpdate(QImage)));
    Q_ASSERT(check);
    check = connect(&record, SIGNAL(sigError(int,QString)),
                    this, SIGNAL(sigError(int,QString)));
    Q_ASSERT(check);
    emit sigStatusChanged(CFrmViewer::RecordVideoStatus::Recording);

    exec();

    nRet = record.Stop();
    if(nRet) {
        qCritical(logThread) << "Stop record fail:" << nRet;
        emit sigStatusChanged(CFrmViewer::RecordVideoStatus::Error);
    }
    else
        emit sigStatusChanged(CFrmViewer::RecordVideoStatus::Stop);
    qDebug(logThread) << "Record video thread end";
}

CRecordVideo::CRecordVideo(QObject *parent)
    : QObject{parent}
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    bool check = connect(
        &m_Recorder, &QMediaRecorder::errorOccurred,
        this, [&](QMediaRecorder::Error error, const QString &errorString) {
            qDebug(log) << "Recorder error occurred:" << error << errorString;
            Stop();
            emit sigError(error, errorString);
        });
    Q_ASSERT(check);
#endif
}

int CRecordVideo::Start(const QString &szFile)
{
    int nRet = 0;
    qDebug(log) << "Start";
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    m_Parameter.SetEnable(true);
    m_Parameter >> m_Recorder;
    m_CaptureSession.setVideoFrameInput(&m_VideoFrameInput);
    //m_CaptureSession.setAudioBufferInput(&m_AudioBufferInput);
    m_CaptureSession.setRecorder(&m_Recorder);
    m_Recorder.setOutputLocation(QUrl::fromLocalFile(m_Parameter.GetFile(true)));
    m_Recorder.record();
#endif
    return nRet;
}

int CRecordVideo::Stop()
{
    int nRet = 0;
    qDebug(log) << "Stop";
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    m_Recorder.stop();
    m_CaptureSession.setVideoFrameInput(nullptr);
    m_CaptureSession.setAudioBufferInput(nullptr);
    m_CaptureSession.setRecorder(nullptr);
#endif

    return nRet;
}

void CRecordVideo::slotUpdate(QImage img)
{
    qDebug(log) << "Update image";
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    QVideoFrame frame(img);
    bool bRet = m_VideoFrameInput.sendVideoFrame(frame);
    if(!bRet) {
        //TODO: 放入未成功发送队列，
        //    当 QVideoFrameInput::readyToSendVideoFrame() 时，再发送
        qDebug(log) << "m_VideoFrameInput.sendVideoFrame fail";
    }
#endif
}
