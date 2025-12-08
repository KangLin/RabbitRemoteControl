#include <QDesktopServices>
#include <QUrl>
#include <QLoggingCategory>
#include <QDateTime>
#include <QFileInfo>
#include "MultimediaRecord.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Record")

CMultimediaRecord::CMultimediaRecord(CParameterWebBrowser *pPara, QObject *parent)
    : QObject{parent}
    , m_pMediaDevices(nullptr)
    , m_VideoFrameStartTime(0)
{
    bool check = false;
    qDebug(log) << Q_FUNC_INFO;
    if(pPara) {
        m_ParaRecord = pPara->m_Record;
        m_pMediaDevices = &pPara->m_MediaDevices;
    }
#if HAVE_QT6_RECORD
    check = connect(&m_Recorder, &QMediaRecorder::recorderStateChanged,
                    this, &CMultimediaRecord::slotRecordStateChanged);
    Q_ASSERT(check);
    check = connect(&m_Recorder, &QMediaRecorder::errorOccurred,
                    this, &CMultimediaRecord::slotRecordError);
    Q_ASSERT(check);
#endif // #if HAVE_QT6_RECORD
}

CMultimediaRecord::~CMultimediaRecord()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CMultimediaRecord::slotStart()
{
#ifdef HAVE_QT6_RECORD
    // 配置音频输入（捕获系统音频输入或麦克风）
    if(m_pMediaDevices) {
        const auto inputs = QMediaDevices::audioInputs();
        foreach(auto input, inputs) {
            if(input.id() == m_pMediaDevices->m_Para.m_AudioInput) {
                m_AudioInput.setDevice(input);
                m_CaptureSession.setAudioInput(&m_AudioInput);
                break;
            }
        }
    }
    m_CaptureSession.setVideoFrameInput(&m_VideoFrameInput);
    // 配置录制器, 设置录制参数：输出文件、编码、质量等
    m_ParaRecord >> m_Recorder;
    m_CaptureSession.setRecorder(&m_Recorder);
    m_VideoFrameStartTime = 0;
    m_Recorder.record();
#endif
}

void CMultimediaRecord::slotStop()
{
#ifdef HAVE_QT6_RECORD
    m_Recorder.stop();
#endif
    m_pMediaDevices = nullptr;
}

void CMultimediaRecord::slotUpdateVideoFrame(const QImage image)
{
    //qDebug(log) << Q_FUNC_INFO;
#ifdef HAVE_QT6_RECORD
    // 将 QImage 转换为 QVideoFrame
    QImage img = image;
    //qDebug(log) << "Image format:" << img.format();
    if(QImage::Format_ARGB32 != image.format())
        img = image.convertToFormat(QImage::Format_ARGB32);
    /*
    QVideoFrameFormat format(image.size(), QVideoFrameFormat::Format_ABGR8888);
    QVideoFrame frame(format);
    if (frame.map(QVideoFrame::WriteOnly)) {
        memcpy(frame.bits(0), image.bits(), image.sizeInBytes());
        frame.unmap();
    }//*/
    QVideoFrame frame(img);
    // 设置帧的时间戳（微秒），这对于同步很重要
    if (m_VideoFrameStartTime == 0) m_VideoFrameStartTime = QDateTime::currentMSecsSinceEpoch() * 1000;
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch() * 1000 - m_VideoFrameStartTime;
    frame.setStartTime(currentTime);
    qreal rate = m_ParaRecord.GetVideoFrameRate();
    if(rate <= 0)
        rate = 24;
    frame.setEndTime(currentTime + qreal(1000000) / rate);
    frame.setStreamFrameRate(m_ParaRecord.GetVideoFrameRate());//*/
    m_VideoFrameInput.sendVideoFrame(frame);
#endif
}

#if HAVE_QT6_RECORD
void CMultimediaRecord::slotRecordStateChanged(QMediaRecorder::RecorderState state)
{
    qDebug(log) << "Record state:" << state;
    switch(state) {
    case QMediaRecorder::RecorderState::StoppedState: {
        QString szFile = m_Recorder.actualLocation().toLocalFile();
        switch(m_ParaRecord.GetEndAction())
        {
        case CParameterRecord::ENDACTION::OpenFile: {
            bool bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
            if(!bRet)
                qCritical(log) << "Fail: Open capture page the file:" << szFile;
            break;
        }
        case CParameterRecord::ENDACTION::OpenFolder: {
            QFileInfo fi(szFile);
            QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
            break;
        }
        default:
            break;
        }
        emit sigFinished();
        break;

    }
    case QMediaRecorder::RecorderState::RecordingState: {
        emit sigRunning();
        break;
    }
    default:
        break;
    }
}

void CMultimediaRecord::slotRecordError(QMediaRecorder::Error error, const QString &errorString)
{
    qCritical(log) << error << errorString;
    emit sigFinished();
}
#endif // HAVE_QT6_RECORD

CMultimediaRecordThread::CMultimediaRecordThread(
    CParameterWebBrowser* pPara, QObject *parent)
    : QThread(parent)
    , m_pPara(pPara)
    , m_pRecord(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    connect(this, &QThread::finished, this, &QObject::deleteLater);
}

CMultimediaRecordThread::~CMultimediaRecordThread()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CMultimediaRecordThread::run()
{
    m_pRecord = new CMultimediaRecord(m_pPara);
    if(!m_pRecord) return;
    connect(m_pRecord, &CMultimediaRecord::sigFinished,
            this, &QThread::quit);
    m_pRecord->slotStart();
    exec();
    delete m_pRecord;
    m_pRecord = nullptr;
}

void CMultimediaRecordThread::slotUpdateVideoFrame(const QImage image)
{
    if(!m_pRecord) return;
    m_pRecord->slotUpdateVideoFrame(image);
}

void CMultimediaRecordThread::slotQuit()
{
    if(!m_pRecord) return;
    m_pRecord->slotStop();
    /*
    QMetaObject::invokeMethod(m_pRecord,
                              "slotStop",
                              Qt::AutoConnection);*/
}
