// Author: Kang Lin <kl222@126.com>

#include "FrmViewer.h"
#include "Connect.h"

#include <QApplication>
#include <QDebug>
#include <QtPlugin>
#include <QClipboard>
#include <QTimer>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

int g_QtKeyboardModifiers = qRegisterMetaType<Qt::KeyboardModifiers>("KeyboardModifiers");
int g_QtMouseButtons = qRegisterMetaType<Qt::MouseButtons>("MouseButtons");
Q_DECLARE_METATYPE(Qt::MouseButton)
int g_QtMouseButton = qRegisterMetaType<Qt::MouseButton>("MouseButton");

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
    qDebug(Client) << "CConnect::~CConnect()";
}

int CConnect::SetConnecter(CConnecter* pConnecter)
{
    Q_ASSERT(pConnecter);
    if(!pConnecter) return -1;
    
    bool check = false;
    check = connect(this, SIGNAL(sigConnected()),
                         pConnecter, SIGNAL(sigConnected()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnected()),
                    pConnecter, SIGNAL(sigDisconnected()));
    Q_ASSUME(check);
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
    check = connect(this, SIGNAL(sigBlockShowWidget(const QString&, int&, void*)),
                    pConnecter, SLOT(slotBlockShowWidget(const QString&, int&, void*)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigBlockShowMessage(
                                     QString, QString,
                                     QMessageBox::StandardButtons,
                                     QMessageBox::StandardButton&,
                                     bool&, QString)),
                    pConnecter, SLOT(slotBlockShowMessage(
                                         QString, QString,
                                         QMessageBox::StandardButtons,
                                         QMessageBox::StandardButton&,
                                         bool&, QString)),
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
    bool check = connect(this, SIGNAL(sigSetDesktopSize(int, int)),
                    m_pView, SLOT(slotSetDesktopSize(int, int)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigServerName(const QString&)),
                    m_pView, SLOT(slotSetName(const QString&)));
    Q_ASSERT(check);

    check = connect(this, SIGNAL(sigUpdateRect(const QRect&, const QImage&)),
                    m_pView, SLOT(slotUpdateRect(const QRect&, const QImage&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateCursor(const QCursor&)),
                    m_pView, SLOT(slotUpdateCursor(const QCursor&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateCursorPosition(const QPoint&)),
                    m_pView, SLOT(slotUpdateCursorPosition(const QPoint&)));
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
        check = connect(m_pView, SIGNAL(sigMousePressEvent(Qt::MouseButtons, QPoint)),
                        this, SLOT(slotMousePressEvent(Qt::MouseButtons, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigMouseReleaseEvent(Qt::MouseButton, QPoint)),
                        this, SLOT(slotMouseReleaseEvent(Qt::MouseButton, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigMouseMoveEvent(Qt::MouseButtons, QPoint)),
                        this, SLOT(slotMouseMoveEvent(Qt::MouseButtons, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigWheelEvent(Qt::MouseButtons, QPoint, QPoint)),
                        this, SLOT(slotWheelEvent(Qt::MouseButtons, QPoint, QPoint)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigKeyPressEvent(int, Qt::KeyboardModifiers)),
                        this, SLOT(slotKeyPressEvent(int, Qt::KeyboardModifiers)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigKeyReleaseEvent(int, Qt::KeyboardModifiers)),
                        this, SLOT(slotKeyReleaseEvent(int, Qt::KeyboardModifiers)),
                        Qt::DirectConnection);
        Q_ASSERT(check);
    } else {
        check = connect(m_pView, SIGNAL(sigMousePressEvent(Qt::MouseButtons, QPoint)),
                        this, SLOT(slotMousePressEvent(Qt::MouseButtons, QPoint)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigMouseReleaseEvent(Qt::MouseButton, QPoint)),
                        this, SLOT(slotMouseReleaseEvent(Qt::MouseButton, QPoint)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigMouseMoveEvent(Qt::MouseButtons, QPoint)),
                        this, SLOT(slotMouseMoveEvent(Qt::MouseButtons, QPoint)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigWheelEvent(Qt::MouseButtons, QPoint, QPoint)),
                        this, SLOT(slotWheelEvent(Qt::MouseButtons, QPoint, QPoint)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigKeyPressEvent(int, Qt::KeyboardModifiers)),
                        this, SLOT(slotKeyPressEvent(int, Qt::KeyboardModifiers)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigKeyReleaseEvent(int, Qt::KeyboardModifiers)),
                        this, SLOT(slotKeyReleaseEvent(int, Qt::KeyboardModifiers)));
        Q_ASSERT(check);
    }
    
    return 0;
}

int CConnect::Connect()
{
    int nRet = 0;
    nRet = OnInit();
    if(nRet < 0) return nRet;

    if(0 == nRet)
        QTimer::singleShot(0, this, SLOT(slotTimeOut()));
    return 0;
}

int CConnect::Disconnect()
{
    int nRet = 0;
    nRet = OnClean();
    emit sigDisconnected();
    if(nRet) return nRet;
    
    return nRet;
}

void CConnect::slotTimeOut()
{
    try {
        // >= 0 : continue
        // <  0: error or stop
        int nTime = OnProcess();
        if(nTime >= 0)
        {
            QTimer::singleShot(nTime, this, SLOT(slotTimeOut()));
            return;
        }
        qCritical(Client) << "Process fail:" << nTime;
    } catch(std::exception e) {
        qCritical(Client) << "Process fail:" << e.what();
        emit sigError(-1, e.what());
    }  catch (...) {
        qCritical(Client) << "Process fail";
        emit sigError(-2, "");
    }

    // Error or stop, must notify user disconnect it
    emit sigDisconnected();
}

int CConnect::OnProcess()
{
    qDebug(Client) << "Need to implement CConnect::OnProcess()";
    return 0;
}

void CConnect::slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta)
{
    qDebug(Client) << "CConnect::slotWheelEvent";
}

void CConnect::slotMouseMoveEvent(Qt::MouseButtons, QPoint)
{
    qDebug(Client) << "CConnect::slotMouseMoveEvent";
}

void CConnect::slotMousePressEvent(Qt::MouseButtons, QPoint)
{
    qDebug(Client) << "CConnect::slotMousePressEvent";
}

void CConnect::slotMouseReleaseEvent(Qt::MouseButton, QPoint)
{
    qDebug(Client) << "CConnect::slotMouseReleaseEvent";
}

void CConnect::slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers)
{
    qDebug(Client) << "CConnect::slotKeyPressEvent";
}

void CConnect::slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers)
{
    qDebug(Client) << "CConnect::slotKeyReleaseEvent";
}
