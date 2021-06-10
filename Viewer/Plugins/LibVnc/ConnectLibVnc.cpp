#include "ConnectLibVnc.h"
#include "ConnecterLibVnc.h"
#include "RabbitCommonLog.h"
#include <QDebug>
#include <QApplication>
#include <QImage>
#include <QClipboard>
#include <QNetworkProxy>

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
        LOG_MODEL_ERROR("Global",
                        "vsprintf buf is short, %d > %d. Truncated it:%d",
                        nRet, LOG_BUFFER_LENGTH, nRet - LOG_BUFFER_LENGTH);
        buf[LOG_BUFFER_LENGTH - 1] = 0;
    }
    LOG_MODEL_DEBUG("LibVncServer", buf);
}

CConnectLibVnc::CConnectLibVnc(CConnecterLibVnc *pConnecter, QObject *parent)
    : CConnect(pConnecter, parent),
      m_pClient(nullptr),
      m_pPara(&pConnecter->m_Para)
{
#ifdef _DEBUG
    rfbClientLog = rfbQtClientLog;
#endif
    if(!m_pPara->bLocalCursor)
    {
        emit sigUpdateCursor(QCursor(Qt::BlankCursor));
    }
}

CConnectLibVnc::~CConnectLibVnc()
{
    qDebug() << "CConnectLibVnc::~CConnectLibVnc()";
}

bool CConnectLibVnc::InitClient()
{
    // Set sock
    switch(m_pPara->eProxyType)
    {
    case CParameter::emProxy::SocksV4:
        break;
    case CParameter::emProxy::SocksV5:
    {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName(m_pPara->szProxyHost);
        proxy.setPort(m_pPara->nProxyPort);
        proxy.setUser(m_pPara->szProxyUser);
        proxy.setPassword(m_pPara->szProxyPassword);
        m_tcpSocket.setProxy(proxy);
        m_tcpSocket.connectToHost(m_pClient->serverHost, m_pClient->serverPort);
        if (!m_tcpSocket.waitForConnected(3000))
            return FALSE;
        //TODO: The is fail
        m_pClient->sock = m_tcpSocket.socketDescriptor();
        break;
    }
    case (CParameter::emProxy) CConnectLibVnc::strPara::emVncProxy::UltraVncRepeater:
        m_pClient->destHost = strdup(m_pPara->szProxyHost.toStdString().c_str());
        m_pClient->destPort = m_pPara->nProxyPort;
    case CParameter::emProxy::No:
        if(!rfbInitClient(m_pClient, nullptr, nullptr))
            return FALSE;
        break;
    default:
        break;
    }

    if (!InitialiseRFBConnection(m_pClient))
      return FALSE;

    m_pClient->width=m_pClient->si.framebufferWidth;
    m_pClient->height=m_pClient->si.framebufferHeight;
    if (!m_pClient->MallocFrameBuffer(m_pClient))
      return FALSE;

    if (!SetFormatAndEncodings(m_pClient))
      return FALSE;

    if (m_pClient->updateRect.x < 0) {
      m_pClient->updateRect.x = m_pClient->updateRect.y = 0;
      m_pClient->updateRect.w = m_pClient->width;
      m_pClient->updateRect.h = m_pClient->height;
    }

    if (m_pClient->appData.scaleSetting>1)
    {
        if (!SendScaleSetting(m_pClient, m_pClient->appData.scaleSetting))
            return FALSE;
        if (!SendFramebufferUpdateRequest(m_pClient,
                    m_pClient->updateRect.x / m_pClient->appData.scaleSetting,
                    m_pClient->updateRect.y / m_pClient->appData.scaleSetting,
                    m_pClient->updateRect.w / m_pClient->appData.scaleSetting,
                    m_pClient->updateRect.h / m_pClient->appData.scaleSetting,
                    FALSE))
            return FALSE;
    }
    else
    {
        if (!SendFramebufferUpdateRequest(m_pClient,
                    m_pClient->updateRect.x, m_pClient->updateRect.y,
                    m_pClient->updateRect.w, m_pClient->updateRect.h,
                    FALSE))
        return FALSE;
    }
    return TRUE;
}

int CConnectLibVnc::Clean()
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

int CConnectLibVnc::Initialize()
{
    int nRet = 0;
    
    if(m_pClient) Q_ASSERT(!m_pClient);
    
    m_pClient = rfbGetClient(8,3,4);
    if(!m_pClient)
    {
        LOG_MODEL_ERROR("LibVnc", "rfbGetClient fail");
        return -1;
    }
    
    SetParamter(m_pPara);
    
    m_pClient->MallocFrameBuffer = cb_resize;
    m_pClient->canHandleNewFBSize = true;
    m_pClient->GotFrameBufferUpdate = cb_update;
    m_pClient->HandleKeyboardLedState = cb_kbd_leds;
    m_pClient->Bell = cb_bell;
    m_pClient->HandleTextChat = cb_text_chat;
    m_pClient->GotXCutText = cb_got_selection;
    m_pClient->GetCredential = cb_get_credential;
    m_pClient->GetPassword = cb_get_password;
    m_pClient->HandleCursorPos = cb_cursor_pos;
    m_pClient->GotCursorShape = cb_got_cursor_shape;
    rfbClientSetClientData(m_pClient, (void*)gThis, this);

    return nRet;
}

/**
  nRet < 0 : error
  nRet = 0 : emit sigConnected
  nRet = 1 : emit sigConnected in CConnect
  */
int CConnectLibVnc::Connect()
{
    int nRet = 1;

    if(!InitClient()) {
        LOG_MODEL_ERROR("LibVnc", "rfbInitClient fail");
        emit sigError(-1, "Connect fail");
        return -2;
    }
    
    QString szInfo = QString("Connect to ") + m_pClient->desktopName;
    LOG_MODEL_INFO("LibVnc", szInfo.toStdString().c_str());
    
    emit sigConnected();
    emit sigServerName(m_pClient->desktopName);

    emit sigSetDesktopSize(m_pClient->width, m_pClient->height);
    
    return nRet;
}

int CConnectLibVnc::Disconnect()
{
    int nRet = 0;
    
    return nRet;
}

int CConnectLibVnc::Process()
{
    int nRet = 0;

    nRet = WaitForMessage(m_pClient, 500);
    if (nRet < 0)
        return nRet;
    
    if(nRet)
    {
        if(!HandleRFBServerMessage(m_pClient))
            return -1;
    }
    
    return 0;
}

void CConnectLibVnc::slotClipBoardChange()
{
    if(m_pPara && !m_pPara->bClipboard) return;
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

int CConnectLibVnc::SetParamter(void*)
{
    int nRet = 0;
    Q_ASSERT(m_pClient);

    // Set server ip and port
    m_pClient->serverHost = strdup(m_pPara->szHost.toStdString().c_str());
    m_pClient->serverPort = m_pPara->nPort;
    
    m_pClient->appData.shareDesktop = m_pPara->bShared;
    m_pClient->appData.viewOnly = m_pPara->bOnlyView;
    m_pClient->appData.useRemoteCursor = m_pPara->bLocalCursor;
    
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
    
    m_pClient->appData.enableJPEG = m_pPara->bJpeg;
    if(m_pClient->appData.enableJPEG)
        m_pClient->appData.qualityLevel = m_pPara->nQualityLevel;
    if(m_pPara->bCompressLevel)
        m_pClient->appData.compressLevel = m_pPara->nCompressLevel;
    
    return nRet;
}

rfbBool CConnectLibVnc::cb_resize(rfbClient* client)
{
    //LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_resize");
    CConnectLibVnc* pThis = (CConnectLibVnc*)rfbClientGetClientData(client, (void*)gThis);
    if(pThis->OnSize()) return FALSE;
    return TRUE;
}

void CConnectLibVnc::cb_update(rfbClient *client, int x, int y, int w, int h)
{
    //LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_update:(%d, %d, %d, %d)", x, y, w, h);
    CConnectLibVnc* pThis = (CConnectLibVnc*)rfbClientGetClientData(client, (void*)gThis);
    emit pThis->sigUpdateRect(QRect(x, y, w, h), pThis->m_Image);
}

void CConnectLibVnc::cb_got_selection(rfbClient *client, const char *text, int len)
{
    //LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_got_selection:%s", text);
    CConnectLibVnc* pThis = (CConnectLibVnc*)rfbClientGetClientData(client, (void*)gThis);
    if(!pThis->m_pPara->bClipboard) return;
    QClipboard* pClipboard = QApplication::clipboard();
    if(pClipboard)
        pClipboard->setText(text);
}

void CConnectLibVnc::cb_kbd_leds(rfbClient *client, int value, int pad)
{
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_kbd_leds");
}

void CConnectLibVnc::cb_bell(struct _rfbClient *client)
{
    qApp->beep();
}

void CConnectLibVnc::cb_text_chat(rfbClient *client, int value, char *text)
{
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_text_chat");
}

rfbCredential* CConnectLibVnc::cb_get_credential(rfbClient *cl, int credentialType)
{
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_get_credential");
    CConnectLibVnc* pThis = (CConnectLibVnc*)rfbClientGetClientData(cl, (void*)gThis);
    rfbCredential *c = (rfbCredential*)malloc(sizeof(rfbCredential));
    c->userCredential.username = (char*)malloc(RFB_BUF_SIZE);
    memset(c->userCredential.username, 0, RFB_BUF_SIZE);
    c->userCredential.password = (char*)malloc(RFB_BUF_SIZE);
    memset(c->userCredential.password, 0, RFB_BUF_SIZE);

    if(credentialType != rfbCredentialTypeUser) {
        LOG_MODEL_ERROR("LibVnc", "something else than username and password required for authentication\n");
        return NULL;
    }

    LOG_MODEL_INFO("LibVnc", "username and password required for authentication!\n");

    memcpy(c->userCredential.username,
           pThis->m_pPara->szUser.toStdString().c_str(),
           pThis->m_pPara->szUser.toStdString().length());
    memcpy(c->userCredential.password,
           pThis->m_pPara->szPassword.toStdString().c_str(),
           pThis->m_pPara->szPassword.toStdString().length());

    return c;
}

char* CConnectLibVnc::cb_get_password(rfbClient *client)
{
    //LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_get_password");
    CConnectLibVnc* pThis = (CConnectLibVnc*)rfbClientGetClientData(client, (void*)gThis);
    return strdup(pThis->m_pPara->szPassword.toStdString().c_str());
}

int CConnectLibVnc::OnSize()
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

rfbBool CConnectLibVnc::cb_cursor_pos(rfbClient *client, int x, int y)
{
    //LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_cursor_pos:%d,%d", x, y);
    rfbBool bRet = true;
    
    return bRet;
}

void CConnectLibVnc::cb_got_cursor_shape(rfbClient *client,
                                         int xhot, int yhot,
                                         int width, int height,
                                         int bytesPerPixel)
{
    /*
    LOG_MODEL_DEBUG("LibVnc", "CConnectLibVnc::cb_got_cursor_shape:x:%d, y:%d, width:%d, height:%d, bytesPerPixel:%d",
                    xhot, yhot, width, height, bytesPerPixel);//*/
    if(!client->rcSource)
    {
        LOG_MODEL_DEBUG("LibVnc", "client->rcSource is null");
        return;
    }
    CConnectLibVnc* pThis = (CConnectLibVnc*)rfbClientGetClientData(client, (void*)gThis);
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
                            //NOTE: Do not manipulate the memory,
                            //      because the host byte order may be different
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

void CConnectLibVnc::slotMousePressEvent(QMouseEvent* e)
{
    if(!m_pClient) return;
    if(m_pPara && m_pPara->bOnlyView) return;
    //qDebug() << "CConnectLibVnc::slotMousePressEvent" << e->button() << e->buttons();
    unsigned char mask = 0;
    if(e->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(e->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(e->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;

    SendPointerEvent(m_pClient, e->x(), e->y(), mask);
}

void CConnectLibVnc::slotMouseReleaseEvent(QMouseEvent* e)
{
    if(!m_pClient) return;
    if(m_pPara && m_pPara->bOnlyView) return;
    int mask = 0;
    SendPointerEvent(m_pClient, e->x(), e->y(), mask);
}

void CConnectLibVnc::slotMouseMoveEvent(QMouseEvent* e)
{
    //qDebug() << "CConnectLibVnc::slotMouseMoveEvent" << e->button() << e->buttons();
    if(!m_pClient) return;
    if(m_pPara && m_pPara->bOnlyView) return;
    int mask = 0;
    if(e->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(e->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(e->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    SendPointerEvent(m_pClient, e->x(), e->y(), mask);
}

void CConnectLibVnc::slotWheelEvent(QWheelEvent* e)
{
    //vlog.debug("CConnectLibVnc::slotWheelEvent");
    if(!m_pClient) return;
    if(m_pPara && m_pPara->bOnlyView) return;
    int mask = 0;
    
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
    
    SendPointerEvent(m_pClient, e->x(), e->y(), mask);
}

/**
 * @brief CConnectTigerVnc::TranslateRfbKey
 * @param inkey
 * @param shiftModifier
 * @return 
 * @see https://github.com/rfbproto/rfbproto/blob/master/rfbproto.rst#keyevent
 */
uint32_t TranslateRfbKey(quint32 inkey, bool shiftModifier)
{
    uint32_t k = 0;
    
    switch (inkey)
    {
    case Qt::Key_Backspace: k = XK_BackSpace; break;
    case Qt::Key_Tab: k = XK_Tab;break;
    case Qt::Key_Enter: k = XK_Return; break;
    case Qt::Key_Return: k = XK_Return; break;
    case Qt::Key_Escape: k = XK_Escape; break;
    case Qt::Key_Pause: k = XK_Pause; break;
    case Qt::Key_Space: k = XK_space; break;
    case Qt::Key_Delete: k = XK_Delete; break;
    case Qt::Key_Period: k = XK_period; break;
    case Qt::Key_Clear: k = XK_Clear; break;
        
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
    case Qt::Key_Comma: return XK_comma; //,
    case Qt::Key_Equal: k = XK_equal; break;    //=
        
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
    
    if (!shiftModifier)
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
    
    return k;
}

void CConnectLibVnc::slotKeyPressEvent(QKeyEvent* e)
{
    if(!m_pClient) return;
    if(m_pPara && m_pPara->bOnlyView) return;
    bool shiftModifier = false;
    if (e->modifiers() & Qt::ShiftModifier)
        shiftModifier = true;
    uint32_t key = TranslateRfbKey(e->key(), shiftModifier);
    if(key)
        SendKeyEvent(m_pClient, key, TRUE);
}

void CConnectLibVnc::slotKeyReleaseEvent(QKeyEvent* e)
{
    if(!m_pClient) return;
    if(m_pPara && m_pPara->bOnlyView) return;
    bool shiftModifier = false;
    if (e->modifiers() & Qt::ShiftModifier)
        shiftModifier = true;
    uint32_t key = TranslateRfbKey(e->key(), shiftModifier);
    if(key)
        SendKeyEvent(m_pClient, key, FALSE);
}
