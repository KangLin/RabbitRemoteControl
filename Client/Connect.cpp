// Author: Kang Lin <kl222@126.com>

#include "FrmViewer.h"
#include "Connect.h"

#include <QApplication>
#include <QDebug>
#include <QtPlugin>
#include <QClipboard>
#include <QTimer>
#include <QLoggingCategory>
#include <QWheelEvent>

static Q_LOGGING_CATEGORY(log, "Client.Connect")

int g_QtKeyboardModifiers = qRegisterMetaType<Qt::KeyboardModifiers>("KeyboardModifiers");
int g_QtMouseButtons = qRegisterMetaType<Qt::MouseButtons>("MouseButtons");
Q_DECLARE_METATYPE(Qt::MouseButton)
int g_QtMouseButton = qRegisterMetaType<Qt::MouseButton>("MouseButton");
int g_QMessageBox_Icon = qRegisterMetaType<Qt::MouseButton>("QMessageBox::Icon");

CConnect::CConnect(CConnecter *pConnecter, QObject *parent, bool bDirectConnection)
    : QObject(parent), m_pView(nullptr)
{
    Q_ASSERT(pConnecter);
    CFrmViewer* pView = qobject_cast<CFrmViewer*>(pConnecter->GetViewer());
    if(pView)
        SetViewer(pView, bDirectConnection);
    SetConnecter(pConnecter);
}

CConnect::~CConnect()
{
    qDebug(log) << "CConnect::~CConnect()";
}

int CConnect::SetConnecter(CConnecter* pConnecter)
{
    Q_ASSERT(pConnecter);
    if(!pConnecter) return -1;
    
    bool check = false;
    check = connect(this, SIGNAL(sigConnected()),
                    pConnecter, SIGNAL(sigConnected()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnect()),
                    pConnecter, SIGNAL(sigDisconnect()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnected()),
                    pConnecter, SIGNAL(sigDisconnected()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigServerName(const QString&)),
                    pConnecter, SLOT(slotSetServerName(const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigError(const int, const QString&)),
                    pConnecter, SIGNAL(sigError(const int, const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigInformation(const QString&)),
                    pConnecter, SIGNAL(sigInformation(const QString&)));
    Q_ASSERT(check);
    check = connect(pConnecter, SIGNAL(sigClipBoardChanged()),
                    this, SLOT(slotClipBoardChanged()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSetClipboard(QMimeData*)),
                    pConnecter, SLOT(slotSetClipboard(QMimeData*)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigShowMessageBox(const QString&, const QString&,
                                                const QMessageBox::Icon&)),
            pConnecter, SIGNAL(sigShowMessageBox(const QString&, const QString&,
                                              const QMessageBox::Icon&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigBlockShowMessageBox(
                              const QString&, const QString&,
                              QMessageBox::StandardButtons,
                              QMessageBox::StandardButton&,
                              bool&, QString)),
                    pConnecter, SLOT(slotBlockShowMessageBox(
                        const QString&, const QString&,
                        QMessageBox::StandardButtons,
                        QMessageBox::StandardButton&,
                        bool&, QString)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigBlockInputDialog(const QString&,
                                                     const QString&,
                                                     const QString&,
                                                     QString&)),
                    pConnecter, SLOT(slotBlockInputDialog(const QString&,
                                                          const QString&,
                                                          const QString&,
                                                          QString&)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigBlockShowWidget(const QString&, int&, void*)),
                    pConnecter, SLOT(slotBlockShowWidget(const QString&, int&, void*)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    return 0;
}

int CConnect::SetViewer(CFrmViewer *pView, bool bDirectConnection)
{
    Q_ASSERT(pView);
    if(!pView) return -1;
    
    if(m_pView)
    {
        this->disconnect(m_pView);
    }
    
    m_pView = pView;
    bool check = false;
    check = connect(this, SIGNAL(sigConnected()), m_pView, SLOT(slotConnected()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSetDesktopSize(int, int)),
                    m_pView, SLOT(slotSetDesktopSize(int, int)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigServerName(const QString&)),
                    m_pView, SLOT(slotSetName(const QString&)));
    Q_ASSERT(check);

    check = connect(this, SIGNAL(sigUpdateRect(const QRect&, const QImage&)),
                    m_pView, SLOT(slotUpdateRect(const QRect&, const QImage&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateRect(const QImage&)),
                    m_pView, SLOT(slotUpdateRect(const QImage&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateCursor(const QCursor&)),
                    m_pView, SLOT(slotUpdateCursor(const QCursor&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateCursorPosition(const QPoint&)),
                    m_pView, SLOT(slotUpdateCursorPosition(const QPoint&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateLedState(unsigned int)),
                    m_pView, SLOT(slotUpdateLedState(unsigned int)));
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
        check = connect(m_pView, SIGNAL(sigMousePressEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMousePressEvent(QMouseEvent*, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigMouseReleaseEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMouseReleaseEvent(QMouseEvent*, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigMouseMoveEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMouseMoveEvent(QMouseEvent*, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigWheelEvent(QWheelEvent*, QPoint)),
                        this, SLOT(slotWheelEvent(QWheelEvent*, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigKeyPressEvent(QKeyEvent*)),
                        this, SLOT(slotKeyPressEvent(QKeyEvent*)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigKeyReleaseEvent(QKeyEvent*)),
                        this, SLOT(slotKeyReleaseEvent(QKeyEvent*)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
    } else {
        check = connect(m_pView, SIGNAL(sigMousePressEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMousePressEvent(QMouseEvent*, QPoint)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigMouseReleaseEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMouseReleaseEvent(QMouseEvent*, QPoint)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigMouseMoveEvent(QMouseEvent*, QPoint)),
                        this, SLOT(slotMouseMoveEvent(QMouseEvent*, QPoint)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigWheelEvent(QWheelEvent*, QPoint)),
                        this, SLOT(slotWheelEvent(QWheelEvent*, QPoint)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigKeyPressEvent(QKeyEvent*)),
                        this, SLOT(slotKeyPressEvent(QKeyEvent*)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigKeyReleaseEvent(QKeyEvent*)),
                        this, SLOT(slotKeyReleaseEvent(QKeyEvent*)));
        Q_ASSERT(check);
    }
    
    return 0;
}

int CConnect::Connect()
{
    qDebug(log) << "CConnect::Connect()";
    int nRet = 0;
    nRet = static_cast<int>(OnInit());
    if(nRet < 0) return nRet;

    if(0 == nRet)
        QTimer::singleShot(0, this, SLOT(slotTimeOut()));
    return 0;
}

int CConnect::Disconnect()
{
    qDebug(log) << "CConnect::Disconnect()";
    int nRet = 0;
    nRet = OnClean();
    return nRet;
}

void CConnect::slotTimeOut()
{
    //qDebug(log) << "CConnect::slotTimeOut()";
    try {
        // >= 0: continue. Call interval
        // = -1: stop
        // < -1: error
        int nTime = OnProcess();
        if(nTime >= 0)
        {
            QTimer::singleShot(nTime, this, SLOT(slotTimeOut()));
            return;
        }
        qCritical(log) << "Process fail or stop:" << nTime;
        if(nTime < -1)
            emit sigError(nTime, "Process fail or stop");
    } catch(std::exception e) {
        qCritical(log) << "Process fail:" << e.what();
        emit sigError(-2, e.what());
    }  catch (...) {
        qCritical(log) << "Process fail";
        emit sigError(-3, "Process fail");
    }

    // Error or stop, must notify user disconnect it
    emit sigDisconnect();
}

int CConnect::OnProcess()
{
    qWarning(log) << "Need to implement CConnect::OnProcess()";
    return 0;
}

void CConnect::slotWheelEvent(QWheelEvent *event, QPoint pos)
{
    QWheelEvent* e = new QWheelEvent(
        pos,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0,0)
        event->globalPosition(),
#else
        event->globalPos(),
#endif
        event->pixelDelta(), event->angleDelta(), event->buttons(),
        event->modifiers(), event->phase(), event->inverted(), event->source());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnect::slotMouseMoveEvent(QMouseEvent *event, QPoint pos)
{
    QMouseEvent* e = new QMouseEvent(event->type(), pos, event->button(),
                                     event->buttons(), event->modifiers());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnect::slotMousePressEvent(QMouseEvent *event, QPoint pos)
{
    QMouseEvent* e = new QMouseEvent(event->type(), pos, event->button(),
                                     event->buttons(), event->modifiers());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnect::slotMouseReleaseEvent(QMouseEvent *event, QPoint pos)
{
    QMouseEvent* e = new QMouseEvent(event->type(), pos, event->button(),
                                     event->buttons(), event->modifiers());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnect::slotKeyPressEvent(QKeyEvent *event)
{
    QKeyEvent* e = new QKeyEvent(event->type(), event->key(),
                                 event->modifiers(), event->text());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnect::slotKeyReleaseEvent(QKeyEvent *event)
{
    QKeyEvent* e = new QKeyEvent(event->type(), event->key(),
                                 event->modifiers(), event->text());
    QCoreApplication::postEvent(this, e);
    WakeUp();
}

void CConnect::mouseMoveEvent(QMouseEvent *event)
{
    qDebug(log) << "Need to implement CConnect::mouseMoveEvent";
}

void CConnect::mousePressEvent(QMouseEvent *event)
{
    qDebug(log) << "Need to implement CConnect::mousePressEvent";
}

void CConnect::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug(log) << "Need to implement CConnect::mouseReleaseEvent";
}

void CConnect::wheelEvent(QWheelEvent *event)
{
    qDebug(log) << "Need to implement CConnect::wheelEvent";
}

void CConnect::keyPressEvent(QKeyEvent *event)
{
    qDebug(log) << "Need to implement CConnect::keyPressEvent";
}

void CConnect::keyReleaseEvent(QKeyEvent *event)
{
    qDebug(log) << "Need to implement CConnect::keyReleaseEvent";
}

int CConnect::WakeUp()
{
    return 0;
}

bool CConnect::event(QEvent *event)
{
    //qDebug(log) << "CConnect::event" << event;
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
    default:
        return QObject::event(event);
    }

    event->accept();
    return true;
}
