// Author: Kang Lin <kl222@126.com>

#include <QApplication>
#include <QClipboard>
#include <QWheelEvent>
#include <QVideoFrame>
#include <QDesktopServices>
#include <QLoggingCategory>
#include <QFileInfo>

#include "BackendDesktop.h"
#include "FrmScroll.h"

static Q_LOGGING_CATEGORY(log, "Backend.Desktop")
static Q_LOGGING_CATEGORY(logMouse, "Backend.Desktop.Mouse")

#define TypeRecordVideo (QEvent::User + 1)
class QRecordVideoEvent : public QEvent
{
public:
    QRecordVideoEvent(const QImage& img): QEvent((QEvent::Type)TypeRecordVideo)
    {
        m_Image = img;
    }
    ~QRecordVideoEvent()
    {
        qDebug(log) << Q_FUNC_INFO;
    }
    
    QImage GetImage()
    {
        return m_Image;
    }
private:
    QImage m_Image;
};

int g_QtKeyboardModifiers = qRegisterMetaType<Qt::KeyboardModifiers>("KeyboardModifiers");
int g_QtMouseButtons = qRegisterMetaType<Qt::MouseButtons>("MouseButtons");
int g_QtMouseButton = qRegisterMetaType<Qt::MouseButton>("MouseButton");
int g_QMessageBox_Icon = qRegisterMetaType<QMessageBox::Icon>("QMessageBox::Icon");

CBackendDesktop::CBackendDesktop(COperateDesktop *pOperate, bool bDirectConnection)
    : CBackend(pOperate)
#if HAVE_QT6_RECORD
    , m_pParameterRecord(nullptr)
    , m_VideoFrameInput(this)
    , m_AudioBufferInput(this)
    , m_AudioBufferOutput(this)
#endif
{
    qDebug(log) << Q_FUNC_INFO;
    if(pOperate) {
        CFrmScroll* pScroll = qobject_cast<CFrmScroll*>(pOperate->GetViewer());
        if(pScroll) {
            CFrmViewer* pView = pScroll->GetViewer();
            if(pView)
                SetViewer(pView, pOperate, bDirectConnection);
            else {
                QString szErr = pOperate->metaObject()->className();
                szErr += "::GetViewer() is not CFrmViewer";
                qWarning(log) << szErr.toStdString().c_str();
            }
        } else {
            QString szErr = pOperate->metaObject()->className();
            szErr += "::GetViewer() is not CFrmScroll";
            qWarning(log) << szErr.toStdString().c_str();
        }
        SetConnect(pOperate);
    }

#if HAVE_QT6_RECORD
    bool check = connect(
        &m_Recorder, &QMediaRecorder::errorOccurred,
        this, [&](QMediaRecorder::Error error, const QString &errorString) {
            qDebug(log) << "Recorder error occurred:" << error << errorString;
            slotRecord(false);
            emit sigError(error, errorString);
        });
    Q_ASSERT(check);
    check = connect(
        &m_Recorder, &QMediaRecorder::recorderStateChanged,
        this, [&](QMediaRecorder::RecorderState state){
            qDebug(log) << "Recorder state changed:" << state;
            if(QMediaRecorder::StoppedState == state)
            {
                slotRecord(false);
                if(m_pParameterRecord) {
                    qDebug(log) << "End action:"
                                << m_pParameterRecord->GetEndAction()
                                << m_Recorder.actualLocation();
                    switch(m_pParameterRecord->GetEndAction())
                    {
                    case CParameterRecord::ENDACTION::OpenFile:
                        QDesktopServices::openUrl(m_Recorder.actualLocation());
                        break;
                    case CParameterRecord::ENDACTION::OpenFolder: {
                        QFileInfo fi(m_Recorder.actualLocation().toLocalFile());
                        QDesktopServices::openUrl(
                            QUrl::fromLocalFile(fi.absolutePath()));
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        });
    Q_ASSERT(check);
    check = connect(&m_Recorder, &QMediaRecorder::actualLocationChanged,
                    this, [&](const QUrl &location){
                        qInfo(log) << "Recorder actual location changed:" << location;
                    });
    Q_ASSERT(check);
#endif
}

CBackendDesktop::~CBackendDesktop()
{
    qDebug(log) << Q_FUNC_INFO;
}

int CBackendDesktop::SetConnect(COperateDesktop *pOperate)
{
    //qDebug(log) << "CBackendDesktop::SetConnect:" << pOperate;
    Q_ASSERT(pOperate);
    if(!pOperate) return -1;
    
    bool check = false;
    check = connect(this, SIGNAL(sigServerName(const QString&)),
                    pOperate, SLOT(slotSetServerName(const QString&)));
    Q_ASSERT(check);
    check = connect(pOperate, SIGNAL(sigClipBoardChanged()),
                    this, SLOT(slotClipBoardChanged()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSetClipboard(QMimeData*)),
                    pOperate, SLOT(slotSetClipboard(QMimeData*)));
    Q_ASSERT(check);
#if HAVE_QT6_RECORD
    if(pOperate) {
        m_pParameterRecord = &pOperate->GetParameter()->m_Record;
        check = connect(pOperate, SIGNAL(sigRecord(bool)),
                        this, SLOT(slotRecord(bool)));
        Q_ASSERT(check);
        
        check = connect(pOperate, SIGNAL(sigRecordPause(bool)),
                        this, SLOT(slotRecordPause(bool)));
        Q_ASSERT(check);
        check = connect(
            &m_Recorder,
            SIGNAL(recorderStateChanged(QMediaRecorder::RecorderState)),
            pOperate, SLOT(slotRecorderStateChanged(QMediaRecorder::RecorderState)));
        Q_ASSERT(check);
    }
#endif
    return 0;
}

int CBackendDesktop::SetViewer(CFrmViewer *pView,
                               COperateDesktop* pOperate, bool bDirectConnection)
{
    Q_ASSERT(pView);
    if(!pView) return -1;

    bool check = false;
    check = connect(this, SIGNAL(sigRunning()), pView, SLOT(slotRunning()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSetDesktopSize(int, int)),
                    pView, SLOT(slotSetDesktopSize(int, int)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigServerName(const QString&)),
                    pView, SLOT(slotSetName(const QString&)));
    Q_ASSERT(check);
    
    check = connect(this, SIGNAL(sigUpdateRect(const QRect&, const QImage&)),
                    pView, SLOT(slotUpdateRect(const QRect&, const QImage&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateRect(const QImage&)),
                    pView, SLOT(slotUpdateRect(const QImage&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateCursor(const QCursor&)),
                    pView, SLOT(slotUpdateCursor(const QCursor&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateCursorPosition(const QPoint&)),
                    pView, SLOT(slotUpdateCursorPosition(const QPoint&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateLedState(unsigned int)),
                    pView, SLOT(slotUpdateLedState(unsigned int)));
    Q_ASSERT(check);
    
#if HAVE_QT6_RECORD
    check = connect(this, SIGNAL(sigRecordVideo(bool, qreal)),
                    pView, SLOT(slotRecordVideo(bool, qreal)));
    Q_ASSERT(check);
    check = connect(pView, SIGNAL(sigRecordVideo(QImage)),
                    this, SLOT(slotRecordVideo(QImage)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
#endif
    if(bDirectConnection)
    {
        /* \~chinese 因为连接可能是在另一个线程中的非Qt事件处理，
         *           它可能会阻塞线程，那会导致键盘、鼠标事件延迟，
         *           所以这里用 Qt::DirectConnection
         * \~english Because the connection may be a non-Qt event processing in another thread,
         *           it may block the thread, which will cause the keyboard and mouse events to be delayed.
         *           So here use Qt::DirectConnection 
         */
        check = connect(pView, SIGNAL(sigMousePressEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMousePressEvent(QMouseEvent*, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigMouseReleaseEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMouseReleaseEvent(QMouseEvent*, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigMouseMoveEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMouseMoveEvent(QMouseEvent*, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigWheelEvent(QWheelEvent*, QPoint)),
                        this, SLOT(slotWheelEvent(QWheelEvent*, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigKeyPressEvent(QKeyEvent*)),
                        this, SLOT(slotKeyPressEvent(QKeyEvent*)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigKeyReleaseEvent(QKeyEvent*)),
                        this, SLOT(slotKeyReleaseEvent(QKeyEvent*)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
    } else {
        check = connect(pView, SIGNAL(sigMousePressEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMousePressEvent(QMouseEvent*, QPoint)));
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigMouseReleaseEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMouseReleaseEvent(QMouseEvent*, QPoint)));
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigMouseMoveEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMouseMoveEvent(QMouseEvent*, QPoint)));
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigWheelEvent(QWheelEvent*, QPoint)),
                        this, SLOT(slotWheelEvent(QWheelEvent*, QPoint)));
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigKeyPressEvent(QKeyEvent*)),
                        this, SLOT(slotKeyPressEvent(QKeyEvent*)));
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigKeyReleaseEvent(QKeyEvent*)),
                        this, SLOT(slotKeyReleaseEvent(QKeyEvent*)));
        Q_ASSERT(check);
    }
    
    return 0;
}

void CBackendDesktop::slotWheelEvent(QWheelEvent *event, QPoint pos)
{
    QWheelEvent* e = new QWheelEvent(
        pos,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->globalPosition(),
#else
        event->globalPos(),
#endif
        event->pixelDelta(), event->angleDelta(), event->buttons(),
        event->modifiers(), event->phase(), event->inverted(), event->source());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CBackendDesktop::slotMouseMoveEvent(QMouseEvent *event, QPoint pos)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
    QMouseEvent* e = new QMouseEvent(event->type(), pos, event->button(),
                                     event->buttons(), event->modifiers());
#else
    QMouseEvent* e = new QMouseEvent(event->type(), pos, pos, event->button(),
                                     event->buttons(), event->modifiers());
#endif
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CBackendDesktop::slotMousePressEvent(QMouseEvent *event, QPoint pos)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
    QMouseEvent* e = new QMouseEvent(event->type(), pos, event->button(),
                                     event->buttons(), event->modifiers());
#else
    QMouseEvent* e = new QMouseEvent(event->type(), pos, pos, event->button(),
                                     event->buttons(), event->modifiers());
#endif
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CBackendDesktop::slotMouseReleaseEvent(QMouseEvent *event, QPoint pos)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
    QMouseEvent* e = new QMouseEvent(event->type(), pos, event->button(),
                                     event->buttons(), event->modifiers());
#else
    QMouseEvent* e = new QMouseEvent(event->type(), pos, pos, event->button(),
                                     event->buttons(), event->modifiers());
#endif
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CBackendDesktop::slotKeyPressEvent(QKeyEvent *event)
{
    QKeyEvent* e = new QKeyEvent(event->type(), event->key(),
                                 event->modifiers(), event->text());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CBackendDesktop::slotKeyReleaseEvent(QKeyEvent *event)
{
    QKeyEvent* e = new QKeyEvent(event->type(), event->key(),
                                 event->modifiers(), event->text());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CBackendDesktop::mouseMoveEvent(QMouseEvent *event)
{
    qDebug(logMouse) << "Need to implement CBackendDesktop::mouseMoveEvent";
}

void CBackendDesktop::mousePressEvent(QMouseEvent *event)
{
    qDebug(logMouse) << "Need to implement CBackendDesktop::mousePressEvent";
}

void CBackendDesktop::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug(logMouse) << "Need to implement CBackendDesktop::mouseReleaseEvent";
}

void CBackendDesktop::wheelEvent(QWheelEvent *event)
{
    qDebug(logMouse) << "Need to implement CBackendDesktop::wheelEvent";
}

void CBackendDesktop::keyPressEvent(QKeyEvent *event)
{
    qDebug(logMouse) << "Need to implement CBackendDesktop::keyPressEvent";
}

void CBackendDesktop::keyReleaseEvent(QKeyEvent *event)
{
    qDebug(logMouse) << "Need to implement CBackendDesktop::keyReleaseEvent";
}

int CBackendDesktop::WakeUp()
{
    return 0;
}

bool CBackendDesktop::event(QEvent *event)
{
    //qDebug(log) << "CBackendDesktop::event" << event;
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
        mousePressEvent((QMouseEvent*)event);
        break;
    case QEvent::MouseButtonRelease:
        mouseReleaseEvent((QMouseEvent*)event);
        break;
    case QEvent::MouseMove:
        mouseMoveEvent((QMouseEvent*)event);
        break;
    case QEvent::Wheel:
        wheelEvent((QWheelEvent*)event);
        break;
    case QEvent::KeyPress:
        keyPressEvent((QKeyEvent*)event);
        break;
    case QEvent::KeyRelease:
        keyReleaseEvent((QKeyEvent*)event);
        break;
#if HAVE_QT6_RECORD
    case TypeRecordVideo:
        RecordVideo((QRecordVideoEvent*)event);
        break;
#endif
    default:
        return QObject::event(event);
    }
    
    event->accept();
    return true;
}

#if HAVE_QT6_RECORD
void CBackendDesktop::slotRecord(bool bRecord)
{
    qDebug(log) << Q_FUNC_INFO << bRecord;
    if(bRecord) {
        if(QMediaRecorder::RecordingState == m_Recorder.recorderState())
            return;
        (*m_pParameterRecord) >> m_Recorder;
        m_CaptureSession.setVideoFrameInput(&m_VideoFrameInput);
        m_CaptureSession.setRecorder(&m_Recorder);
        m_Recorder.record();
    } else {
        m_Recorder.stop();
        m_CaptureSession.setVideoFrameInput(nullptr);
        m_CaptureSession.setAudioBufferInput(nullptr);
        m_CaptureSession.setRecorder(nullptr);
    }
    emit sigRecordVideo(bRecord, m_pParameterRecord->GetVideoFrameRate());
}

void CBackendDesktop::slotRecordPause(bool bPause)
{
    qDebug(log) << Q_FUNC_INFO << bPause;
    if(bPause) {
        if(m_Recorder.recorderState() == QMediaRecorder::RecordingState)
            m_Recorder.pause();
    } else {
        if(m_Recorder.recorderState() == QMediaRecorder::PausedState)
            m_Recorder.record();
    }
}

void CBackendDesktop::slotRecordVideo(const QImage &img)
{
    QRecordVideoEvent* e = new QRecordVideoEvent(img);
    if(!e) return;
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CBackendDesktop::RecordVideo(QRecordVideoEvent *e)
{
    qDebug(log) << "Update image";
    if(!e) return;
    if(QMediaRecorder::RecordingState != m_Recorder.recorderState()) {
        qCritical(log) << "Recorder is inavailable";
        return;
    }
    QVideoFrame frame(e->GetImage());
    bool bRet = m_VideoFrameInput.sendVideoFrame(frame);
    if(!bRet) {
        //TODO: 放入未成功发送队列，
        //    当 QVideoFrameInput::readyToSendVideoFrame() 时，再发送
        qDebug(log) << "m_VideoFrameInput.sendVideoFrame fail";
    }
}

int CBackendDesktop::Stop()
{
    slotRecord(false);
    return CBackend::Stop();
}
#endif
