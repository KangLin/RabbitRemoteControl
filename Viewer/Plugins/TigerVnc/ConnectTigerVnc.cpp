//! @author: Kang Lin(kl222@126.com)

#include "ConnectTigerVnc.h"
#include "ConnecterTigerVnc.h"
#include "DlgPassword.h"

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

#include "rfb/Security.h"
#ifdef HAVE_GNUTLS
#include "rfb/CSecurityTLS.h"
#endif

static rfb::LogWriter vlog("ConnectTigerVnc");

// 8 colours (1 bit per component)
static const rfb::PixelFormat verylowColourPF(8, 3,false, true,
                                         1, 1, 1, 2, 1, 0);
// 64 colours (2 bits per component)
static const rfb::PixelFormat lowColourPF(8, 6, false, true,
                                     3, 3, 3, 4, 2, 0);
// 256 colours (2-3 bits per component)
static const rfb::PixelFormat mediumColourPF(8, 8, false, true,
                                        7, 7, 3, 5, 2, 0);

// RGB (4 bits per component)
static const rfb::PixelFormat fullColourPF(32, 24, false, true,
                                           255, 255, 255, 16, 8, 0);

CConnectTigerVnc::CConnectTigerVnc(CConnecterTigerVnc *pConnecter, QObject *parent)
    : CConnect(pConnecter, parent),
      m_pSock(nullptr),
      m_pPara(nullptr),
      m_bWriteClipboard(false)
{
    security.setUserPasswdGetter(this);
    
#ifdef HAVE_GNUTLS
    rfb::CSecurityTLS::msg = this;
#endif
    
    SetParamter(&pConnecter->m_Para);
    if(!m_pPara->bLocalCursor)
    {
        emit sigUpdateCursor(QCursor(Qt::BlankCursor));
    }
}

CConnectTigerVnc::~CConnectTigerVnc()
{
    qDebug() << "CConnectTigerVnc::~CConnectTigerVnc()";
}

int CConnectTigerVnc::SetServerName(const QString &serverName)
{
    m_szServerName = serverName;
    CConnect::SetServerName(m_szServerName);
    
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

int CConnectTigerVnc::SetParamter(void *pPara)
{
    if(!pPara) return -1;
    
    m_pPara = (struct strPara*)pPara;

    SetServerName(m_pPara->szServerName);
    SetUser(m_pPara->szUser, m_pPara->szPassword);

    setShared(m_pPara->bShared);
    supportsLocalCursor = m_pPara->bLocalCursor;
    
    // Set Preferred Encoding
    setPreferredEncoding(m_pPara->nEncoding);
    setCompressLevel(m_pPara->nCompressLevel);
    setQualityLevel(m_pPara->nQualityLevel);

    // Set server pixmap format
    updatePixelFormat();
    
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

        setStreams(&m_pSock->inStream(), &m_pSock->outStream());
      
        initialiseProtocol();
    } catch (rdr::Exception& e) {
        vlog.error("%s", e.str());
        emit sigError(-1, e.str());
        return -1;
    }
    return 0;
}

int CConnectTigerVnc::Disconnect()
{
    if(m_pSock)
    {
        m_pSock->shutdown();
    }

    return 0;
}

int CConnectTigerVnc::Process()
{
    int nRet = 0;
    
    try {
        auto in = getInStream();
        if(in)
            in->check(1);
        processMsg();
    } catch (rdr::EndOfStream& e) {
        vlog.error("exec error: %s", e.str());
        emit sigError(-1, e.str());
        nRet = -1;
    } catch (rdr::Exception& e) {
        vlog.error("exec error: %s", e.str());
        nRet = -2;
        emit sigError(-1, e.str());
    }
    
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
    Q_ASSERT(m_pPara); // Please call SetParamter before call Connect
    vlog.info("initDone");
    
    // If using AutoSelect with old servers, start in FullColor
    // mode. See comment in autoSelectFormatAndEncoding. 
    if (server.beforeVersion(3, 8) && m_pPara->bAutoSelect)
        m_pPara->nColorLevel = Full;
    
    emit sigSetDesktopSize(server.width(), server.height());
    QString szName = QString::fromUtf8(server.name());
    setServerName(m_szServerName.toStdString().c_str());
    SetServerName(szName);
    
    //Set viewer frame buffer
    setFramebuffer(new CFramePixelBuffer(server.width(), server.height()));

    emit sigConnected();
}

void CConnectTigerVnc::bell()
{
    qApp->beep();
}

void CConnectTigerVnc::setCursor(int width, int height, const rfb::Point &hotspot, const rdr::U8 *data)
{
    //vlog.debug("CConnectTigerVnc::setCursor:%d,%d", hotspot.x, hotspot.y);
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

void CConnectTigerVnc::getUserPasswd(bool secure, char **user, char **password)
{
    if(user)
        *user = rfb::strDup(m_szUser.toStdString().c_str());
    if(password)
        *password = rfb::strDup(m_szPassword.toStdString().c_str());
}

bool CConnectTigerVnc::showMsgBox(int flags, const char *title, const char *text)
{
    vlog.error("%s:%s\n", title, text);
    return true;
}

// framebufferUpdateEnd() is called at the end of an update.
// For each rectangle, the FdInStream will have timed the speed
// of the connection, allowing us to select format and encoding
// appropriately, and then request another incremental update.
void CConnectTigerVnc::framebufferUpdateEnd()
{
    rfb::CConnection::framebufferUpdateEnd();
    //vlog.debug("CConnectTigerVnc::framebufferUpdateEnd");
    if(m_pPara && m_pPara->bBufferEndRefresh)
    {
        const QImage& img = dynamic_cast<CFramePixelBuffer*>(getFramebuffer())->getImage();
        emit sigUpdateRect(img.rect(), img);
    }
    
    // Compute new settings based on updated bandwidth values
    if (m_pPara && m_pPara->bAutoSelect)
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
void CConnectTigerVnc::autoSelectFormatAndEncoding()
{
    int kbitsPerSecond = m_pSock->inStream().kbitsPerSecond();
    unsigned int timeWaited = m_pSock->inStream().timeWaited();
    bool newFullColour = m_pPara->nColorLevel == Full ? true : false;
    int newQualityLevel = m_pPara->nQualityLevel;
    
    // Always use Tight
    setPreferredEncoding(rfb::encodingTight);
    
    // Check that we have a decent bandwidth measurement
    if ((kbitsPerSecond == 0) || (timeWaited < 10000))
        return;
    
    // Select appropriate quality level
    if (!m_pPara->bNoJpeg) {
        if (kbitsPerSecond > 16000)
            newQualityLevel = 8;
        else
            newQualityLevel = 6;
        
        if (newQualityLevel != m_pPara->nQualityLevel) {
            vlog.info(("Throughput %d kbit/s - changing to quality %d"),
                      kbitsPerSecond, newQualityLevel);
            m_pPara->nQualityLevel = newQualityLevel;
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
    newFullColour = (kbitsPerSecond > 256);
    if (newFullColour != (0 == m_pPara->nColorLevel)) {
        if (newFullColour)
            vlog.info(("Throughput %d kbit/s - full color is now enabled"),
                      kbitsPerSecond);
        else
            vlog.info(("Throughput %d kbit/s - full color is now disabled"),
                      kbitsPerSecond);
        m_pPara->nColorLevel = newFullColour ? CConnectTigerVnc::Full : CConnectTigerVnc::Low;
        updatePixelFormat();
    } 
}

// requestNewUpdate() requests an update from the server, having set the
// format and encoding appropriately.
void CConnectTigerVnc::updatePixelFormat()
{
    Q_ASSERT(m_pPara);
    
    if(!m_pPara) return;
    rfb::PixelFormat pf;
    
    switch (m_pPara->nColorLevel) {
    case CConnectTigerVnc::Full:
        pf = fullColourPF;
        break;
    case CConnectTigerVnc::Medium:
        pf = mediumColourPF;
        break;
    case CConnectTigerVnc::Low:
        pf = lowColourPF;
        break;
    case CConnectTigerVnc::VeryLow:
        pf = verylowColourPF;
        break;
    }
  
    char str[256];
    pf.print(str, 256);
    vlog.info(tr("Using pixel format %s").toStdString().c_str(), str);
    setPF(pf);
}

void CConnectTigerVnc::dataRect(const rfb::Rect &r, int encoding)
{
    m_pSock->inStream().startTiming();
    rfb::CConnection::dataRect(r, encoding);
    m_pSock->inStream().stopTiming();
    
    //vlog.debug("CConnectTigerVnc::dataRect:%d, %d, %d, %d; %d",
    //           r.tl.x, r.tl.y, r.width(), r.height(), encoding);
    // 立即更新图像
    if(m_pPara && !m_pPara->bBufferEndRefresh)
    {
        const QImage& img = dynamic_cast<CFramePixelBuffer*>(getFramebuffer())->getImage();
        emit sigUpdateRect(img.rect(), img);
    }
}

void CConnectTigerVnc::slotMousePressEvent(QMouseEvent* e)
{
    //vlog.debug("CConnectTigerVnc::slotMousePressEvent");
    if(!writer()) return;
    unsigned char mask = 0;
    rfb::Point pos(e->x(), e->y());
    if(e->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(e->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(e->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;

    writer()->writePointerEvent(pos, mask);
}

void CConnectTigerVnc::slotMouseReleaseEvent(QMouseEvent* e)
{
    if(!writer()) return;
    //vlog.debug("CConnectTigerVnc::slotMouseReleaseEvent");
    int mask = 0;
    rfb::Point pos(e->x(), e->y());
    
    writer()->writePointerEvent(pos, mask);
}

void CConnectTigerVnc::slotMouseMoveEvent(QMouseEvent* e)
{
    //vlog.debug("CConnectTigerVnc::slotMouseMoveEvent");
    //qDebug() << "slotMouseMoveEvent x:" << e->x() << ";y:" << e->y();
    if(!writer()) return;
    int mask = 0;
    rfb::Point pos(e->x(), e->y());
    if(e->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(e->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(e->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    writer()->writePointerEvent(pos, mask);
}

void CConnectTigerVnc::slotWheelEvent(QWheelEvent* e)
{
    //vlog.debug("CConnectTigerVnc::slotWheelEvent");
    if(!writer()) return;
    int mask = 0;
    rfb::Point pos(e->x(), e->y());
    
    if(e->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(e->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(e->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    
    QPoint p = e->angleDelta();
    if(p.y() > 0)
        mask |= 8;
    if(p.y() < 0)
        mask |= 16;
    if(p.x() < 0)
        mask |= 32;
    if(p.x() > 0)
        mask |= 64;
    
    writer()->writePointerEvent(pos, mask);
}

void CConnectTigerVnc::slotKeyPressEvent(QKeyEvent* e)
{
    if(!writer()) return;
    bool modifier = true;
    if (e->modifiers() == Qt::NoModifier)
        modifier = false;
    //vlog.debug("key:%d", e->key());
    writer()->writeKeyEvent(TranslateRfbKey(e->key(), modifier),
                                  0, true);
}

void CConnectTigerVnc::slotKeyReleaseEvent(QKeyEvent* e)
{
    if(!writer()) return;
    bool modifier = true;
    if (e->modifiers() == Qt::NoModifier)
        modifier = false;

    writer()->writeKeyEvent(TranslateRfbKey(e->key(), modifier), 0, false);
}

/**
 * @brief CConnectTigerVnc::TranslateRfbKey
 * @param inkey
 * @param modifier
 * @return 
 * @see https://github.com/rfbproto/rfbproto/blob/master/rfbproto.rst#keyevent
 */
quint32 CConnectTigerVnc::TranslateRfbKey(quint32 inkey, bool modifier)
{
    quint32 k = 5000;

    switch (inkey)
    {
        case Qt::Key_Backspace: k = XK_BackSpace; break;
        case Qt::Key_Tab: k = XK_Tab;break;
        case Qt::Key_Clear: k = XK_Clear; break;
        case Qt::Key_Return: k = XK_Return; break;
        case Qt::Key_Pause: k = XK_Pause; break;
        case Qt::Key_Escape: k = XK_Escape; break;
        case Qt::Key_Space: k = XK_space; break;
        case Qt::Key_Delete: k = XK_Delete; break;
        case Qt::Key_Period: k = XK_period; break;

        //special keyboard char
        case Qt::Key_Exclam: k = XK_exclam;break; //!
        case Qt::Key_QuoteDbl: k = XK_quotedbl;break; //?
        case Qt::Key_NumberSign: k = XK_numbersign;break; //#
        case Qt::Key_Percent: k = XK_percent;break; //%
        case Qt::Key_Dollar: k = XK_dollar;break;   //$
        case Qt::Key_Ampersand: k = XK_ampersand;break; //&
        case Qt::Key_Apostrophe: k = XK_apostrophe;break;//!
        case Qt::Key_ParenLeft: k = XK_parenleft;break;
        case Qt::Key_ParenRight: k = XK_parenright;break;

        case Qt::Key_Slash: k = XK_slash; break;    ///
        case Qt::Key_Asterisk: k = XK_asterisk; break;  //*
        case Qt::Key_Minus: k = XK_minus; break;    //-
        case Qt::Key_Plus: k = XK_plus; break;  //+
        case Qt::Key_Enter: k = XK_Return; break;   //
        case Qt::Key_Equal: k = XK_equal; break;    //=
        case Qt::Key_Comma: return XK_comma; //,
        
        case Qt::Key_Colon: k = XK_colon;break;
        case Qt::Key_Semicolon: k = XK_semicolon; break;
        case Qt::Key_Greater: k = XK_greater; break;
        case Qt::Key_Question: k = XK_question; break;
        case Qt::Key_At: k = XK_at; break;

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
        case  Qt::Key_registered: k = XK_registered; break;

        case Qt::Key_Up: k = XK_Up; break;
        case Qt::Key_Down: k = XK_Down; break;
        case Qt::Key_Right: k = XK_Right; break;
        case Qt::Key_Left: k = XK_Left; break;
        case Qt::Key_Insert: k = XK_Insert; break;
        case Qt::Key_Home: k = XK_Home; break;
        case Qt::Key_End: k = XK_End; break;
        case Qt::Key_PageUp: k = XK_Page_Up; break;
        case Qt::Key_PageDown: k = XK_Page_Down; break;
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

        case Qt::Key_Mode_switch: k = XK_Mode_switch; break;
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

void CConnectTigerVnc::slotClipBoardChange()
{
    if(!m_pPara->bClipboard) return;
    QClipboard* pClip = QApplication::clipboard();
    if(pClip->ownsClipboard()) return;
    
//    vlog.debug("CConnectTigerVnc::slotClipBoardChange()");

    announceClipboard(true);
}

void CConnectTigerVnc::handleClipboardRequest()
{
    if(!m_pPara->bClipboard) return;
    
//    vlog.debug("CConnectTigerVnc::handleClipboardRequest");
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    
    if (mimeData->hasImage()) {
//        setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
    } else if (mimeData->hasText()) {
        QString szText = mimeData->text();
//        vlog.debug("CConnectTigerVnc::handleClipboardRequest:szText:%s",
//                   szText.toStdString().c_str());
        try{
            sendClipboardData(rfb::clipboardUTF8, szText.toStdString().c_str(),
                              szText.toStdString().size());
        } catch (rdr::Exception& e) {
            vlog.error("%s", e.str());
        }
    } else if (mimeData->hasHtml()) {
        QString szHtml = mimeData->html();
        //        vlog.debug("CConnectTigerVnc::handleClipboardRequest:html:%s",
        //                   szHtml.toStdString().c_str());
        try{
            sendClipboardData(rfb::clipboardHTML, mimeData->html().toStdString().c_str(),
                              mimeData->html().toStdString().size());
        } catch (rdr::Exception& e) {
            vlog.error("%s", e.str());
        }
    } else {
        vlog.debug("Cannot display data");
    }
}

void CConnectTigerVnc::handleClipboardAnnounce(bool available)
{
    //    vlog.debug("CConnectTigerVnc::handleClipboardAnnounce");
    if(!m_pPara->bClipboard) return;
    
    if(available)
        this->requestClipboard();
}

void CConnectTigerVnc::handleClipboardData(unsigned int format, const char *data, size_t length)
{
//    vlog.debug("CConnectTigerVnc::handleClipboardData");
    if(!m_pPara->bClipboard) return;
    
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
        vlog.debug("Don't implement");
    }
}
