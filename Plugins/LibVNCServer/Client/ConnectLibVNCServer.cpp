
#include <QApplication>
#include <QImage>
#include <QClipboard>
#include <QNetworkProxy>
#include <QInputDialog>
#include <QLoggingCategory>

#include "ConnectLibVNCServer.h"

static Q_LOGGING_CATEGORY(log, "LibVNCServer.Connect")
static Q_LOGGING_CATEGORY(logger, "LibVNCServer.Connect.log")

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
    if(nRet < 0)
    {
        qCritical(log) << "vsnprintf fail";
        return;
    }
    if(nRet >= LOG_BUFFER_LENGTH)
    {
        qCritical(log) << "vsprintf buf is short,"
                                << nRet << ">" <<  LOG_BUFFER_LENGTH
                                << ". Truncated it:" << nRet - LOG_BUFFER_LENGTH;
        buf[LOG_BUFFER_LENGTH - 1] = 0;
    }
    qDebug(logger) << buf;
}

CConnectLibVNCServer::CConnectLibVNCServer(CConnecterLibVNCServer *pConnecter, QObject *parent)
    : CConnect(pConnecter, parent),
    m_pClient(nullptr),
    m_pParameter(dynamic_cast<CParameterLibVNCServer*>(pConnecter->GetParameter()))
#ifdef HAVE_LIBSSH
    ,m_pThread(nullptr)
#endif
{
    rfbClientLog = rfbQtClientLog;

    if(!m_pParameter->GetLocalCursor())
    {
        emit sigUpdateCursor(QCursor(Qt::BlankCursor));
    }
}

CConnectLibVNCServer::~CConnectLibVNCServer()
{
    qDebug(log) << "CConnectLibVNCServer::~CConnectLibVNCServer()";
#ifdef HAVE_LIBSSH
    if(m_pThread){
        m_pThread->m_bExit;
        m_pThread->exit();
        m_pThread->deleteLater();
    }
#endif
}

/*
 * return
 *  < 0: error
 *  = 0: Use OnProcess (non-Qt event loop)
 *  > 0: Don't use OnProcess (qt event loop)
 */
CConnect::OnInitReturnValue CConnectLibVNCServer::OnInit()
{
    qDebug(log) << "CConnectLibVNCServer::OnInit()";
    if(m_pClient) Q_ASSERT(false);
    
    m_pClient = rfbGetClient(8, 3, 4);
    if(!m_pClient)
    {
        QString szErr;
        szErr = tr("Protocol version error");
        emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
        qCritical(log) << "rfbGetClient fail";
        return OnInitReturnValue::Fail;
    }
    
    // Set parameters
    m_pClient->programName = strdup(qApp->applicationName().toStdString().c_str());
    m_pClient->appData.shareDesktop = m_pParameter->GetShared();
    m_pClient->appData.viewOnly = m_pParameter->GetOnlyView();
    m_pClient->appData.useRemoteCursor = m_pParameter->GetLocalCursor();
    
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
    
    m_pClient->appData.enableJPEG = m_pParameter->GetJpeg();
    if(m_pClient->appData.enableJPEG)
        m_pClient->appData.qualityLevel = m_pParameter->GetQualityLevel();
    if(m_pParameter->GetEnableCompressLevel())
        m_pClient->appData.compressLevel = m_pParameter->GetCompressLevel();
    
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
    switch(m_pParameter->m_Proxy.GetUsedType())
    {
    case CParameterProxy::TYPE::None:
    {
        // Set server ip and port
        if(m_pParameter->m_Net.GetHost().isEmpty())
        {
            QString szErr;
            szErr = tr("The server is empty, please input it");
            qCritical(log) << szErr;
            emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
            return OnInitReturnValue::Fail;
        }
        auto &net = m_pParameter->m_Net;
        m_pClient->serverHost = strdup(net.GetHost().toStdString().c_str());
        m_pClient->serverPort = net.GetPort();
        
        if(!rfbInitClient(m_pClient, nullptr, nullptr))
        {
            QString szErr;
            szErr = tr("Connect to %1:%2 fail").arg(m_pParameter->m_Net.GetHost(),
                                                    QString::number(m_pParameter->m_Net.GetPort()));
            qCritical(log) <<  szErr;
            emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
            return OnInitReturnValue::Fail;
        }

        QString szInfo = QString("Connect to ") + m_pClient->desktopName;
        qInfo(log) << szInfo;
        
        emit sigSetDesktopSize(m_pClient->width, m_pClient->height);
        emit sigServerName(m_pClient->desktopName);
        emit sigInformation(szInfo);
        emit sigConnected();
        return OnInitReturnValue::UseOnProcess;
    }
    /*case CParameterProxy::TYPE::SockesV5:
    {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::Socks5Proxy);
        auto &net = m_pParameter->m_Proxy.m_SockesV5;
        proxy.setHostName(net.GetHost());
        if(net.GetHost().isEmpty())
        {
            QString szErr;
            szErr = tr("The proxy server is empty, please input it");
            qCritical(log) << szErr;
            emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
            return OnInitReturnValue::Fail;
        }
        proxy.setPort(net.GetPort());
        auto &user = net.m_User;
        proxy.setUser(user.GetUser());
        proxy.setPassword(user.GetPassword());
        m_tcpSocket.setProxy(proxy);
        m_tcpSocket.connectToHost(m_pClient->serverHost, m_pClient->serverPort);
        if (!m_tcpSocket.waitForConnected(3000))
            return OnInitReturnValue::Fail;
        //TODO: The is fail
        m_pClient->sock = m_tcpSocket.socketDescriptor();
        if(!rfbInitClient(m_pClient, nullptr, nullptr))
        {
            QString szErr;
            szErr = tr("Connect to %1:%2 fail").arg(m_pParameter->m_Net.GetHost(),
                                                    QString::number(m_pParameter->m_Net.GetPort()));
            qCritical(log) <<  szErr;
            emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
            return OnInitReturnValue::Fail;
        }
        break;
    }//*/
    /*TODO: add UltraVncRepeater
    case (CParameterConnecter::emProxy) CParameterLibVNCServer::emVncProxy::UltraVncRepeater:
        m_pClient->destHost = strdup(m_pPara->GetProxyHost().toStdString().c_str());
        m_pClient->destPort = m_pPara->GetProxyPort();//*/
    
    //! [Use SSH Tunnel]
#ifdef HAVE_LIBSSH
    case CParameterProxy::TYPE::SSHTunnel:
    {
        // Set SSH parameters
        QSharedPointer<CParameterChannelSSH> parameter(new CParameterChannelSSH());
        auto &ssh = m_pParameter->m_Proxy.m_SSH;
        parameter->setServer(ssh.GetHost());
        parameter->setPort(ssh.GetPort());
        auto &user = ssh.m_User;
        parameter->SetUser(user.GetUser());
        parameter->SetUseSystemFile(user.GetUseSystemFile());
        if(CParameterUser::TYPE::UserPassword == user.GetUsedType()) {
            parameter->SetAuthenticationMethod(SSH_AUTH_METHOD_PASSWORD);
            parameter->SetPassword(user.GetPassword());
        }
        if(CParameterUser::TYPE::PublicKey == user.GetUsedType()) {
            parameter->SetAuthenticationMethod(SSH_AUTH_METHOD_PUBLICKEY);
            parameter->SetPublicKeyFile(user.GetPublicKeyFile());
            parameter->SetPrivateKeyFile(user.GetPrivateKeyFile());
            parameter->SetPassphrase(user.GetPassphrase());
        }
        auto &net = m_pParameter->m_Net;
        parameter->SetRemoteHost(net.GetHost());
        parameter->SetRemotePort(net.GetPort());

        // Start ssh thread
        if(!m_pThread)
            m_pThread = new CSSHTunnelThread(parameter);
        if(!m_pThread)
            return OnInitReturnValue::Fail;
        bool check = connect(m_pThread, SIGNAL(sigServer(quint16)),
                                   this, SLOT(slotConnectProxyServer(quint16)));
        Q_ASSERT(check);
        check = connect(m_pThread, SIGNAL(sigError(int,QString)),
                        this, SIGNAL(sigError(int,QString)));
        Q_ASSERT(check);
        check = connect(m_pThread, SIGNAL(sigDisconnect()),
                        this, SIGNAL(sigDisconnect()));
        Q_ASSERT(check);
        m_pThread->start();
        
        return OnInitReturnValue::UseOnProcess;
    }
#endif
    //! [Use SSH Tunnel]
    default:
        break;
    }
    
    return OnInitReturnValue::UseOnProcess;
}

int CConnectLibVNCServer::OnClean()
{
    qDebug(log) << "CConnectLibVNCServer::OnClean()";
    if(m_pThread)
        m_pThread->m_bExit = true;
    if(m_pClient)
    {
        m_tcpSocket.close();
        //m_pClient->sock = -1; //RFB_INVALID_SOCKET;
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
    //LOG_MODEL_DEBUG("CConnectLibVNCServer", "CConnectLibVNCServer::Process()");
    int nRet = 0;
    if(!m_pClient)
    {
        Q_ASSERT(m_pClient);
        return -1;
    }

    struct timeval timeout = {0, 50000};
    fd_set set;
    FD_ZERO(&set);
    if(RFB_INVALID_SOCKET != m_pClient->sock)
    {
        FD_SET(m_pClient->sock, &set);
    }
    rfbSocket eventFd = m_Event.GetFd();
    if(RFB_INVALID_SOCKET != eventFd)
    {
        FD_SET(eventFd, &set);
    }
    rfbSocket fd = RFB_INVALID_SOCKET;
    fd = std::max(eventFd, m_pClient->sock);
    //qDebug(log) << "fd:" << fd << "select m_pClient->sock:" << (int)m_pClient->sock << "event:" << eventFd;
    nRet = select(fd + 1, &set, NULL, NULL, &timeout);
    //qDebug(log) << "select end";
    if (nRet < 0) {
        qCritical(log) << "select fail:" << nRet << "errno" << errno << strerror(errno);
        return nRet;
    }

    /*if(0 == nRet)
    {
        //qDebug(log) << "Time out";
        return 0;
    }//*/

    if(RFB_INVALID_SOCKET != eventFd && FD_ISSET(eventFd, &set))
    {
        //qDebug(log) << "fires event";
        m_Event.Reset();
    }

    if(RFB_INVALID_SOCKET != m_pClient->sock
        && FD_ISSET(m_pClient->sock, &set)) {
        //qDebug(log) << "HandleRFBServerMessage";
        if(!HandleRFBServerMessage(m_pClient))
        {
            qCritical(log) << "HandleRFBServerMessage fail";
            return -2;
        }
    }
    
    return 0;
}

int CConnectLibVNCServer::WakeUp()
{
    return m_Event.WakeUp();
}

void CConnectLibVNCServer::slotClipBoardChanged()
{
    if(!m_pParameter || !m_pParameter->GetClipboard() || !m_pClient) return;
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
    //qDebug(log) << "CConnectLibVnc::cb_resize:" << client->width << client->height;
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(client, (void*)gThis);
    if(pThis->OnSize()) return FALSE;
    return TRUE;
}

void CConnectLibVNCServer::cb_update(rfbClient *client, int x, int y, int w, int h)
{
    //qDebug(LibVNCServer, "CConnectLibVnc::cb_update:(%d, %d, %d, %d)", x, y, w, h);
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(client, (void*)gThis);
    emit pThis->sigUpdateRect(QRect(x, y, w, h), pThis->m_Image);
}

void CConnectLibVNCServer::cb_got_selection(rfbClient *client, const char *text, int len)
{
    //qDebug(LibVNCServer, "CConnectLibVnc::cb_got_selection:%s", text);
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(client, (void*)gThis);
    if(!pThis->m_pParameter->GetClipboard()) return;
    QClipboard* pClipboard = QApplication::clipboard();
    if(pClipboard)
        pClipboard->setText(text);
}

void CConnectLibVNCServer::cb_kbd_leds(rfbClient *client, int value, int pad)
{
    qDebug(log) << "CConnectLibVnc::cb_kbd_leds";
}

void CConnectLibVNCServer::cb_bell(struct _rfbClient *client)
{
    qApp->beep();
}

void CConnectLibVNCServer::cb_text_chat(rfbClient *client, int value, char *text)
{
    qDebug(log) << "CConnectLibVnc::cb_text_chat";
}

rfbCredential* CConnectLibVNCServer::cb_get_credential(rfbClient *cl, int credentialType)
{
    qDebug(log) <<"CConnectLibVnc::cb_get_credential";
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(cl, (void*)gThis);
    rfbCredential *c = (rfbCredential*)malloc(sizeof(rfbCredential));
    c->userCredential.username = (char*)malloc(RFB_BUF_SIZE);
    memset(c->userCredential.username, 0, RFB_BUF_SIZE);
    c->userCredential.password = (char*)malloc(RFB_BUF_SIZE);
    memset(c->userCredential.password, 0, RFB_BUF_SIZE);

    if(credentialType != rfbCredentialTypeUser) {
        qCritical(log) << "something else than username and password required for authentication";
        return NULL;
    }

    qDebug(log) << "Username and password required for authentication!";

    memcpy(c->userCredential.username,
           pThis->m_pParameter->m_Net.m_User.GetUser().toStdString().c_str(),
           pThis->m_pParameter->m_Net.m_User.GetUser().toStdString().length());
    memcpy(c->userCredential.password,
           pThis->m_pParameter->m_Net.m_User.GetPassword().toStdString().c_str(),
           pThis->m_pParameter->m_Net.m_User.GetPassword().toStdString().length());

    return c;
}

char* CConnectLibVNCServer::cb_get_password(rfbClient *client)
{
    //LOG_MODEL_ERROR("LibVNCServer", "CConnectLibVnc::cb_get_password");
    CConnectLibVNCServer* pThis = (CConnectLibVNCServer*)rfbClientGetClientData(client, (void*)gThis);
    QString szPassword = pThis->m_pParameter->m_Net.m_User.GetPassword();
    if(szPassword.isEmpty())
    {
        int nRet = QDialog::Rejected;
        emit pThis->sigBlockShowWidget("CDlgLibVNCServerPassword", nRet, pThis->m_pParameter);
        if(QDialog::Accepted == nRet)
        {
            szPassword = pThis->m_pParameter->m_Net.m_User.GetPassword();
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
    
    emit sigSetDesktopSize(m_pClient->width, m_pClient->height);
    return nRet;
}

rfbBool CConnectLibVNCServer::cb_cursor_pos(rfbClient *client, int x, int y)
{
    //qDebug(LibVNCServer, "CConnectLibVnc::cb_cursor_pos:%d,%d", x, y);
    rfbBool bRet = true;
    
    return bRet;
}

void CConnectLibVNCServer::cb_got_cursor_shape(rfbClient *client,
                                         int xhot, int yhot,
                                         int width, int height,
                                         int bytesPerPixel)
{
    /*
    qDebug(LibVNCServer, "CConnectLibVnc::cb_got_cursor_shape:x:%d, y:%d, width:%d, height:%d, bytesPerPixel:%d",
                    xhot, yhot, width, height, bytesPerPixel);//*/
    if(!client->rcSource)
    {
        qCritical(log) << "client->rcSource is null";
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

void CConnectLibVNCServer::mousePressEvent(QMouseEvent *event)
{
    if(!m_pClient) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    //qDebug(log) << "CConnectLibVnc::slotMousePressEvent" << e->button() << e->buttons();
    unsigned char mask = 0;
    if(event->button() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(event->button() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(event->button() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    
    QPoint pos = event->pos();
    SendPointerEvent(m_pClient, pos.x(), pos.y(), mask);
}

void CConnectLibVNCServer::mouseReleaseEvent(QMouseEvent *event)
{
    if(!m_pClient) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    int mask = 0;
    QPoint pos = event->pos();
    SendPointerEvent(m_pClient, pos.x(), pos.y(), mask);
}

void CConnectLibVNCServer::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug(log) << "CConnectLibVnc::slotMouseMoveEvent" << buttons << pos;
    if(!m_pClient) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    int mask = 0;
    if(event->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(event->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(event->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    QPoint pos = event->pos();
    SendPointerEvent(m_pClient, pos.x(), pos.y(), mask);
}

void CConnectLibVNCServer::wheelEvent(QWheelEvent *event)
{
    //qDebug(log) << "CConnectLibVnc::slotWheelEvent" << buttons << pos << angleDelta;
    if(!m_pClient) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    int mask = 0;
    if(event->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(event->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(event->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    
    QPoint d = event->angleDelta();
    if(d.y() > 0)
        mask |= 0x8;
    if(d.y() < 0)
        mask |= 0x10;
    if(d.x() < 0)
        mask |= 0x20;
    if(d.x() > 0)
        mask |= 0x40;
    
    QPointF pos;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    pos = event->position();
#else
    pos = event->pos();
#endif
    
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

void CConnectLibVNCServer::keyPressEvent(QKeyEvent *event)
{
    if(!m_pClient) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    bool modifier = false;
    if (event->modifiers() & Qt::ShiftModifier)
        modifier = true;
    uint32_t k = TranslateRfbKey(event->key(), modifier);
    SendKeyEvent(m_pClient, k, TRUE);
}

void CConnectLibVNCServer::keyReleaseEvent(QKeyEvent *event)
{
    if(!m_pClient) return;
    if(m_pParameter && m_pParameter->GetOnlyView()) return;
    bool modifier = false;
    if (event->modifiers() & Qt::ShiftModifier)
        modifier = true;
    uint32_t k = TranslateRfbKey(event->key(), modifier);
    SendKeyEvent(m_pClient, k, FALSE);
}

//! [connect local socket server]
void CConnectLibVNCServer::slotConnectProxyServer(quint16 nPort)
{
    QString szErr;
    qDebug(log) << "CConnectLibVNCServer::slotConnectServer";
    // Set server ip and port
    m_pClient->serverHost = strdup("127.0.0.1");
    m_pClient->serverPort = nPort;
    szErr = tr("Connect to local socket server %1:%2")
                .arg(m_pClient->serverHost,
                     QString::number(m_pClient->serverPort));
    qDebug(log) << szErr;
    if(!rfbInitClient(m_pClient, nullptr, nullptr))
    {
        szErr += tr(" fail");
        qCritical(log) << szErr;
        //emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
        return;
    }
    
    QString szInfo = QString("Connect to ") + m_pClient->desktopName;
    qInfo(log) << szInfo;
    
    emit sigSetDesktopSize(m_pClient->width, m_pClient->height);
    emit sigServerName(m_pClient->desktopName);
    emit sigInformation(szInfo);
    emit sigConnected();
}
//! [connect local socket server]
