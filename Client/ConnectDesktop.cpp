// Author: Kang Lin <kl222@126.com>

#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QLoggingCategory>
#include <QWheelEvent>
#include <QVideoFrame>

#include "ConnectDesktop.h"
#include "ConnecterConnect.h"

static Q_LOGGING_CATEGORY(log, "Client.Connect.Desktop")

int g_QtKeyboardModifiers = qRegisterMetaType<Qt::KeyboardModifiers>("KeyboardModifiers");
int g_QtMouseButtons = qRegisterMetaType<Qt::MouseButtons>("MouseButtons");
int g_QtMouseButton = qRegisterMetaType<Qt::MouseButton>("MouseButton");
int g_QMessageBox_Icon = qRegisterMetaType<Qt::MouseButton>("QMessageBox::Icon");

CConnectDesktop::CConnectDesktop(CConnecter *pConnecter, bool bDirectConnection)
    : CConnect(pConnecter)
    , m_pParameter(nullptr)
{
    if(pConnecter) {
        CFrmViewer* pView = qobject_cast<CFrmViewer*>(pConnecter->GetViewer());
        if(pView)
            SetViewer(pView, bDirectConnection);
        else
            qWarning(log) << "pConnecter->GetViewer() is not CFrmView";
        SetConnecter(pConnecter);
    }

#if HAVE_QT6_MULTIMEDIA
    bool check = connect(
        &m_Recorder, &QMediaRecorder::errorOccurred,
        this, [&](QMediaRecorder::Error error, const QString &errorString) {
            qDebug(log) << "Recorder error occurred:" << error << errorString;
            slotRecord(false);
            emit sigError(error, errorString);
        });
    Q_ASSERT(check);
    check = connect(&m_Recorder, &QMediaRecorder::recorderStateChanged,
                    this, [&](QMediaRecorder::RecorderState state){
                        qDebug(log) << "Recorder state changed:" << state;
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
    CConnecterConnect* p = qobject_cast<CConnecterConnect*>(pConnecter);
    if(p) {
        m_pParameter = p->GetParameter();
        check = connect(p, SIGNAL(sigSceenShot()),
                        this, SLOT(slotScreenShot()));
        Q_ASSERT(check);
        check = connect(p, SIGNAL(sigRecord(bool)),
                        this, SLOT(slotRecord(bool)));
        Q_ASSERT(check);
    }
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
    check = connect(this, SIGNAL(sigScreenShot(const QString&)),
                    pView, SLOT(slotScreenShot(const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigRecordVideo(bool)),
                    pView, SLOT(slotRecordVideo(bool)));
    Q_ASSERT(check);
    check = connect(pView, SIGNAL(sigRecordVideo(QImage)),
                    this, SLOT(slotRecordVideo(QImage)),
                    Qt::DirectConnection);
    Q_ASSERT(check);

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
    qDebug(log) << "Need to implement CConnectDesktop::mouseMoveEvent";
}

void CConnectDesktop::mousePressEvent(QMouseEvent *event)
{
    qDebug(log) << "Need to implement CConnectDesktop::mousePressEvent";
}

void CConnectDesktop::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug(log) << "Need to implement CConnectDesktop::mouseReleaseEvent";
}

void CConnectDesktop::wheelEvent(QWheelEvent *event)
{
    qDebug(log) << "Need to implement CConnectDesktop::wheelEvent";
}

void CConnectDesktop::keyPressEvent(QKeyEvent *event)
{
    qDebug(log) << "Need to implement CConnectDesktop::keyPressEvent";
}

void CConnectDesktop::keyReleaseEvent(QKeyEvent *event)
{
    qDebug(log) << "Need to implement CConnectDesktop::keyReleaseEvent";
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
    case TypeRecordVideo:
        RecordVideo((QRecordVideoEvent*)event);
        break;
    default:
        return QObject::event(event);
    }

    event->accept();
    return true;
}

void CConnectDesktop::slotRecord(bool bRecord)
{
#if HAVE_QT6_MULTIMEDIA
    if(bRecord) {
        m_pParameter->m_Record >> m_Recorder;
#if HAVE_QT6_RECORD
        m_CaptureSession.setVideoFrameInput(&m_VideoFrameInput);
        //m_CaptureSession.setAudioBufferInput(&m_AudioBufferInput);
#endif
        m_CaptureSession.setRecorder(&m_Recorder);
        m_Recorder.setOutputLocation(QUrl::fromLocalFile(m_Parameter.GetFile(true)));
        m_Recorder.record();
    } else {
        m_Recorder.stop();
#if HAVE_QT6_RECORD
        m_CaptureSession.setVideoFrameInput(nullptr);
        m_CaptureSession.setAudioBufferInput(nullptr);
#endif
        m_CaptureSession.setRecorder(nullptr);
    }
    emit sigRecordVideo(bRecord);
#endif
}

void CConnectDesktop::slotRecordVideo(const QImage &img)
{
    QRecordVideoEvent* e = new QRecordVideoEvent(img);
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnectDesktop::RecordVideo(QRecordVideoEvent *e)
{
    qDebug(log) << "Update image";
    if(!e) return;
#if HAVE_QT6_RECORD
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
#endif
}

void CConnectDesktop::slotScreenShot()
{
    if(!m_pParameter)
        return;
    QString szFile;
    szFile = m_pParameter->m_Record.GetImageFile(true);
    emit sigScreenShot(szFile);
}
