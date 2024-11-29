// Author: Kang Lin <kl222@126.com>

#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QLoggingCategory>
#include <QWheelEvent>
#include <QVideoFrame>
#include <QDesktopServices>

#include "ConnectDesktop.h"
#include "ConnecterThread.h"

static Q_LOGGING_CATEGORY(log, "Client.Connect.Desktop")
static Q_LOGGING_CATEGORY(logMouse, "Client.Connect.Desktop.Mouse")

#define TypeRecordVideo (QEvent::User + 1)
class QRecordVideoEvent : public QEvent
{
public:
    QRecordVideoEvent(const QImage& img): QEvent((QEvent::Type)TypeRecordVideo)
    {
        m_Image = img;
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
int g_QMessageBox_Icon = qRegisterMetaType<Qt::MouseButton>("QMessageBox::Icon");

CConnectDesktop::CConnectDesktop(CConnecter *pConnecter, bool bDirectConnection)
    : CConnect(pConnecter)
#if HAVE_QT6_RECORD
    , m_pParameterRecord(nullptr)
    , m_VideoFrameInput(this)
    , m_AudioBufferInput(this)
    , m_AudioBufferOutput(this)
#endif
{
    if(pConnecter) {
        CFrmScroll* pScroll = qobject_cast<CFrmScroll*>(pConnecter->GetViewer());
        if(pScroll) {
            CFrmViewer* pView = pScroll->GetViewer();
            if(pView)
                SetViewer(pView, bDirectConnection);
        } else {
            QString szErr = pConnecter->metaObject()->className();
            szErr += "::GetViewer() is not CFrmView";
            qDebug(log) << szErr.toStdString().c_str();
        }
        SetConnecter(pConnecter);
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

CConnectDesktop::~CConnectDesktop()
{
    qDebug(log) << "CConnectDesktop::~CConnectDesktop()";
}

int CConnectDesktop::SetConnecter(CConnecter* pConnecter)
{
    qDebug(log) << "CConnectDesktop::SetConnecter" << pConnecter;
    Q_ASSERT(pConnecter);
    if(!pConnecter) return -1;

    bool check = false;
    check = connect(this, SIGNAL(sigServerName(const QString&)),
                    pConnecter, SLOT(slotSetServerName(const QString&)));
    Q_ASSERT(check);
    check = connect(pConnecter, SIGNAL(sigClipBoardChanged()),
                    this, SLOT(slotClipBoardChanged()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSetClipboard(QMimeData*)),
                    pConnecter, SLOT(slotSetClipboard(QMimeData*)));
    Q_ASSERT(check);
#if HAVE_QT6_RECORD
    CConnecterThread* p = qobject_cast<CConnecterThread*>(pConnecter);
    if(p) {
        m_pParameterRecord = &p->GetParameter()->m_Record;
        check = connect(p, SIGNAL(sigRecord(bool)),
                        this, SLOT(slotRecord(bool)));
        Q_ASSERT(check);

        check = connect(p, SIGNAL(sigRecordPause(bool)),
                        this, SLOT(slotRecordPause(bool)));
        Q_ASSERT(check);
        check = connect(
            &m_Recorder,
            SIGNAL(recorderStateChanged(QMediaRecorder::RecorderState)),
            p, SLOT(slotRecorderStateChanged(QMediaRecorder::RecorderState)));
        Q_ASSERT(check);
    }
#endif
    return 0;
}

int CConnectDesktop::SetViewer(CFrmViewer *pView, bool bDirectConnection)
{
    Q_ASSERT(pView);
    if(!pView) return -1;
    
    bool check = false;
    check = connect(this, SIGNAL(sigConnected()), pView, SLOT(slotConnected()));
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
    check = connect(this, SIGNAL(sigRecordVideo(bool)),
                    pView, SLOT(slotRecordVideo(bool)));
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

void CConnectDesktop::slotWheelEvent(QWheelEvent *event, QPoint pos)
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

void CConnectDesktop::slotMouseMoveEvent(QMouseEvent *event, QPoint pos)
{
    QMouseEvent* e = new QMouseEvent(event->type(), pos, event->button(),
                                     event->buttons(), event->modifiers());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnectDesktop::slotMousePressEvent(QMouseEvent *event, QPoint pos)
{
    QMouseEvent* e = new QMouseEvent(event->type(), pos, event->button(),
                                     event->buttons(), event->modifiers());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnectDesktop::slotMouseReleaseEvent(QMouseEvent *event, QPoint pos)
{
    QMouseEvent* e = new QMouseEvent(event->type(), pos, event->button(),
                                     event->buttons(), event->modifiers());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnectDesktop::slotKeyPressEvent(QKeyEvent *event)
{
    QKeyEvent* e = new QKeyEvent(event->type(), event->key(),
                                 event->modifiers(), event->text());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnectDesktop::slotKeyReleaseEvent(QKeyEvent *event)
{
    QKeyEvent* e = new QKeyEvent(event->type(), event->key(),
                                 event->modifiers(), event->text());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnectDesktop::mouseMoveEvent(QMouseEvent *event)
{
    qDebug(logMouse) << "Need to implement CConnectDesktop::mouseMoveEvent";
}

void CConnectDesktop::mousePressEvent(QMouseEvent *event)
{
    qDebug(logMouse) << "Need to implement CConnectDesktop::mousePressEvent";
}

void CConnectDesktop::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug(logMouse) << "Need to implement CConnectDesktop::mouseReleaseEvent";
}

void CConnectDesktop::wheelEvent(QWheelEvent *event)
{
    qDebug(logMouse) << "Need to implement CConnectDesktop::wheelEvent";
}

void CConnectDesktop::keyPressEvent(QKeyEvent *event)
{
    qDebug(logMouse) << "Need to implement CConnectDesktop::keyPressEvent";
}

void CConnectDesktop::keyReleaseEvent(QKeyEvent *event)
{
    qDebug(logMouse) << "Need to implement CConnectDesktop::keyReleaseEvent";
}

int CConnectDesktop::WakeUp()
{
    return 0;
}

bool CConnectDesktop::event(QEvent *event)
{
    //qDebug(log) << "CConnectDesktop::event" << event;
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
void CConnectDesktop::slotRecord(bool bRecord)
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
    emit sigRecordVideo(bRecord);
}

void CConnectDesktop::slotRecordPause(bool bPause)
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

void CConnectDesktop::slotRecordVideo(const QImage &img)
{
    QRecordVideoEvent* e = new QRecordVideoEvent(img);
    if(!e) return;
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnectDesktop::RecordVideo(QRecordVideoEvent *e)
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

int CConnectDesktop::Disconnect()
{
    slotRecord(false);
    return CConnect::Disconnect();
}
#endif
