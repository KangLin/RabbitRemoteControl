#include "ConnectLibVNCServer.h"
#include "ConnecterLibVNCServer.h"

#include <QDebug>
#include <QApplication>
#include <QImage>
#include <QClipboard>
#include <QNetworkProxy>
#include <QInputDialog>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(LibVNCServer)

const char* gThis = "This pointer";
#define LOG_BUFFER_LENGTH 1024
static void rfbQtClientLog(const char *format, ...)
{
    int nRet = 0;
    va_list args;
    char buf[LOG_BUFFER_LENGTH];
     
    if(!rfbEnableClientLogging)
        return;
    
    va_start(args, format);    
    nRet = vsnprintf(buf, LOG_BUFFER_LENGTH, format, args);
    va_end(args);
    if(nRet < 0 || nRet >= LOG_BUFFER_LENGTH)
    {
        qCritical(LibVNCServer) << "vsprintf buf is short,"
                                << nRet << ">" <<  LOG_BUFFER_LENGTH
                                << ". Truncated it:" << nRet - LOG_BUFFER_LENGTH;
        buf[LOG_BUFFER_LENGTH - 1] = 0;
    }
    qDebug(LibVNCServer) << buf;
}

CConnectLibVNCServer::CConnectLibVNCServer(CConnecterLibVNCServer *pConnecter, QObject *parent)
    : CConnect(pConnecter, parent),
      m_pClient(nullptr),
      m_pPara(dynamic_cast<CParameterLibVNCServer*>(pConnecter->GetParameter()))
{
#ifdef _DEBUG
    rfbClientLog = rfbQtClientLog;
#endif
    if(!m_pPara->GetLocalCursor())
    {
        emit sigUpdateCursor(QCursor(Qt::BlankCursor));
    }
}

CConnectLibVNCServer::~CConnectLibVNCServer()
{
    qDebug(LibVNCServer) << "CConnectLibVNCServer::~CConnectLibVNCServer()";
}

bool CConnectLibVNCServer::InitClient()
{
    if(m_pClient) Q_ASSERT(!m_pClient);
    
    m_pClient = rfbGetClient(8,3,4);
    if(!m_pClient)
    {
        qCritical(LibVNCServer) << "rfbGetClient fail";
        return false;
    }
    
    // Set parameters
    m_pClient->programName = strdup(qApp->applicationName().toStdString().c_str());
    // Set server ip and port
    m_pClient->serverHost = strdup(m_pPara->GetHost().toStdString().c_str());
    m_pClient->serverPort = m_pPara->GetPort();
    
    m_pClient->appData.shareDesktop = m_pPara->GetShared();
    m_pClient->appData.viewOnly = m_pPara->GetOnlyView();
    m_pClient->appData.useRemoteCursor = m_pPara->GetLocalCursor();
    
    //Qt is support QImage::Format_RGB32, so we use default format QImage::Format_RGB32 in OnSize()
//    m_pClient->appData.requestedDepth = m_pPara->nColorLevel;
//    m_pClient->format.depth = m_pPara->nColorLevel;
//    switch (m_pClient->format.depth) {
//	case 8:
//		m_pClient->format.depth = 8;
//		m_pClient->format.bitsPerPixel = 8;
//		m_pClient->format.blueMax = 3;
//		m_pClient->format.blueShift = 6;
//		m_pClient->format.greenMax = 7;
//		m_pClient->format.greenShift = 3;
//		m_pClient->format.redMax = 7;
//		m_pClient->format.redShift = 0;
//		break;
//	case 16:
//		m_pClient->format.depth = 15;
//		m_pClient->format.bitsPerPixel = 16;
//		m_pClient->format.redShift = 11;
//		m_pClient->format.greenShift = 6;
//		m_pClient->format.blueShift = 1;
//		m_pClient->format.redMax = 31;
//		m_pClient->format.greenMax = 31;
//		m_pClient->format.blueMax = 31;
//		break;
//	case 32:
//	default:
//		m_pClient->format.depth = 24;
//		m_pClient->format.bitsPerPixel = 32;
//		m_pClient->format.blueShift = 0;
//		m_pClient->format.redShift = 16;
//		m_pClient->format.greenShift = 8;
//		m_pClient->format.blueMax = 0xff;
//		m_pClient->format.redMax = 0xff;
//		m_pClient->format.greenMax = 0xff;
//		break;
//	}
    
    m_pClient->appData.enableJPEG = m_pPara->GetJpeg();
    if(m_pClient->appData.enableJPEG)
        m_pClient->appData.qualityLevel = m_pPara->GetQualityLevel();
    if(m_pPara->GetEnableCompressLevel())
        m_pClient->appData.compressLevel = m_pPara->GetCompressLevel();
    
    // Set callback function
    m_pClient->MallocFrameBuffer = cb_resize;
    m_pClient->GotFrameBufferUpdate = cb_update;
    m_pClient->HandleKeyboardLedState = cb_kbd_leds;
    m_pClient->Bell = cb_bell;
    m_pClient->HandleTextChat = cb_text_chat;
    m_pClient->GotXCutText = cb_got_selection;
    m_pClient->GetCredential = cb_get_credential;
    m_pClient->GetPassword = cb_get_password;
    m_pClient->HandleCursorPos = cb_cursor_pos;
    m_pClient->GotCursorShape = cb_got_cursor_shape;
    
    m_pClient->canHandleNewFBSize = TRUE;
    rfbClientSetClientData(m_pClient, (void*)gThis, this);
    
    // Set sock
    switch(m_pPara->GetProxyType())
    {
    case CParameterConnecter::emProxy::SocksV4:
        break;
    case CParameterConnecter::emProxy::SocksV5:
    {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName(m_pPara->GetProxyHost());
        proxy.setPort(m_pPara->GetProxyPort());
        proxy.setUser(m_pPara->GetProxyUser());
        proxy.setPassword(m_pPara->GetProxyPassword());
        m_tcpSocket.setProxy(proxy);
        m_tcpSocket.connectToHost(m_pClient->serverHost, m_pClient->serverPort);
        if (!m_tcpSocket.waitForConnected(3000))
            return FALSE;
        //TODO: The is fail
        m_pClient->sock = m_tcpSocket.socketDescriptor();
        break;
    }
    case (CParameterConnecter::emProxy) CParameterLibVNCServer::emVncProxy::UltraVncRepeater:
        m_pClient->destHost = strdup(m_pPara->GetProxyHost().toStdString().c_str());
        m_pClient->destPort = m_pPara->GetProxyPort();
    case CParameterConnecter::emProxy::No:
        if(!rfbInitClient(m_pClient, nullptr, nullptr))
        {
            qCritical(LibVNCServer) <<  "rfbInitClient fail";
            return FALSE;
        }
        break;
    default:
        break;
    }

    return TRUE;
}

/*
 * \return 
 * \li < 0: error
 * \li = 0: Use OnProcess (non-Qt event loop)
 * \li > 0: Don't use OnProcess (qt event loop)
 */
int CConnectLibVNCServer::OnInit()
{
    if(!InitClient()) {
        qCritical(LibVNCServer) << "rfbInitClient fail";
        emit sigError(-1, "Connect fail");
        return -2;
    }

    emit sigConnected();
    emit sigServerName(m_pClient->desktopName);
    emit sigSetDesktopSize(m_pClient->width, m_pClient->height);
    
    QString szInfo = QString("Connect to ") + m_pClient->desktopName;
    qInfo(LibVNCServer) << szInfo;
    emit sigInformation(szInfo);
 
    return 0;
}

int CConnectLibVNCServer::OnClean()
{
    if(m_pClient)
    {
        m_tcpSocket.close();
        m_pClient->sock = -1; //RFB_INVALID_SOCKET;

        rfbClientCleanup(m_pClient);
        m_pClient = nullptr;
    }
    return 0;
}

/*!
 * \~chinese 插件连接的具体操作处理。因为此插件是非Qt事件，所以在此函数中等待。
 * \~english Specific operation processing of plug-in connection.
 *           Because of it is a non-Qt event loop, so wait in here.
 * \~
 * \return 
 *       \li >= 0: continue, Interval call time (msec)
 *       \li < 0: error or stop
 * \see slotTimeOut()
 */
int CConnectLibVNCServer::OnProcess()
{
    int nRet = 0;
    //LOG_MODEL_DEBUG("CConnectLibVNCServer", "CConnectLibVNCServer::Process()");
    nRet = WaitForMessage(m_pClient, 500);
    if (nRet < 0)
        return nRet;
    
    if(nRet > 0)
        if(!HandleRFBServerMessage(m_pClient))
            return -1;
    
    return 0;
}

void CConnectLibVNCServer::slotClipBoardChanged()
{
    if(m_pPara && !m_pPara->GetClipboard()) return;
    QClipboard* pClipboard = QApplication::clipboard();
    if(pClipboard)
    {
        QString szText = pClipboard->text();
        if(!szText.isEmpty() && m_pClient)
            SendClientCutText(m_pClient,
                              (char*)szText.toStdString().c_str(),
                              szText.toStdString().length());
    }
}

rfbBool CConnectLibVNCServer::cb_resize(rfbClient* client)
{
    //LOG_MODEL_ERROR("LibVNCServer", "CConnectLibVnc::cb_resize");
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(client, (void*)gThis);
    if(pThis->OnSize()) return FALSE;
    return TRUE;
}

void CConnectLibVNCServer::cb_update(rfbClient *client, int x, int y, int w, int h)
{
    //LOG_MODEL_ERROR("LibVNCServer", "CConnectLibVnc::cb_update:(%d, %d, %d, %d)", x, y, w, h);
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(client, (void*)gThis);
    emit pThis->sigUpdateRect(QRect(x, y, w, h), pThis->m_Image);
}

void CConnectLibVNCServer::cb_got_selection(rfbClient *client, const char *text, int len)
{
    //LOG_MODEL_ERROR("LibVNCServer", "CConnectLibVnc::cb_got_selection:%s", text);
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(client, (void*)gThis);
    if(!pThis->m_pPara->GetClipboard()) return;
    QClipboard* pClipboard = QApplication::clipboard();
    if(pClipboard)
        pClipboard->setText(text);
}

void CConnectLibVNCServer::cb_kbd_leds(rfbClient *client, int value, int pad)
{
    qDebug(LibVNCServer) << "CConnectLibVnc::cb_kbd_leds";
}

void CConnectLibVNCServer::cb_bell(struct _rfbClient *client)
{
    qApp->beep();
}

void CConnectLibVNCServer::cb_text_chat(rfbClient *client, int value, char *text)
{
    qDebug(LibVNCServer) << "CConnectLibVnc::cb_text_chat";
}

rfbCredential* CConnectLibVNCServer::cb_get_credential(rfbClient *cl, int credentialType)
{
    qDebug(LibVNCServer) <<"CConnectLibVnc::cb_get_credential";
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(cl, (void*)gThis);
    rfbCredential *c = (rfbCredential*)malloc(sizeof(rfbCredential));
    c->userCredential.username = (char*)malloc(RFB_BUF_SIZE);
    memset(c->userCredential.username, 0, RFB_BUF_SIZE);
    c->userCredential.password = (char*)malloc(RFB_BUF_SIZE);
    memset(c->userCredential.password, 0, RFB_BUF_SIZE);

    if(credentialType != rfbCredentialTypeUser) {
        qCritical(LibVNCServer) << "something else than username and password required for authentication";
        return NULL;
    }

    qDebug(LibVNCServer) << "Username and password required for authentication!";

    memcpy(c->userCredential.username,
           pThis->m_pPara->GetUser().toStdString().c_str(),
           pThis->m_pPara->GetUser().toStdString().length());
    memcpy(c->userCredential.password,
           pThis->m_pPara->GetPassword().toStdString().c_str(),
           pThis->m_pPara->GetPassword().toStdString().length());

    return c;
}

char* CConnectLibVNCServer::cb_get_password(rfbClient *client)
{
    //LOG_MODEL_ERROR("LibVNCServer", "CConnectLibVnc::cb_get_password");
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(client, (void*)gThis);
    QString szPassword = pThis->m_pPara->GetPassword();
    if(szPassword.isEmpty())
    {
        int nRet = QDialog::Rejected;
        emit pThis->sigBlockShowWidget("CDlgLibVNCServerPassword", nRet, pThis->m_pPara);
        if(QDialog::Accepted == nRet)
        {
            szPassword = pThis->m_pPara->GetPassword();    
        }
        if(szPassword.isEmpty())
            return nullptr;
    }
    return strdup(szPassword.toStdString().c_str());
}

int CConnectLibVNCServer::OnSize()
{
    int nRet = 0;
    int nWidth = m_pClient->width;
    int nHeight = m_pClient->height;

    m_Image = QImage(nWidth, nHeight, QImage::Format_RGB32);
    
	m_pClient->frameBuffer = m_Image.bits();

    m_pClient->format.bitsPerPixel = 32;
	m_pClient->format.depth = m_Image.depth();
	m_pClient->format.redShift = 16;
	m_pClient->format.greenShift = 8;
	m_pClient->format.blueShift = 0;
    m_pClient->format.redMax = 255;
	m_pClient->format.greenMax = 255;
	m_pClient->format.blueMax = 255;
	SetFormatAndEncodings(m_pClient);
    return nRet;
}

rfbBool CConnectLibVNCServer::cb_cursor_pos(rfbClient *client, int x, int y)
{
    //LOG_MODEL_ERROR("LibVNCServer", "CConnectLibVnc::cb_cursor_pos:%d,%d", x, y);
    rfbBool bRet = true;
    
    return bRet;
}

void CConnectLibVNCServer::cb_got_cursor_shape(rfbClient *client,
                                         int xhot, int yhot,
                                         int width, int height,
                                         int bytesPerPixel)
{
    /*
    LOG_MODEL_ERROR("LibVNCServer", "CConnectLibVnc::cb_got_cursor_shape:x:%d, y:%d, width:%d, height:%d, bytesPerPixel:%d",
                    xhot, yhot, width, height, bytesPerPixel);//*/
    if(!client->rcSource)
    {
        qCritical(LibVNCServer) << "client->rcSource is null";
        return;
    }
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(client, (void*)gThis);
    if ((width == 0) || (height == 0)) {
        QImage cursor(1, 1, QImage::Format_ARGB32);
        uchar* buffer = cursor.bits();
        memset(buffer, 0, 4);
        emit pThis->sigUpdateCursor(QCursor(QPixmap::fromImage(cursor), xhot, yhot));
    } else {
        switch (bytesPerPixel) {
        case 4:
        {
            QImage cursor(width, height, QImage::Format_ARGB32);
            uchar *buffer = cursor.bits();
            memcpy(buffer, client->rcSource, width * height * 4);
            uchar* pMask = client->rcMask;
            if(pMask)
            {
                uchar* pDest = buffer;
                for(int y = 0; y < height; y++)
                {
                    for(int w = 0; w < width; w++)
                    {
                        if(*pMask++)
                        {
                            // NOTE: Do not manipulate the memory,
                            //       because the host byte order may be different
                            QColor c = cursor.pixel(w, y);
                            c.setAlpha(0xff);
                            cursor.setPixelColor(w,y,c);
                        }
                        else
                            *((uint32_t*)pDest) = 0;
                        
                        pDest += 4;
                    }
                }
            }
            emit pThis->sigUpdateCursor(QCursor(QPixmap::fromImage(cursor), xhot, yhot));
            break;
        }
        default:
            break;
        }
    }
}

void CConnectLibVNCServer::slotMousePressEvent(Qt::MouseButtons buttons, QPoint pos)
{
    if(!m_pClient) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    //qDebug(LibVNCServer) << "CConnectLibVnc::slotMousePressEvent" << e->button() << e->buttons();
    unsigned char mask = 0;
    if(buttons & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(buttons & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(buttons & Qt::MouseButton::RightButton)
        mask |= 0x4;

    SendPointerEvent(m_pClient, pos.x(), pos.y(), mask);
}

void CConnectLibVNCServer::slotMouseReleaseEvent(Qt::MouseButton button, QPoint pos)
{
    if(!m_pClient) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    int mask = 0;
    if(button & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(button & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(button & Qt::MouseButton::RightButton)
        mask |= 0x4;
    SendPointerEvent(m_pClient, pos.x(), pos.y(), mask);
}

void CConnectLibVNCServer::slotMouseMoveEvent(Qt::MouseButtons buttons, QPoint pos)
{
    //qDebug(LibVNCServer) << "CConnectLibVnc::slotMouseMoveEvent" << e->button() << e->buttons();
    if(!m_pClient) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    int mask = 0;
    if(buttons & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(buttons & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(buttons & Qt::MouseButton::RightButton)
        mask |= 0x4;
    SendPointerEvent(m_pClient, pos.x(), pos.y(), mask);
}

void CConnectLibVNCServer::slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta)
{
    //vlog.debug("CConnectLibVnc::slotWheelEvent");
    if(!m_pClient) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    int mask = 0;
    
    if(buttons & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(buttons & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(buttons & Qt::MouseButton::RightButton)
        mask |= 0x4;
    
    QPoint p = angleDelta;
    if(p.y() > 0)
        mask |= 8;
    if(p.y() < 0)
        mask |= 16;
    if(p.x() < 0)
        mask |= 32;
    if(p.x() > 0)
        mask |= 64;
    
    SendPointerEvent(m_pClient, pos.x(), pos.y(), mask);
}

/**
 * @brief CConnectTigerVnc::TranslateRfbKey
 * @param inkey
 * @param shiftModifier
 * @return 
 * @see https://github.com/rfbproto/rfbproto/blob/master/rfbproto.rst#keyevent
 */
uint32_t TranslateRfbKey(quint32 inkey, bool modifier)
{
    quint32 k = 0;
    
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

void CConnectLibVNCServer::slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers)
{
    if(!m_pClient) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    bool shiftModifier = false;
    if (modifiers & Qt::ShiftModifier)
        shiftModifier = true;
    uint32_t k = TranslateRfbKey(key, shiftModifier);
    if(key)
        SendKeyEvent(m_pClient, k, TRUE);
}

void CConnectLibVNCServer::slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers)
{
    if(!m_pClient) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    bool shiftModifier = false;
    if (modifiers & Qt::ShiftModifier)
        shiftModifier = true;
    uint32_t k = TranslateRfbKey(key, shiftModifier);
    if(key)
        SendKeyEvent(m_pClient, k, FALSE);
}
