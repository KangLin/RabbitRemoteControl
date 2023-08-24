// Author: Kang Lin <kl222@126.com>

#include "ConnectRabbitVNC.h"
#include "ConnecterRabbitVNC.h"

#ifndef WIN32
#include <string.h>
#include "network/UnixSocket.h"
#endif

#include <rfb/CMsgWriter.h>
#include <rfb/Hostname.h>
#include "rfb/LogWriter.h"
#include <rfb/util.h>
#include <rfb/clipboardTypes.h>

// the X11 headers on some systems
#ifndef XK_VoidSymbol
#define XK_LATIN1
#define XK_MISCELLANY
#define XK_XKB_KEYS
#include <rfb/keysymdef.h>
#endif

#include "FramePixelBuffer.h"

#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QNetworkProxy>
#include <QMutex>
#include <QInputDialog>

#include "rfb/Security.h"
#ifdef HAVE_GNUTLS
#include "rfb/CSecurityTLS.h"
#endif

#ifdef HAVE_ICE
#include "ICE/Ice.h"
#include "ICE/ChannelIce.h"
#endif

Q_DECLARE_LOGGING_CATEGORY(RabbitVNC)

// 8 colours (1 bit per component)
static const rfb::PixelFormat verylowColourPF(8, 3,false, true, 1, 1, 1, 2, 1, 0);
// 64 colours (2 bits per component)
static const rfb::PixelFormat lowColourPF(8, 6, false, true, 3, 3, 3, 4, 2, 0);
// 256 colours (2-3 bits per component)
static const rfb::PixelFormat mediumColourPF(8, 8, false, true, 7, 7, 3, 5, 2, 0);

// RGB (4 bits per component)
static const rfb::PixelFormat fullColourPF(32, 24, false, true, 255, 255, 255, 16, 8, 0);

// Time new bandwidth estimates are weighted against (in ms)
static const unsigned bpsEstimateWindow = 1000;

/* 因为此事件循环为Qt事件循环，不会被阻塞。
 * 并具 Socket 不能在不同的线程中调用，所以这里 bDirectConnection 设置成 false
 */
CConnectRabbitVNC::CConnectRabbitVNC(CConnecterRabbitVNC *pConnecter, QObject *parent)
    : CConnect(pConnecter, parent, false),
      m_bpsEstimate(20000000),
      m_updateCount(0),
      m_pPara(nullptr)
{
    security.setUserPasswdGetter(this);
    
#ifdef HAVE_GNUTLS
    rfb::CSecurityTLS::msg = this;
#endif
    
    m_pPara = dynamic_cast<CParameterRabbitVNC*>(pConnecter->GetParameter());
    Q_ASSERT(m_pPara);
    
    setShared(m_pPara->GetShared());
    supportsLocalCursor = m_pPara->GetLocalCursor();
    
    // Set Preferred Encoding
    setPreferredEncoding(m_pPara->GetEncoding());
    setCompressLevel(m_pPara->GetCompressLevel());
    setQualityLevel(m_pPara->GetQualityLevel());
    
    // Set server pixmap format
    updatePixelFormat();
    
    if(!m_pPara->GetLocalCursor())
    {
        emit sigUpdateCursor(QCursor(Qt::BlankCursor));
    }
}

CConnectRabbitVNC::~CConnectRabbitVNC()
{
    if(m_DataChannel)
        m_DataChannel->close();
    qDebug(RabbitVNC) << "CConnectRabbitVNC::~CConnectRabbitVNC()";
}

/*
 * \return 
 * \li < 0: error
 * \li = 0: Use OnProcess (non-Qt event loop)
 * \li > 0: Don't use OnProcess (qt event loop)
 */
int CConnectRabbitVNC::OnInit()
{
    int nRet = 1;
    if(m_pPara->GetIce())
        nRet = IceInit();
    else
        nRet = SocketInit();
    if(nRet) return nRet;
    return 1;
}

int CConnectRabbitVNC::IceInit()
{
#ifdef HAVE_ICE
    CIceSignal* pSignal = CICE::Instance()->GetSignal().data();
    if(!pSignal || !pSignal->IsConnected())
    {
        qCritical(RabbitVNC) << "The g_pSignal is null. or signal don't connect server";
        return -1;
    }
    
    qInfo(RabbitVNC) << "Connected to signal server:"
                      << CICE::Instance()->GetParameter()->getSignalServer()
                      << ":"
                      << CICE::Instance()->GetParameter()->getSignalPort()
                      << CICE::Instance()->GetParameter()->getSignalUser();
    auto channel = QSharedPointer<CChannelIce>(new CChannelIce(pSignal));
    if(!channel)
    {
        qCritical(RabbitVNC) << "new CDataChannelIce fail";
        return -2;
    }
    m_DataChannel = channel;
    SetChannelConnect(channel);
    
    rtc::Configuration config;
    rtc::IceServer stun(CICE::Instance()->GetParameter()->getStunServer().toStdString().c_str(),
                        CICE::Instance()->GetParameter()->getStunPort());
    rtc::IceServer turn(CICE::Instance()->GetParameter()->getTurnServer().toStdString().c_str(),
                        CICE::Instance()->GetParameter()->getTurnPort(),
                        CICE::Instance()->GetParameter()->getTurnUser().toStdString().c_str(),
                        CICE::Instance()->GetParameter()->getTurnPassword().toStdString().c_str());
    config.iceServers.push_back(stun);
    config.iceServers.push_back(turn);
    channel->SetConfigure(config);
    
    bool bRet = channel->open(CICE::Instance()->GetParameter()->getSignalUser(),
                              m_pPara->GetPeerUser(), true);
    if(!bRet)
    {
        emit sigError(-1, "Open channel fail");
    }
    
#endif
    return 0;
}

int CConnectRabbitVNC::SocketInit()
{
    int nRet = 0;
    try{
        m_DataChannel = QSharedPointer<CChannel>(new CChannel());
        if(!m_DataChannel) return -1;
        
        // The pointer is freed by CChannel. see: CChannel::open
        QTcpSocket* pSock = new QTcpSocket(this);
        if(!pSock) return -2;
        if(!m_DataChannel->open(pSock, QIODevice::ReadWrite))
        {
            qCritical(RabbitVNC) << "Open channel fail";
            return -3;
        }
        
        SetChannelConnect(m_DataChannel);
        
        QNetworkProxy::ProxyType type = QNetworkProxy::NoProxy;
        // Set sock
        switch(m_pPara->GetProxyType())
        {
        case CParameterConnecter::emProxy::SocksV4:
            break;
        case CParameterConnecter::emProxy::SocksV5:
            type = QNetworkProxy::Socks5Proxy;
            break;
        case CParameterConnecter::emProxy::Http:
            type = QNetworkProxy::HttpProxy;
            break;
        case CParameterConnecter::emProxy::No:
            break;
        default:
            break;
        }
        
        if(QNetworkProxy::NoProxy != type)
        {
            QNetworkProxy proxy;
            proxy.setType(type);
            proxy.setHostName(m_pPara->GetProxyHost());
            proxy.setPort(m_pPara->GetProxyPort());
            proxy.setUser(m_pPara->GetProxyUser());
            proxy.setPassword(m_pPara->GetProxyPassword());
            pSock->setProxy(proxy);
        }
        
        pSock->connectToHost(m_pPara->GetHost(), m_pPara->GetPort());
        
        return nRet;
    } catch (rdr::Exception& e) {
        qCritical(RabbitVNC) << "SocketInit" << e.str();
        emit sigError(-4, e.str());
        nRet = -4;
    }
    return nRet;
}

int CConnectRabbitVNC::SetChannelConnect(QSharedPointer<CChannel> channel)
{
    bool check = false;
    check = connect(channel.data(), SIGNAL(sigConnected()),
                    this, SLOT(slotConnected()));
    Q_ASSERT(check);
    check = connect(channel.data(), SIGNAL(sigDisconnected()),
                    this, SLOT(slotDisConnected()));
    Q_ASSERT(check);
    check = connect(channel.data(), SIGNAL(readyRead()),
                    this, SLOT(slotReadyRead()));
    Q_ASSERT(check);
    check = connect(channel.data(), SIGNAL(sigError(int, const QString&)),
                    this, SLOT(slotError(int, const QString&)));
    Q_ASSERT(check);
    return 0;
}

int CConnectRabbitVNC::OnClean()
{
    close();
    setStreams(nullptr, nullptr);
    if(m_DataChannel)
        m_DataChannel->close();
    //emit sigDisconnected();
    return 0;
}

void CConnectRabbitVNC::slotConnected()
{
    if(m_pPara->GetIce())
        qInfo(RabbitVNC) << "Connected to peer"
                          << m_pPara->GetPeerUser().toStdString().c_str();
    else
        qInfo(RabbitVNC) << "Connected to"
                          << m_pPara->GetHost() << ":" << m_pPara->GetPort();

    m_InStream = QSharedPointer<rdr::InStream>(new CInStreamChannel(m_DataChannel.data()));
    m_OutStream = QSharedPointer<rdr::OutStream>(new COutStreamChannel(m_DataChannel.data()));
    setStreams(m_InStream.data(), m_OutStream.data());
    initialiseProtocol();
}

void CConnectRabbitVNC::slotDisConnected()
{
    qInfo(RabbitVNC) << "slotDisConnected to"
                      << m_pPara->GetHost() << ":" << m_pPara->GetPort();
    emit sigDisconnected();
}

void CConnectRabbitVNC::slotReadyRead()
{
    //qDebug(RabbitVNC) << "CConnectRabbitVnc::slotReadyRead";
    try {
        while(processMsg())
            ;
        return;
    } catch (rdr::EndOfStream& e) {
        qCritical(RabbitVNC) << e.str();
        emit sigError(-1, e.str());
    } catch(rdr::GAIException &e) {
        qCritical(RabbitVNC) << e.err << e.str();
        emit sigError(-1, e.str());
    } catch(rdr::SystemException &e) {
        qCritical(RabbitVNC) << e.err << e.str();
        emit sigError(-1, e.str());
    } catch (rdr::Exception& e) {
        qCritical(RabbitVNC) << e.str();
        emit sigError(-1, e.str());
    } catch (std::exception &e) {
        qCritical(RabbitVNC) << e.what();
        emit sigError(-1, e.what());
    } catch(...) {
        qCritical(RabbitVNC) << "processMsg error";
        emit sigError(-1);
    }
    return;
    emit sigDisconnected();
}

void CConnectRabbitVNC::slotError(int nErr, const QString& szErr)
{
    qCritical(RabbitVNC) << "Error:" << nErr << "-" << szErr;
    emit sigError(nErr, szErr);
    emit sigDisconnected();
}

void CConnectRabbitVNC::initDone()
{
    Q_ASSERT(m_pPara);
    qDebug(RabbitVNC) <<
                    "CConnectRabbitVnc::initDone():name:"
                    << server.name()
                    << "width:" << server.width()
                    << "height:" << server.height();
    
    // If using AutoSelect with old servers, start in FullColor
    // mode. See comment in autoSelectFormatAndEncoding. 
    if (server.beforeVersion(3, 8) && m_pPara->GetAutoSelect())
    {
        m_pPara->SetColorLevel(CParameterRabbitVNC::Full);
        updatePixelFormat();
    }
    
    emit sigSetDesktopSize(server.width(), server.height());
    QString szName = QString::fromUtf8(server.name());
    
    emit sigServerName(szName);
    
    //Set viewer frame buffer
    setFramebuffer(new CFramePixelBuffer(server.width(), server.height()));
    
    emit sigConnected();
}

void CConnectRabbitVNC::setColourMapEntries(int firstColour,
                                            int nColours, rdr::U16 *rgbs)
{
    qCritical(RabbitVNC) << "Invalid SetColourMapEntries from server!";
}

void CConnectRabbitVNC::bell()
{
    qApp->beep();
}

void CConnectRabbitVNC::setCursor(int width, int height,
                                  const rfb::Point &hotspot, const rdr::U8 *data)
{
    //qDebug(RabbitVNC) << "CConnectRabbitVnc::setCursor:" << hotspot.x << "," << hotspot.y;
    if ((width == 0) || (height == 0)) {
        QImage cursor(1, 1, QImage::Format_ARGB32);
        rdr::U8 *buffer = cursor.bits();
        memset(buffer, 0, 4);
        emit sigUpdateCursor(QCursor(QPixmap::fromImage(cursor), hotspot.x, hotspot.y));
    } else {
        QImage cursor(width, height, QImage::Format_ARGB32);
        rdr::U8 *buffer = cursor.bits();
        memcpy(buffer, data, width * height * 4);
        emit sigUpdateCursor(QCursor(QPixmap::fromImage(cursor), hotspot.x, hotspot.y));
    }
}

void CConnectRabbitVNC::setCursorPos(const rfb::Point &pos)
{
    //qDebug(RabbitVNC) << "CConnectRabbitVnc::setCursorPos:x:" << pos.x << ",y:" << pos.y;
    emit sigUpdateCursorPosition(QPoint(pos.x, pos.y));
}

void CConnectRabbitVNC::getUserPasswd(bool secure, char **user, char **password)
{
    if(password && !*password)
    {
        *password = rfb::strDup(m_pPara->GetPassword().toStdString().c_str());
        if(m_pPara->GetPassword().isEmpty())
        {
            int nRet = QDialog::Rejected;
            emit sigBlockShowWidget("CDlgGetPasswordRabbitVNC", nRet, m_pPara);
            if(QDialog::Accepted == nRet)
            {
                *password = rfb::strDup(m_pPara->GetPassword().toStdString().c_str());
            }
        }
    }
}

bool CConnectRabbitVNC::showMsgBox(int flags, const char *title, const char *text)
{
    qDebug(RabbitVNC) << title << text;
    QMessageBox::StandardButton nRet = QMessageBox::No;
    bool bCheckBox = 0;
    emit sigBlockShowMessage(QString(title), QString(text),
                             QMessageBox::Ok | QMessageBox::No, nRet, bCheckBox);
    if(QMessageBox::Ok == nRet)
        return true;
    return false;
}

// framebufferUpdateStart() is called at the beginning of an update.
// Here we try to send out a new framebuffer update request so that the
// next update can be sent out in parallel with us decoding the current
// one.
void CConnectRabbitVNC::framebufferUpdateStart()
{
    CConnection::framebufferUpdateStart();
    
    // For bandwidth estimate
    gettimeofday(&updateStartTime, NULL);
    m_updateStartPos = m_InStream->pos();
}

// framebufferUpdateEnd() is called at the end of an update.
// For each rectangle, the FdInStream will have timed the speed
// of the connection, allowing us to select format and encoding
// appropriately, and then request another incremental update.
void CConnectRabbitVNC::framebufferUpdateEnd()
{
    unsigned long long elapsed, bps, weight;
    struct timeval now;
    
    rfb::CConnection::framebufferUpdateEnd();
    //qDebug(RabbitVNC) << "CConnectRabbitVnc::framebufferUpdateEnd";
    
    m_updateCount++;
    
    // Calculate bandwidth everything managed to maintain during this update
    gettimeofday(&now, NULL);
    elapsed = (now.tv_sec - updateStartTime.tv_sec) * 1000000;
    elapsed += now.tv_usec - updateStartTime.tv_usec;
    if (elapsed == 0)
        elapsed = 1;
    bps = (unsigned long long)(m_InStream->pos() -
                               m_updateStartPos) * 8 *
            1000000 / elapsed;
    // Allow this update to influence things more the longer it took, to a
    // maximum of 20% of the new value.
    weight = elapsed * 1000 / bpsEstimateWindow;
    if (weight > 200000)
        weight = 200000;
    m_bpsEstimate = ((m_bpsEstimate * (1000000 - weight)) +
                     (bps * weight)) / 1000000;
    
    if(m_pPara && m_pPara->GetBufferEndRefresh())
    {
        const QImage& img = dynamic_cast<CFramePixelBuffer*>(getFramebuffer())->getImage();
        emit sigUpdateRect(img.rect(), img);
    }
    
    // Compute new settings based on updated bandwidth values
    if (m_pPara && m_pPara->GetAutoSelect())
        autoSelectFormatAndEncoding();
}

// autoSelectFormatAndEncoding() chooses the format and encoding appropriate
// to the connection speed:
//
//   First we wait for at least one second of bandwidth measurement.
//
//   Above 16Mbps (i.e. LAN), we choose the second highest JPEG quality,
//   which should be perceptually lossless.
//
//   If the bandwidth is below that, we choose a more lossy JPEG quality.
//
//   If the bandwidth drops below 256 Kbps, we switch to palette mode.
//
//   Note: The system here is fairly arbitrary and should be replaced
//         with something more intelligent at the server end.
//
void CConnectRabbitVNC::autoSelectFormatAndEncoding()
{
    bool newFullColour = m_pPara->GetColorLevel() == CParameterRabbitVNC::Full ? true : false;
    int newQualityLevel = m_pPara->GetQualityLevel();
    
    // Always use Tight
    setPreferredEncoding(rfb::encodingTight);
    
    // Select appropriate quality level
    if (!m_pPara->GetNoJpeg()) {
        if (m_bpsEstimate > 16000)
            newQualityLevel = 8;
        else
            newQualityLevel = 6;
        
        if (newQualityLevel != m_pPara->GetQualityLevel()) {
            qInfo(RabbitVNC) << "Throughput" << (int)(m_bpsEstimate/1000) << "kbit/s"
                              << "- changing to quality" << newQualityLevel;
            m_pPara->SetQualityLevel(newQualityLevel);
            setQualityLevel(newQualityLevel);
        }
    }
    
    if (server.beforeVersion(3, 8)) {
        // Xvnc from TightVNC 1.2.9 sends out FramebufferUpdates with
        // cursors "asynchronously". If this happens in the middle of a
        // pixel format change, the server will encode the cursor with
        // the old format, but the client will try to decode it
        // according to the new format. This will lead to a
        // crash. Therefore, we do not allow automatic format change for
        // old servers.
        return;
    }
    
    // Select best color level
    newFullColour = (m_bpsEstimate > 256);
    if (newFullColour != (0 == m_pPara->GetColorLevel())) {
        if (newFullColour)
            qInfo(RabbitVNC) << "Throughput" << m_bpsEstimate / 1000 << "kbit/s - full color is now enabled";
        else
            qInfo(RabbitVNC) << "Throughput" << m_bpsEstimate / 1000 << "kbit/s - full color is now disabled";
        m_pPara->SetColorLevel(newFullColour ? CParameterRabbitVNC::Full : CParameterRabbitVNC::Low);
        updatePixelFormat();
    } 
}

// requestNewUpdate() requests an update from the server, having set the
// format and encoding appropriately.
void CConnectRabbitVNC::updatePixelFormat()
{
    Q_ASSERT(m_pPara);
    
    if(!m_pPara) return;
    rfb::PixelFormat pf;
    
    switch (m_pPara->GetColorLevel()) {
    case CParameterRabbitVNC::Full:
        pf = fullColourPF;
        break;
    case CParameterRabbitVNC::Medium:
        pf = mediumColourPF;
        break;
    case CParameterRabbitVNC::Low:
        pf = lowColourPF;
        break;
    case CParameterRabbitVNC::VeryLow:
        pf = verylowColourPF;
        break;
    }
    
    char str[256];
    pf.print(str, 256);
    qDebug(RabbitVNC) << "Using pixel format" << str;
    setPF(pf);
}

bool CConnectRabbitVNC::dataRect(const rfb::Rect &r, int encoding)
{
    if(!rfb::CConnection::dataRect(r, encoding))
        return false;
    /*
    qDebug(RabbitVNC, "CConnectRabbitVnc::dataRect:%d, %d, %d, %d; %d",
                   r.tl.x, r.tl.y, r.width(), r.height(), encoding); //*/
    // 立即更新图像
    if(m_pPara && !m_pPara->GetBufferEndRefresh())
    {
        const QImage& img = dynamic_cast<CFramePixelBuffer*>(getFramebuffer())->getImage();
        emit sigUpdateRect(img.rect(), img);
    }
    return true;
}

void CConnectRabbitVNC::slotMousePressEvent(Qt::MouseButtons buttons, QPoint pos)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    unsigned char mask = 0;
    rfb::Point p(pos.x(), pos.y());
    if(buttons & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(buttons & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(buttons & Qt::MouseButton::RightButton)
        mask |= 0x4;
    
    //qDebug(RabbitVNC) << "CConnectRabbitVnc::slotMousePressEvent" << buttons << pos << mask;
    writer()->writePointerEvent(p, mask);
}

void CConnectRabbitVNC::slotMouseReleaseEvent(Qt::MouseButton button, QPoint pos)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    int mask = 0;
    rfb::Point p(pos.x(), pos.y());
    //qDebug(RabbitVNC) << "CConnectRabbitVnc::slotMouseReleaseEvent" << button << pos << mask;
    writer()->writePointerEvent(p, mask);
}

void CConnectRabbitVNC::slotMouseMoveEvent(Qt::MouseButtons buttons, QPoint pos)
{
    //qDebug(RabbitVNC) << "CConnectRabbitVnc::slotMouseMoveEvent" << buttons << pos;
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    int mask = 0;
    rfb::Point p(pos.x(), pos.y());
    if(buttons & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(buttons & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(buttons & Qt::MouseButton::RightButton)
        mask |= 0x4;
    writer()->writePointerEvent(p, mask);
}

void CConnectRabbitVNC::slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta)
{
    //qDebug(RabbitVNC) << "CConnectRabbitVnc::slotWheelEvent" << buttons << pos;
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    int mask = 0;
    rfb::Point p(pos.x(), pos.y());
    
    if(buttons & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(buttons & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(buttons & Qt::MouseButton::RightButton)
        mask |= 0x4;
    
    QPoint d = angleDelta;
    if(d.y() > 0)
        mask |= 0x8;
    if(d.y() < 0)
        mask |= 0x10;
    if(d.x() < 0)
        mask |= 0x20;
    if(d.x() > 0)
        mask |= 0x40;
    
    writer()->writePointerEvent(p, mask);
}

void CConnectRabbitVNC::slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    bool modifier = true;
    if (modifiers == Qt::NoModifier)
        modifier = false;
    //qDebug(RabbitVNC) << "slotKeyPressEvent key" << key << modifiers;
    uint32_t k = TranslateRfbKey(key, modifier);
    if(key)
        writer()->writeKeyEvent(k, 0, true);
}

void CConnectRabbitVNC::slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers)
{
    if(m_pPara && m_pPara->GetOnlyView()) return;
    if(!writer()) return;
    bool modifier = true;
    if (modifiers == Qt::NoModifier)
        modifier = false;
    //qDebug(RabbitVNC) << "slotKeyReleaseEvent key" << key << modifiers;
    uint32_t k = TranslateRfbKey(key, modifier);
    if(key)
        writer()->writeKeyEvent(k, 0, false);
}

/**
 * @brief CConnectRabbitVnc::TranslateRfbKey
 * @param inkey
 * @param modifier
 * @return 
 * @see https://github.com/rfbproto/rfbproto/blob/master/rfbproto.rst#keyevent
 */
quint32 CConnectRabbitVNC::TranslateRfbKey(quint32 inkey, bool modifier)
{
    quint32 k = 5000;
    
    switch (inkey)
    {
    case Qt::Key_Backspace: k = XK_BackSpace; break;
    case Qt::Key_Tab: k = XK_Tab; break;
    case Qt::Key_Clear: k = XK_Clear; break;
    case Qt::Key_Return: k = XK_Return; break;
    case Qt::Key_Pause: k = XK_Pause; break;
    case Qt::Key_Escape: k = XK_Escape; break;
    case Qt::Key_Space: k = XK_space; break;
    case Qt::Key_Delete: k = XK_Delete; break;
    case Qt::Key_Period: k = XK_period; break;
    
    /* International & multi-key character composition */
    case Qt::Key_Multi_key: k = XK_Multi_key; break;
    case Qt::Key_Codeinput: k = XK_Codeinput; break;
    case Qt::Key_SingleCandidate: k = XK_SingleCandidate; break;
    case Qt::Key_MultipleCandidate: k = XK_MultipleCandidate; break;	 
    case Qt::Key_PreviousCandidate: k = XK_PreviousCandidate; break;

    /* Japanese keyboard support */
    case Qt::Key_Kanji: k = XK_Kanji; break;
    case Qt::Key_Muhenkan: k = XK_Muhenkan; break;
    case Qt::Key_Henkan: k = XK_Henkan; break;
    case Qt::Key_Romaji: k = XK_Romaji; break;	 
    case Qt::Key_Hiragana: k = XK_Hiragana; break;
    case Qt::Key_Katakana: k = XK_Katakana; break;	 
    case Qt::Key_Hiragana_Katakana:	k = XK_Hiragana_Katakana;break;
    case Qt::Key_Zenkaku: k = XK_Zenkaku; break;
    case Qt::Key_Hankaku: k = XK_Hankaku; break;
    case Qt::Key_Zenkaku_Hankaku: k = XK_Zenkaku_Hankaku; break;
    case Qt::Key_Touroku: k = XK_Touroku; break;
    case Qt::Key_Massyo: k = XK_Massyo; break;
    case Qt::Key_Kana_Lock: k = XK_Kana_Lock; break;
    case Qt::Key_Kana_Shift: k = XK_Kana_Shift; break;
    case Qt::Key_Eisu_Shift: k = XK_Eisu_Shift; break;
    case Qt::Key_Eisu_toggle: k = XK_Eisu_toggle; break;
        
    //special keyboard char
    case Qt::Key_Exclam: k = XK_exclam; break; //!
    case Qt::Key_QuoteDbl: k = XK_quotedbl; break; //"
    case Qt::Key_NumberSign: k = XK_numbersign; break; //#
    case Qt::Key_Percent: k = XK_percent; break; //%
    case Qt::Key_Dollar: k = XK_dollar; break;   //$
    case Qt::Key_Ampersand: k = XK_ampersand; break; //&
    case Qt::Key_Apostrophe: k = XK_apostrophe; break;//!
    case Qt::Key_ParenLeft: k = XK_parenleft; break; // (
    case Qt::Key_ParenRight: k = XK_parenright; break; // )
        
    case Qt::Key_Slash: k = XK_slash; break;    // /
    case Qt::Key_Asterisk: k = XK_asterisk; break;  //*
    case Qt::Key_Minus: k = XK_minus; break;    //-
    case Qt::Key_Plus: k = XK_plus; break;  //+
    case Qt::Key_Enter: k = XK_Return; break;   //
    case Qt::Key_Equal: k = XK_equal; break;    //=
    case Qt::Key_Comma: return XK_comma; //,
        
    case Qt::Key_Colon: k = XK_colon;break; // :
    case Qt::Key_Semicolon: k = XK_semicolon; break; //;
    case Qt::Key_Less: k = XK_less; break; // <
    case Qt::Key_Greater: k = XK_greater; break; // >
    case Qt::Key_Question: k = XK_question; break; //?
    case Qt::Key_At: k = XK_at; break; //@
        
    case Qt::Key_BracketLeft: k = XK_bracketleft; break;
    case Qt::Key_Backslash: k = XK_backslash;break;
    case Qt::Key_BracketRight: k = XK_bracketright;break;
    case Qt::Key_AsciiCircum: k = XK_asciicircum;break;
    case Qt::Key_Underscore: k = XK_underscore;break;
    case Qt::Key_QuoteLeft: k = XK_quoteleft;break;
    case Qt::Key_BraceLeft: k = XK_braceleft;break;
    case Qt::Key_Bar: k = XK_bar; break;
    case Qt::Key_BraceRight: k = XK_braceright;break;
    case Qt::Key_AsciiTilde: k = XK_asciitilde;break;
    case Qt::Key_nobreakspace: k = XK_nobreakspace;break;
    case Qt::Key_exclamdown: k = XK_exclamdown;break;
    case Qt::Key_cent: k = XK_cent;break;
    case Qt::Key_sterling: k = XK_sterling;break;
    case Qt::Key_currency: k = XK_currency;break;
    case Qt::Key_yen: k = XK_yen;break;
    case Qt::Key_brokenbar: k = XK_brokenbar;break;
    case Qt::Key_section: k = XK_section;break;
    case Qt::Key_diaeresis: k = XK_diaeresis;break;
    case Qt::Key_copyright: k = XK_copyright; break;
    case Qt::Key_ordfeminine: k = XK_ordfeminine; break;
    case Qt::Key_guillemotleft: k = XK_guillemotleft; break;
    case Qt::Key_guillemotright: k = XK_guillemotright; break;
    case Qt::Key_notsign: k = XK_notsign; break;
    case Qt::Key_hyphen: k = XK_hyphen; break;
    case Qt::Key_registered: k = XK_registered; break;
        
    case Qt::Key_Up: k = XK_Up; break;
    case Qt::Key_Down: k = XK_Down; break;
    case Qt::Key_Right: k = XK_Right; break;
    case Qt::Key_Left: k = XK_Left; break;
    case Qt::Key_Insert: k = XK_Insert; break;
    case Qt::Key_Home: k = XK_Home; break;
    case Qt::Key_End: k = XK_End; break;
    case Qt::Key_PageUp: k = XK_Page_Up; break;
    case Qt::Key_PageDown: k = XK_Page_Down; break;
    case Qt::Key_MediaPrevious: k = XK_Prior; break;
    case Qt::Key_MediaNext: k = XK_Next; break;
    case Qt::Key_MediaPlay: k = XK_Begin; break;

    /* Misc Functions */
    case Qt::Key_Select: k = XK_Select; break;
    case Qt::Key_Printer: k = XK_Print; break;
    case Qt::Key_Execute: k = XK_Execute; break;
    case Qt::Key_Undo: k = XK_Undo; break;
    case Qt::Key_Redo: k = XK_Redo; break;
    case Qt::Key_Menu: k = XK_Menu;break;
    case Qt::Key_Find: k = XK_Find; break;
    case Qt::Key_Exit:	 
    case Qt::Key_Cancel:
    case Qt::Key_Stop:
        k = XK_Cancel;
        break;
    case Qt::Key_Mode_switch: k = XK_Mode_switch; break;
        
    case Qt::Key_F1: k = XK_F1; break;
    case Qt::Key_F2: k = XK_F2; break;
    case Qt::Key_F3: k = XK_F3; break;
    case Qt::Key_F4: k = XK_F4; break;
    case Qt::Key_F5: k = XK_F5; break;
    case Qt::Key_F6: k = XK_F6; break;
    case Qt::Key_F7: k = XK_F7; break;
    case Qt::Key_F8: k = XK_F8; break;
    case Qt::Key_F9: k = XK_F9; break;
    case Qt::Key_F10: k = XK_F10; break;
    case Qt::Key_F11: k = XK_F11; break;
    case Qt::Key_F12: k =  XK_F12; break;
    case Qt::Key_F13: k = XK_F13; break;
    case Qt::Key_F14: k = XK_F14; break;
    case Qt::Key_F15: k = XK_F15; break;
    case Qt::Key_F16: k = XK_F16; break;
    case Qt::Key_F17: k = XK_F17; break;
    case Qt::Key_F18: k = XK_F18; break;
    case Qt::Key_F19: k = XK_F19; break;
    case Qt::Key_F20: k = XK_F20; break;
    case Qt::Key_F21: k = XK_F21; break;
    case Qt::Key_F22: k = XK_F22; break;
    case Qt::Key_F23: k = XK_F23; break;
    case Qt::Key_F24: k = XK_F24; break;
    case Qt::Key_F25: k = XK_F25; break;
    case Qt::Key_F26: k = XK_F26; break;
    case Qt::Key_F27: k = XK_F27; break;
    case Qt::Key_F28: k = XK_F28; break;
    case Qt::Key_F29: k = XK_F29; break;
    case Qt::Key_F30: k = XK_F30; break;
    case Qt::Key_F31: k = XK_F31; break;
    case Qt::Key_F32: k = XK_F32; break;
    case Qt::Key_F33: k = XK_F33; break;
    case Qt::Key_F34: k = XK_F34; break;
    case Qt::Key_F35: k = XK_F35; break;

    case Qt::Key_NumLock: k = XK_Num_Lock; break;
    case Qt::Key_CapsLock: k = XK_Caps_Lock; break;
    case Qt::Key_ScrollLock: k = XK_Scroll_Lock; break;
        
    case Qt::Key_Shift: k = XK_Shift_R; break; //k = XK_Shift_L; break;
    case Qt::Key_Control: k = XK_Control_R; break;// k = XK_Control_L; break;
    case Qt::Key_Alt: k = XK_Alt_R; break;//k = XK_Alt_L; break;
    case Qt::Key_Meta: k = XK_Meta_R; break;//k = XK_Meta_L; break;*/
        
    case Qt::Key_Super_L: k = XK_Super_L; break;		/* left "windows" key */
    case Qt::Key_Super_R: k = XK_Super_R; break;		/* right "windows" key */

    case Qt::Key_Help: k = XK_Help; break;
    case Qt::Key_Print: k = XK_Print; break;
    case Qt::Key_SysReq: k = XK_Sys_Req; break;
    case Qt::Key_0: k = XK_0;break;
    case Qt::Key_1: k = XK_1;break;
    case Qt::Key_2: k = XK_2;break;
    case Qt::Key_3: k = XK_3;break;
    case Qt::Key_4: k = XK_4;break;
    case Qt::Key_5: k = XK_5;break;
    case Qt::Key_6: k = XK_6;break;
    case Qt::Key_7: k = XK_7;break;
    case Qt::Key_8: k = XK_8;break;
    case Qt::Key_9: k = XK_9;break;
    }
    
    if (k == 5000)
    {
        
        if (!modifier)
        {
            switch (inkey)
            {
            case Qt::Key_A: k = XK_a;break;
            case Qt::Key_B: k = XK_b;break;
            case Qt::Key_C: k = XK_c;break;
            case Qt::Key_D: k = XK_d;break;
            case Qt::Key_E: k = XK_e;break;
            case Qt::Key_F: k = XK_f;break;
            case Qt::Key_G: k = XK_g;break;
            case Qt::Key_H: k = XK_h;break;
            case Qt::Key_I: k = XK_i;break;
            case Qt::Key_J: k = XK_j;break;
            case Qt::Key_K: k = XK_k;break;
            case Qt::Key_L: k = XK_l;break;
            case Qt::Key_M: k = XK_m;break;
            case Qt::Key_N: k = XK_n;break;
            case Qt::Key_O: k = XK_o;break;
            case Qt::Key_P: k = XK_p;break;
            case Qt::Key_Q: k = XK_q;break;
            case Qt::Key_R: k = XK_r;break;
            case Qt::Key_S: k = XK_s;break;
            case Qt::Key_T: k = XK_t;break;
            case Qt::Key_U: k = XK_u;break;
            case Qt::Key_V: k = XK_v;break;
            case Qt::Key_W: k = XK_w;break;
            case Qt::Key_X: k = XK_x;break;
            case Qt::Key_Y: k = XK_y;break;
            case Qt::Key_Z: k = XK_z;break;
            }
        }
        else
        {
            switch (inkey)
            {
            case Qt::Key_A: k = XK_A;break;
            case Qt::Key_B: k = XK_B;break;
            case Qt::Key_C: k = XK_C;break;
            case Qt::Key_D: k = XK_D;break;
            case Qt::Key_E: k = XK_E;break;
            case Qt::Key_F: k = XK_F;break;
            case Qt::Key_G: k = XK_G;break;
            case Qt::Key_H: k = XK_H;break;
            case Qt::Key_I: k = XK_I;break;
            case Qt::Key_J: k = XK_J;break;
            case Qt::Key_K: k = XK_K;break;
            case Qt::Key_L: k = XK_L;break;
            case Qt::Key_M: k = XK_M;break;
            case Qt::Key_N: k = XK_N;break;
            case Qt::Key_O: k = XK_O;break;
            case Qt::Key_P: k = XK_P;break;
            case Qt::Key_Q: k = XK_Q;break;
            case Qt::Key_R: k = XK_R;break;
            case Qt::Key_S: k = XK_S;break;
            case Qt::Key_T: k = XK_T;break;
            case Qt::Key_U: k = XK_U;break;
            case Qt::Key_V: k = XK_V;break;
            case Qt::Key_W: k = XK_W;break;
            case Qt::Key_X: k = XK_X;break;
            case Qt::Key_Y: k = XK_Y;break;
            case Qt::Key_Z: k = XK_Z;break;
            }
        }
    }
    
    return k;
    
}

void CConnectRabbitVNC::slotClipBoardChanged()
{
    if(!m_pPara->GetClipboard() || !getOutStream()) return;
    QClipboard* pClip = QApplication::clipboard();
    if(pClip->ownsClipboard()) return;
    qDebug(RabbitVNC) << "CConnectRabbitVnc::slotClipBoardChanged()";
    announceClipboard(true);
}

void CConnectRabbitVNC::handleClipboardRequest()
{
    if(!m_pPara->GetClipboard() || !getOutStream()) return;
    
    qDebug(RabbitVNC) << "CConnectRabbitVnc::handleClipboardRequest";
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    
    if (mimeData->hasImage()) {
        //        setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
    } else if (mimeData->hasText()) {
        QString szText = mimeData->text();
        qDebug(RabbitVNC) << "CConnectRabbitVnc::handleClipboardRequest:szText:" << szText;
        try{
            sendClipboardData(rfb::clipboardUTF8, szText.toStdString().c_str(),
                              szText.toStdString().size());
        } catch (rdr::Exception& e) {
            qCritical(RabbitVNC) << "sendClipboardData fail:" << e.str();
        }
    } else if (mimeData->hasHtml()) {
        QString szHtml = mimeData->html();
        qDebug(RabbitVNC) << "CConnectRabbitVnc::handleClipboardRequest:html:" << szHtml;
        try{
            sendClipboardData(rfb::clipboardHTML, mimeData->html().toStdString().c_str(),
                              mimeData->html().toStdString().size());
        } catch (rdr::Exception& e) {
            qCritical(RabbitVNC) << "sendClipboardData fail:" << e.str();
        }
    } else {
        qDebug(RabbitVNC) << "Cannot display data";
    }
}

void CConnectRabbitVNC::handleClipboardAnnounce(bool available)
{
    qDebug(RabbitVNC) << "CConnectRabbitVnc::handleClipboardAnnounce";
    if(!m_pPara->GetClipboard() || !getOutStream()) return;
    
    if(available)
        this->requestClipboard();
}

void CConnectRabbitVNC::handleClipboardData(unsigned int format, const char *data, size_t length)
{
    qDebug(RabbitVNC) << "CConnectRabbitVnc::handleClipboardData";
    if(!m_pPara->GetClipboard()) return;
    
    if(rfb::clipboardUTF8 & format) {
        QMimeData* pData = new QMimeData();
        pData->setText(QString::fromUtf8(data));
        emit sigSetClipboard(pData);
    } else if(rfb::clipboardHTML & format) {
        QMimeData* pData = new QMimeData();
        pData->setHtml(data);
        emit sigSetClipboard(pData);
        //pClip->setMimeData(pData);
    } else {
        qDebug(RabbitVNC) << "Don't implement";
    }
}
