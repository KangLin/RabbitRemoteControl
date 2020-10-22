#include "ConnectTigerVnc.h"

#ifndef WIN32
#include <string.h>
#include "network/UnixSocket.h"
#endif

#include <rfb/CMsgWriter.h>
#include <rfb/Hostname.h>
#include "rfb/LogWriter.h"
#include <rfb/util.h>
#include "FramePixelBuffer.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QKeyEvent>
#include <QMouseEvent>

static rfb::LogWriter vlog("ConnectTigerVnc");

CConnectTigerVnc::CConnectTigerVnc(CFrmViewer *pView, QObject *parent)
    : CConnect(pView, parent)
{
//    bool check = connect(pView, SIGNAL(sigMousePressEvent(QMouseEvent*)),
//                    this, SLOT(slotMousePressEvent(QMouseEvent*)));
//    Q_ASSERT(check);
//    check = connect(pView, SIGNAL(sigMouseReleaseEvent(QMouseEvent*)),
//                    this, SLOT(slotMouseReleaseEvent(QMouseEvent*)));
//    Q_ASSERT(check);
//    check = connect(pView, SIGNAL(sigMouseDoubleClickEvent(QMouseEvent*)),
//                    this, SLOT(slotMouseDoubleClickEvent(QMouseEvent*)));
//    Q_ASSERT(check);
//    check = connect(pView, SIGNAL(sigMouseMoveEvent(QMouseEvent*)),
//                    this, SLOT(slotMouseMoveEvent(QMouseEvent*)));
//    Q_ASSERT(check);
//    check = connect(pView, SIGNAL(sigWheelEvent(QWheelEvent*)),
//                    this, SLOT(slotWheelEvent(QWheelEvent*)));
//    Q_ASSERT(check);
//    check = connect(pView, SIGNAL(sigKeyPressEvent(QKeyEvent*)),
//                    this, SLOT(slotKeyPressEvent(QKeyEvent*)));
//    Q_ASSERT(check);
//    check = connect(pView, SIGNAL(sigKeyReleaseEvent(QKeyEvent*)),
//                    this, SLOT(slotKeyReleaseEvent(QKeyEvent*)));
//    Q_ASSERT(check);
}

int CConnectTigerVnc::SetServerName(const QString &serverName)
{
    m_szServerName = serverName;
    try{
#ifndef WIN32
        if (strchr(serverName.toStdString().c_str(), '/') != NULL) {
            m_pSock = new network::UnixSocket(serverName.toStdString().c_str());
            m_szHost = m_pSock->getPeerAddress();
            vlog.info(("Connected to socket %s"), m_szHost.toStdString().c_str());
        } else
#endif
        {
            char* serverHost;
            rfb::getHostAndPort(serverName.toStdString().c_str(),
                                &serverHost, &m_nPort);
            m_szHost = serverHost;        
        }
    } catch (rdr::Exception& e) {
        vlog.error("%s", e.str());
        emit sigError(-1, e.str());
        return -100;
    }
    return 0;
}

int CConnectTigerVnc::Connect()
{
    try{
        m_pSock = new network::TcpSocket(m_szHost.toStdString().c_str(), m_nPort);
        vlog.info("Connected to host %s port %d",
                  m_szHost.toStdString().c_str(), m_nPort);
        // See callback below
        m_pSock->inStream().setBlockCallback(this);
      
        setServerName(m_szServerName.toStdString().c_str());
        setStreams(&m_pSock->inStream(), &m_pSock->outStream());
      
        initialiseProtocol();
    } catch (rdr::Exception& e) {
        vlog.error("%s", e.str());
        emit sigError(-1, e.str());
        return -1;
    }
    return 0;
}

int CConnectTigerVnc::Exec()
{
    int nRet = 0;
    
    try {
        while (!m_bExit) {
            auto in = getInStream();
            if(in)
                in->check(1);
            processMsg();
        }
    } catch (rdr::EndOfStream& e) {
        vlog.info(e.str());
        emit sigError(-1, e.str());
    } catch (rdr::Exception& e) {
        vlog.error(e.str());
        nRet = -1;
        emit sigError(-1, e.str());
    }
    
    emit sigDisconnect();
    
    return nRet;
}

void CConnectTigerVnc::blockCallback()
{}

void CConnectTigerVnc::setColourMapEntries(int firstColour, int nColours, rdr::U16 *rgbs)
{
    vlog.error("Invalid SetColourMapEntries from server!");
}

void CConnectTigerVnc::initDone()
{
    vlog.info("initDone");
    
    emit sigSetDesktopSize(server.width(), server.height());
    
    //Set viewer frame buffer
    setFramebuffer(new CFramePixelBuffer(server.width(), server.height()));
    //TODO: Set server pixmap format
    
    //TODO: Set Preferred Encoding
    
    emit sigConnected();
}

void CConnectTigerVnc::bell()
{
    qApp->beep();
}

void CConnectTigerVnc::setCursor(int width, int height, const rfb::Point &hotspot, const rdr::U8 *data)
{
    
}

void CConnectTigerVnc::getUserPasswd(bool secure, char **user, char **password)
{
    //*user = rfb::strDup(m_szUser.toStdString().c_str());
    *password = rfb::strDup(m_szPassword.toStdString().c_str());
}

bool CConnectTigerVnc::showMsgBox(int flags, const char *title, const char *text)
{
    vlog.error("%s:%s\n", title, text);
    return true;
}

void CConnectTigerVnc::framebufferUpdateEnd()
{
    rfb::CConnection::framebufferUpdateEnd();
    //vlog.debug("CConnectTigerVnc::framebufferUpdateEnd");
    const QImage& img = dynamic_cast<CFramePixelBuffer*>(getFramebuffer())->getImage();
    emit sigUpdateRect(img.rect(), img);
}

void CConnectTigerVnc::dataRect(const rfb::Rect &r, int encoding)
{
    rfb::CConnection::dataRect(r, encoding);
    //vlog.debug("CConnectTigerVnc::dataRect:%d, %d, %d, %d; %d",
    //           r.tl.x, r.tl.y, r.width(), r.height(), encoding);
}

void CConnectTigerVnc::slotMousePressEvent(QMouseEvent* e)
{
    vlog.debug("CConnectTigerVnc::slotMousePressEvent");
    int mask = 0;
    rfb::Point pos(e->x(), e->y());
    if(e->buttons() && Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(e->buttons() && Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(e->buttons() && Qt::MouseButton::RightButton)
        mask |= 0x4;
    this->writer()->writePointerEvent(pos, mask);
}

void CConnectTigerVnc::slotMouseReleaseEvent(QMouseEvent* e)
{
    vlog.debug("CConnectTigerVnc::slotMouseReleaseEvent");
    int mask = 0;
    rfb::Point pos;
    this->writer()->writePointerEvent(pos, mask);
}

void CConnectTigerVnc::slotMouseDoubleClickEvent(QMouseEvent* e)
{}

void CConnectTigerVnc::slotMouseMoveEvent(QMouseEvent* e)
{
    vlog.debug("CConnectTigerVnc::slotMouseMoveEvent");
    int mask = 0;
    rfb::Point pos(e->x(), e->y());
    if(e->buttons() && Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(e->buttons() && Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(e->buttons() && Qt::MouseButton::RightButton)
        mask |= 0x4;
    this->writer()->writePointerEvent(pos, mask);
}

void CConnectTigerVnc::slotWheelEvent(QWheelEvent* e)
{
    vlog.debug("CConnectTigerVnc::slotWheelEvent");
    int mask = 0;
    rfb::Point pos(e->x(), e->y());
    
    if(e->buttons() && Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(e->buttons() && Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(e->buttons() && Qt::MouseButton::RightButton)
        mask |= 0x4;
    
    QPoint p = e->angleDelta();
    if(p.y() < 0)
        mask |= 8;
    if(p.y() > 0)
        mask |= 16;
    if(p.x() < 0)
        mask |= 32;
    if(p.x() > 0)
        mask |= 64;
    this->writer()->writePointerEvent(pos, mask);
}

void CConnectTigerVnc::slotKeyPressEvent(QKeyEvent* e)
{
    this->writer()->writeKeyEvent(e->key(), 0, true);
}

void CConnectTigerVnc::slotKeyReleaseEvent(QKeyEvent* e)
{
    this->writer()->writeKeyEvent(e->key(), 0, false);
}
