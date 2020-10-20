#include "ConnectTigerVnc.h"

#ifndef WIN32
#include <string.h>
#include "network/UnixSocket.h"
#endif

#include <rfb/Hostname.h>
#include "rfb/LogWriter.h"
#include <rfb/util.h>
#include "FramePixelBuffer.h"

#include <QApplication>
#include <QDebug>
#include <QThread>

static rfb::LogWriter vlog("ConnectTigerVnc");

CConnectTigerVnc::CConnectTigerVnc(CFrmViewer *pView, QObject *parent)
    : CConnect(pView, parent)
{
}

int CConnectTigerVnc::SetServerName(const QString &serverName)
{
    m_szServerName = serverName;
    
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
    } catch (rdr::Exception& e) {
        vlog.error(e.str());
        nRet = -1;
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
    //Set server pixmap format
    
    //Set Preferred Encoding
    
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
    vlog.debug("CConnectTigerVnc::framebufferUpdateEnd");
    qDebug() << "framebufferUpdateEnd thread:" << QThread::currentThreadId();
    const QImage& img = dynamic_cast<CFramePixelBuffer*>(getFramebuffer())->getImage();
    emit sigUpdateRect(img.rect(), img);
}

void CConnectTigerVnc::dataRect(const rfb::Rect &r, int encoding)
{
    rfb::CConnection::dataRect(r, encoding);
    vlog.debug("CConnectTigerVnc::dataRect:%d, %d, %d, %d; %d",
               r.tl.x, r.tl.y, r.width(), r.height(), encoding);
}
