// Author: Kang Lin <kl222@126.com>

#include "ConnectVnc.h"
#include "ConnecterDesktopThread.h"

#ifndef WIN32
#include <string.h>
#include "network/UnixSocket.h"
#endif

#include <rfb/CMsgWriter.h>
#include <rfb/Hostname.h>
#include "rfb/LogWriter.h"
#include <rfb/util.h>
#include <rfb/clipboardTypes.h>
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include <rfb/fenceTypes.h>

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
#include <QLoggingCategory>
#include <QThread>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QNetworkProxy>
#include <QMutex>
#include <QInputDialog>

#include "rfb/Security.h"
#include "rfb/Exception.h"
#ifdef HAVE_GNUTLS
#include "rfb/CSecurityTLS.h"
#endif

#ifdef HAVE_ICE
    #include "Ice.h"
    #include "ChannelIce.h"
#endif

#ifdef HAVE_LIBSSH
    #include "ChannelSSHTunnel.h"
#endif

static Q_LOGGING_CATEGORY(log, "VNC.Connect")
static Q_LOGGING_CATEGORY(logVNC, "VNC.Log")
    
class VncLogger: public rfb::Logger
{
public:
    VncLogger(const char *name) : rfb::Logger(name)
    {}
    void write(int level, const char *logname, const char *text) override
    {
        if (level >= rfb::LogWriter::LEVEL_DEBUG) {
            qDebug(logVNC) << logname << text;
        } else if (level >= rfb::LogWriter::LEVEL_INFO) {
            qInfo(logVNC) << logname << text;
        } else if (level >= rfb::LogWriter::LEVEL_STATUS) {
            qWarning(logVNC) << logname << text;
        } else {
            qCritical(logVNC) << logname << text;
        }
    }
};

static VncLogger g_logger("Rabbit");

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

CConnectVnc::CConnectVnc(CConnecterDesktopThread *pConnecter, QObject *parent)
    : CConnectDesktop(pConnecter, parent),
      m_pPara(nullptr)
{
    static bool initlog = false;
    if(!initlog)
    {
        g_logger.registerLogger();
        rfb::LogWriter::setLogParams("*:Rabbit:100");
        /*rfb::initStdIOLoggers();
        rfb::LogWriter::setLogParams("*:stderr:100");
        QString szFile = RabbitCommon::CDir::Instance()->GetDirLog()
                + QDir::separator()
                + "TigerVnc.log";
        rfb::initFileLogger(szFile.toStdString().c_str());
        rfb::LogWriter::setLogParams("*:file:100");
        */
        initlog = true;
    }
    m_pPara = dynamic_cast<CParameterVnc*>(pConnecter->GetParameter());
    Q_ASSERT(m_pPara);

    if(!m_pPara->GetLocalCursor())
    {
        emit sigUpdateCursor(QCursor(Qt::BlankCursor));
    }
}

CConnectVnc::~CConnectVnc()
{
    qDebug(log) << "CConnectVnc::~CConnectVnc()";
}

int CConnectVnc::SetPara()
{
    QDir d(os::getvncconfigdir());
    if(!d.exists())
        d.mkpath(os::getvncconfigdir());
    QDir dstat(os::getvncstatedir());
    if(!dstat.exists())
        dstat.mkpath(os::getvncstatedir());
    QDir dvnc(os::getvncdatadir());
    if(!dvnc.exists())
        dvnc.mkpath(os::getvncdatadir());
    
    setServerName(m_pPara->m_Net.GetHost().toStdString().c_str());
    setShared(m_pPara->GetShared());
    supportsLocalCursor = m_pPara->GetLocalCursor();
    supportsCursorPosition = m_pPara->GetCursorPosition();
    supportsDesktopResize = m_pPara->GetSupportsDesktopResize();
    supportsLEDState = m_pPara->GetLedState();
    
    std::list<uint32_t> sec;
    switch(m_pPara->m_Net.m_User.GetUsedType()) {
    case CParameterUser::TYPE::None:
        sec.push_back(rfb::secTypeNone);
        break;  
    case CParameterUser::TYPE::OnlyPassword:
        sec.push_back(rfb::secTypeVncAuth);
        break;
    case CParameterUser::TYPE::UserPassword:
        sec.push_back(rfb::secTypePlain);
        break;
    case CParameterUser::TYPE::OnlyPasswordX509None:
        sec.push_back(rfb::secTypeTLSVnc);
        break;
    case CParameterUser::TYPE::OnlyPasswordX509:
        sec.push_back(rfb::secTypeX509Vnc);
        break;
    case CParameterUser::TYPE::UserPasswordX509None:
        sec.push_back(rfb::secTypeTLSPlain);
        break;
    case CParameterUser::TYPE::UserPasswordX509:
        sec.push_back(rfb::secTypeX509Plain);
        break;
    //TODO: add authentication
    }
    //*
    qDebug(log) << "secTypes:" << security.GetEnabledSecTypes() << security.ToString()
                << "new:" << sec; // << "Type:" << m_pPara->m_Net.m_User.GetType();//*/
    security.SetSecTypes(sec);

    // Set Preferred Encoding
    setPreferredEncoding(m_pPara->GetPreferredEncoding());
    setCompressLevel(m_pPara->GetCompressLevel());
    setQualityLevel(m_pPara->GetQualityLevel());
    
    // Set server pixmap format
    updatePixelFormat();
    return 0;
}

/*
 * \return
 * \li OnInitReturnValue::Fail: error
 * \li OnInitReturnValue::UseOnProcess: Use OnProcess (non-Qt event loop)
 * \li OnInitReturnValue::NotUseOnProcess: Don't use OnProcess (qt event loop)
 */
CConnect::OnInitReturnValue CConnectVnc::OnInit()
{
    qDebug(log) << "CConnectVnc::OnInit()";
    int nRet = 0;
    
    if(m_pPara->GetIce())
        nRet = IceInit();
    else {
#ifdef HAVE_LIBSSH
        if(m_pPara->m_Proxy.GetUsedType() == CParameterProxy::TYPE::SSHTunnel) {
            nRet = SSHInit();
            if(nRet) return OnInitReturnValue::Fail; // error
            return OnInitReturnValue::UseOnProcess;
        }
#endif
        nRet = SocketInit();
    }
    if(nRet) return OnInitReturnValue::Fail; // error
    // Don't use OnProcess (qt event loop)
    return OnInitReturnValue::NotUseOnProcess;
}

int CConnectVnc::IceInit()
{
#ifdef HAVE_ICE
    CIceSignal* pSignal = CICE::Instance()->GetSignal().data();
    if(!pSignal || !pSignal->IsConnected())
    {
        qCritical(log) << "The g_pSignal is null. or signal don't connect server";
        return -1;
    }
    
    qInfo(log, "Connected to signal server:%s:%d; user:%s",
                   CICE::Instance()->GetParameter()->getSignalServer().toStdString().c_str(),
                   CICE::Instance()->GetParameter()->getSignalPort(),
                   CICE::Instance()->GetParameter()->getSignalUser().toStdString().c_str());
    auto channel = QSharedPointer<CChannelIce>(new CChannelIce(pSignal));
    if(!channel)
    {
        qCritical(log) << "new CDataChannelIce fail";
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

int CConnectVnc::SocketInit()
{
    int nRet = 0;
    try{
        QTcpSocket* pSock = new QTcpSocket(this);
        if(!pSock)
            return -2;
        m_DataChannel = QSharedPointer<CChannel>(new CChannel(pSock));
        if(!m_DataChannel) {
            qCritical(log) << "New CChannel fail";
            return -1;
        }
        
        SetChannelConnect(m_DataChannel);

        if(!m_DataChannel->open(QIODevice::ReadWrite))
        {
            qCritical(log) << "Open channel fail";
            return -3;
        }

        QNetworkProxy::ProxyType type = QNetworkProxy::NoProxy;
        // Set sock
        switch(m_pPara->m_Proxy.GetUsedType())
        {
        case CParameterProxy::TYPE::SockesV5:
            type = QNetworkProxy::Socks5Proxy;
            break;
        case CParameterProxy::TYPE::Default:
            type = QNetworkProxy::DefaultProxy;
            break;
        case CParameterProxy::TYPE::None:
        default:
            break;
        }
        
        switch(type) {
        case QNetworkProxy::DefaultProxy:
        {
            pSock->setProxy(QNetworkProxy::applicationProxy());
            break;
        }
        case QNetworkProxy::Socks5Proxy:
        {
            QNetworkProxy proxy;
            proxy.setType(type);
            auto &net = m_pPara->m_Proxy.m_SockesV5;
            if(net.GetHost().isEmpty())
            {
                QString szErr;
                szErr = tr("The proxy server is empty, please input it");
                qCritical(log) << szErr;
                emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
                return -4;
            }
            proxy.setHostName(net.GetHost());
            proxy.setPort(net.GetPort());
            auto &user = net.m_User;
            proxy.setUser(user.GetUser());
            proxy.setPassword(user.GetPassword());
            pSock->setProxy(proxy);
            break;
        }
        default:
            break;
        }

        if(m_pPara->m_Net.GetHost().isEmpty())
        {
            QString szErr;
            szErr = tr("The server is empty, please input it");
            qCritical(log) << szErr;
            emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
            return -5;
        }
        pSock->connectToHost(m_pPara->m_Net.GetHost(), m_pPara->m_Net.GetPort());
        
        QString serverHost;
        serverHost = pSock->peerName();
        qDebug(log) << "Server Host:" << serverHost;
        setServerName(serverHost.toStdString().c_str());
        
        return nRet;
    } catch (rdr::Exception& e) {
        qCritical(log) << "SocketInit exception:" << e.str();
        emit sigError(-6, e.str());
        nRet = -6;
    }
    return nRet;
}

int CConnectVnc::SSHInit()
{
    bool check = false;
#ifdef HAVE_LIBSSH
    QSharedPointer<CParameterChannelSSH> parameter(new CParameterChannelSSH());
    auto &ssh = m_pPara->m_Proxy.m_SSH;
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
    auto &net = m_pPara->m_Net;
    parameter->SetRemoteHost(net.GetHost());
    parameter->SetRemotePort(net.GetPort());
    
    auto channel = QSharedPointer<CChannelSSHTunnel>(new CChannelSSHTunnel(parameter));
    if(!channel) {
        qCritical(log) << "New CChannelSSHTunnel fail";
        return -1;
    }
    m_DataChannel = channel;
    SetChannelConnect(m_DataChannel);
    check = connect(channel.data(), SIGNAL(sigBlockShowMessageBox(const QString&, const QString&, QMessageBox::StandardButtons, QMessageBox::StandardButton&, bool&, QString)),
                    this, SIGNAL(sigBlockShowMessageBox(const QString&, const QString&, QMessageBox::StandardButtons, QMessageBox::StandardButton&, bool&, QString)));
    Q_ASSERT(check);
    check = connect(channel.data(), SIGNAL(sigBlockShowWidget(const QString&, int&, void*)),
                    this, SIGNAL(sigBlockShowWidget(const QString&, int&, void*)));
    Q_ASSERT(check);
    if(!channel->open(QIODevice::ReadWrite))
    {
        QString szErr;
        szErr = tr("Failed to open SSH tunnel:");
        szErr += "(" + m_pPara->m_Proxy.m_SSH.GetHost();
        szErr += ":";
        szErr += QString::number(m_pPara->m_Proxy.m_SSH.GetPort());
        szErr += " - ";
        szErr += m_pPara->m_Net.GetHost();
        szErr += ":";
        szErr += QString::number(m_pPara->m_Net.GetPort()) + ")";
        QString szMsg = szErr + "\n" + channel->errorString();
        emit sigShowMessageBox(tr("Error"), szMsg, QMessageBox::Critical);
        return -2;
    }
#endif
    return 0;
}

int CConnectVnc::SetChannelConnect(QSharedPointer<CChannel> channel)
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
                    this, SLOT(slotChannelError(int, const QString&)));
    Q_ASSERT(check);
    return 0;
}

int CConnectVnc::OnClean()
{
    qDebug(log) << "CConnectVnc::OnClean()";
    close();
    setStreams(nullptr, nullptr);
    if(m_DataChannel) {
        m_DataChannel->close();
    }
    emit sigDisconnected();
    return 0;
}

/*!
 * \~chinese 插件连接的具体操作处理。因为此插件是非Qt事件，所以在此函数中等待。
 *
 * \~english Specific operation processing of plug-in connection.
 *           Because of it is a non-Qt event loop, so wait in here.
 *
 * \~
 * \return
 *    \li >= 0: continue, Interval call time (msec)
 *    \li = -1: stop
 *    \li < -1: error
 * \see CConnect::Connect() CConnect::slotTimeOut()
 */
int CConnectVnc::OnProcess()
{
    int nRet = 0;
#ifdef HAVE_LIBSSH
    if(m_pPara->m_Proxy.GetUsedType() == CParameterProxy::TYPE::SSHTunnel) {
        CChannelSSHTunnel* channel = (CChannelSSHTunnel*)m_DataChannel.data();
        if(channel)
            nRet = channel->Process();
    }
#endif

    return nRet;
}

int CConnectVnc::WakeUp()
{
#ifdef HAVE_LIBSSH
    if(m_pPara->m_Proxy.GetUsedType() == CParameterProxy::TYPE::SSHTunnel) {
        CChannelSSHTunnel* channel = (CChannelSSHTunnel*)m_DataChannel.data();
        if(channel)
            channel->WakeUp();
    }
#endif
    return 0;
}

void CConnectVnc::slotConnected()
{
    //qDebug(log) << "CConnectVnc::slotConnected()";
    if(m_pPara->GetIce())
        qInfo(log) << "Connected to peer " + m_pPara->GetPeerUser();
    else {
        auto &net = m_pPara->m_Net;
        QString szInfo = "Connected to "
                         + net.GetHost() + ":" + QString::number(net.GetPort());
#ifdef HAVE_LIBSSH
        if(CParameterProxy::TYPE::SSHTunnel == m_pPara->m_Proxy.GetUsedType())
        {
            auto &ssh = m_pPara->m_Proxy.m_SSH;
            szInfo += " with ssh turnnel: " + ssh.GetHost()
                      + ":" + QString::number(ssh.GetPort());
        }
#endif
        qInfo(log) << szInfo;
    }
    int nRet = SetPara();
    if(nRet)
    {
        emit sigDisconnect();
        return;
    }
    m_InStream = QSharedPointer<rdr::InStream>(new CInStreamChannel(m_DataChannel.data()));
    m_OutStream = QSharedPointer<rdr::OutStream>(new COutStreamChannel(m_DataChannel.data()));
    if(!(m_InStream && m_OutStream))
    {
        qCritical(log) << "m_InStream or m_OutStream is null";
        emit sigDisconnect();
        return;
    }
    setStreams(m_InStream.data(), m_OutStream.data());
    initialiseProtocol();
}

void CConnectVnc::slotDisConnected()
{
    QString szInfo;
    szInfo = "CConnectVnc::slotDisConnected() from "
             + m_pPara->m_Net.GetHost() + ":"
             + QString::number(m_pPara->m_Net.GetPort());
#ifdef HAVE_LIBSSH
    if(CParameterProxy::TYPE::SSHTunnel == m_pPara->m_Proxy.GetUsedType())
    {
        auto &ssh = m_pPara->m_Proxy.m_SSH;
        szInfo += " with ssh turnnel: " + ssh.GetHost()
                  + ":" + QString::number(ssh.GetPort());
    }
#endif
    qInfo(log) << szInfo;
}

void CConnectVnc::slotReadyRead()
{
    //qDebug(log) << "CConnectVnc::slotReadyRead";
    QString szErr("processMsg exception: ");
    int nRet = -1;
    try {
        getOutStream()->flush();
        
        getOutStream()->cork(true);
        while(processMsg())
            ;
        getOutStream()->cork(false);
        return;
    } catch (rfb::AuthFailureException& e) {
        szErr = tr("Logon to ");
        szErr += m_pPara->m_Net.GetHost();
        szErr += ":";
        szErr += QString::number(m_pPara->m_Net.GetPort());
        szErr += tr(" fail.");
        QString szMsg = szErr + "\n" + tr("Please check that the username and password are correct.") + "\n";
        emit sigShowMessageBox(tr("Error"), szMsg, QMessageBox::Critical);

        szErr += " [";
        szErr += e.str();
        szErr += "]";
    } catch (rfb::ConnFailedException& e) {
        QString szErr;
        szErr = tr("Connect to ");
        szErr += m_pPara->m_Net.GetHost();
        szErr += ":";
        szErr += QString::number(m_pPara->m_Net.GetPort());
        szErr += tr(" fail.");
        szErr += " [";
        szErr += e.str();
        szErr += "]";
        emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
    } catch (rdr::EndOfStream& e) {
        szErr += e.str();
    } catch(rdr::GAIException &e) {
        nRet = e.err;
        szErr += "[" + QString::number(e.err) + "] " + e.str();
    } catch(rdr::SystemException &e) {
        nRet = e.err;
        szErr += "[" + QString::number(e.err) + "] " + e.str();
    } catch (rdr::Exception& e) {
        szErr += e.str();
    } catch (std::exception &e) {
        szErr += e.what();
    } catch(...) {
        szErr += "unknown exception";
    }
    qCritical(log) << szErr;
    emit sigError(nRet, szErr);
    emit sigDisconnect();
}

void CConnectVnc::slotChannelError(int nErr, const QString& szErr)
{
    qDebug(log) << "Channel error:" << nErr << "-" << szErr;
    emit sigError(nErr, szErr);
    emit sigDisconnected();
}

// initDone() is called when the serverInit message has been received.  At
// this point we create the desktop window and display it.  We also tell the
// server the pixel format and encodings to use and request the first update.
void CConnectVnc::initDone()
{
    Q_ASSERT(m_pPara);
    qDebug(log, "initDone:\n%s", ConnectInformation().toStdString().c_str());
    
    emit sigSetDesktopSize(server.width(), server.height());
    QString szName = QString::fromUtf8(server.name());
    emit sigServerName(szName);

    //Set viewer frame buffer
    setFramebuffer(new CFramePixelBuffer(server.width(), server.height()));
    
    // Force a switch to the format and encoding we'd like
    // Set Preferred Encoding
    setPreferredEncoding(m_pPara->GetPreferredEncoding());
    updatePixelFormat();
    
    emit sigConnected();
}

void CConnectVnc::resizeFramebuffer()
{
    rfb::ModifiablePixelBuffer* buffer = getFramebuffer();

    qDebug(log) << "CConnectVnc::resizeFramebuffer: new:"
                << server.width() << server.height()
                << "old:" << buffer->width() << buffer->height();

    if(server.width() == buffer->width() && server.height() == buffer->height())
        return;

    //Set viewer frame buffer
    setFramebuffer(new CFramePixelBuffer(server.width(), server.height()));
    emit sigSetDesktopSize(server.width(), server.height());
}

void CConnectVnc::setColourMapEntries(int firstColour, int nColours, uint16_t *rgbs)
{
    qCritical(log) << "Invalid SetColourMapEntries from server!";
}

void CConnectVnc::bell()
{
    qApp->beep();
}

void CConnectVnc::setLEDState(unsigned int state)
{
    qDebug(log) << "CConnectVnc::setLEDState" << state;
    CConnection::setLEDState(state);
    emit sigUpdateLedState(state);
}

void CConnectVnc::setCursor(int width, int height, const rfb::Point &hotspot, const uint8_t *data)
{
    //qDebug(log) << "CConnectVnc::setCursor x:" << hotspot.x << ";y:" << hotspot.y;
    if ((width == 0) || (height == 0)) {
        QImage cursor(1, 1, QImage::Format_ARGB32);
        uint8_t *buffer = cursor.bits();
        memset(buffer, 0, 4);
        emit sigUpdateCursor(QCursor(QPixmap::fromImage(cursor), hotspot.x, hotspot.y));
    } else {
        QImage cursor(width, height, QImage::Format_ARGB32);
        uint8_t *buffer = cursor.bits();
        memcpy(buffer, data, width * height * 4);
        emit sigUpdateCursor(QCursor(QPixmap::fromImage(cursor), hotspot.x, hotspot.y));
    }
}

void CConnectVnc::setCursorPos(const rfb::Point &pos)
{
    //qDebug(log) << "CConnectVnc::setCursorPos x:" << pos.x << ";y:" << pos.y;
    emit sigUpdateCursorPosition(QPoint(pos.x, pos.y));
}

void CConnectVnc::fence(uint32_t flags, unsigned int len, const uint8_t data[])
{
    //qDebug(log, "CConnectVnc::fence:flags:0x%X; len:%d", flags, len);
    CMsgHandler::fence(flags, len, data);
    if (!(flags & rfb::fenceFlagRequest))
        return;
    
    // We handle everything synchronously so we trivially honor these modes
    flags = flags & (rfb::fenceFlagBlockBefore | rfb::fenceFlagBlockAfter);
    writer()->writeFence(flags, len, data);
    return;   
}

void CConnectVnc::getUserPasswd(bool secure, std::string *user, std::string *password)
{
    if(password)
    {
        auto &user = m_pPara->m_Net.m_User;
        *password = user.GetPassword().toStdString();
        if(user.GetPassword().isEmpty())
        {
            int nRet = QDialog::Rejected;
            emit sigBlockShowWidget("CDlgGetPasswordVNC", nRet, m_pPara);
            if(QDialog::Accepted == nRet)
            {
                *password = user.GetPassword().toStdString();
            }
        }
    }
}

int CConnectVnc::getX509File(std::string *ca, std::string *crl)
{
    auto &user = m_pPara->m_Net.m_User;
    if(ca)
        *ca = user.GetCAFile().toStdString().c_str();
    if(crl)
        *crl = user.GetCRLFile().toStdString().c_str();
    return 0;
}

bool CConnectVnc::showMsgBox(rfb::MsgBoxFlags flags, const char *title, const char *text)
{
    qDebug(log) << title << text;
    QMessageBox::StandardButton nRet = QMessageBox::No;
    QMessageBox::StandardButtons fgBtn = QMessageBox::No;
    bool bCheckBox = 0;
    if((int)flags & (int)rfb::MsgBoxFlags::M_OK)
        fgBtn |= QMessageBox::Ok;
        
    if((int)flags & (int)rfb::MsgBoxFlags::M_OKCANCEL)
        fgBtn |= QMessageBox::Ok | QMessageBox::Cancel;
    if((int)flags & (int)rfb::MsgBoxFlags::M_YESNO)
        fgBtn |= QMessageBox::Yes | QMessageBox::No;

    emit sigBlockShowMessageBox(QString(title), QString(text),
                             fgBtn, nRet, bCheckBox);
    if(QMessageBox::Ok == nRet
        || QMessageBox::Yes == nRet)
        return true;
    return false;
}

// setName() is called when the desktop name changes
void CConnectVnc::setName(const char *name)
{
    qDebug(log) << "CConnectVnc::setName:" << name;
    CConnection::setName(name);
    QString szName = QString::fromUtf8(server.name());
    emit sigServerName(szName);
}

// framebufferUpdateStart() is called at the beginning of an update.
// Here we try to send out a new framebuffer update request so that the
// next update can be sent out in parallel with us decoding the current
// one.
void CConnectVnc::framebufferUpdateStart()
{
    //qDebug(log) << "CConnectVnc::framebufferUpdateStart()";
    CConnection::framebufferUpdateStart();
}

// framebufferUpdateEnd() is called at the end of an update.
// For each rectangle, the FdInStream will have timed the speed
// of the connection, allowing us to select format and encoding
// appropriately, and then request another incremental update.
void CConnectVnc::framebufferUpdateEnd()
{
    //qDebug(log) << "CConnectVnc::framebufferUpdateEnd()";
    rfb::CConnection::framebufferUpdateEnd();
            
    if(m_pPara /*&& m_pPara->GetBufferEndRefresh()*/)
    {
        const QImage& img = dynamic_cast<CFramePixelBuffer*>(getFramebuffer())->getImage();
        emit sigUpdateRect(img);
    }
}

// requestNewUpdate() requests an update from the server, having set the
// format and encoding appropriately.
void CConnectVnc::updatePixelFormat()
{
    Q_ASSERT(m_pPara);
    
    if(!m_pPara) return;
    rfb::PixelFormat pf;
    
    switch (m_pPara->GetColorLevel()) {
    case CParameterVnc::Full:
        pf = fullColourPF;
        break;
    case CParameterVnc::Medium:
        pf = mediumColourPF;
        break;
    case CParameterVnc::Low:
        pf = lowColourPF;
        break;
    case CParameterVnc::VeryLow:
        pf = verylowColourPF;
        break;
    }
    
    char str[256];
    pf.print(str, 256);
    qInfo(log) << "Update pixel format:" << str;
    setPF(pf);
}

void CConnectVnc::mousePressEvent(QMouseEvent *event)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    
    unsigned char mask = 0;
    if(event->button() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(event->button() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(event->button() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    
    QPoint pos = event->pos();
    rfb::Point p(pos.x(), pos.y());

    /*
    qDebug(log) << "CConnectVnc::slotMousePressEvent buttons:"
                << event->buttons() << event->button() << pos << mask;//*/
    try{
        writer()->writePointerEvent(p, mask);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

void CConnectVnc::mouseReleaseEvent(QMouseEvent *event)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    int mask = 0;
    QPoint pos = event->pos();
    rfb::Point p(pos.x(), pos.y());
    /*
    qDebug(log) << "CConnectVnc::slotMouseReleaseEvent buttons:"
                << event->buttons() << event->button() << pos << mask;//*/
    try{
        writer()->writePointerEvent(p, mask);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

void CConnectVnc::mouseMoveEvent(QMouseEvent *event)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    int mask = 0;
    QPoint pos = event->pos();
    rfb::Point p(pos.x(), pos.y());
    if(event->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(event->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(event->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    /*
    qDebug(log) << "CConnectVnc::slotMouseMoveEvent buttons:"
                << event->buttons() << event->button() << pos << mask;//*/
    try{
        writer()->writePointerEvent(p, mask);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

// https://github.com/rfbproto/rfbproto/blob/master/rfbproto.rst#pointerevent
void CConnectVnc::wheelEvent(QWheelEvent *event)
{
    /*
    qDebug(log) << "CConnectVnc::slotWheelEvent buttons:"
                << event->buttons() << event->angleDelta() << pos;//*/
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
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

    rfb::Point p(pos.x(), pos.y());

    try{
        writer()->writePointerEvent(p, mask);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

void CConnectVnc::keyPressEvent(QKeyEvent *event)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    bool modifier = false;
    if (event->modifiers() & Qt::ShiftModifier)
        modifier = true;
    //qDebug(log) << "slotKeyPressEvent key:" << key << modifiers;
    uint32_t k = TranslateRfbKey(event->key(), modifier);
    
    try{
        writer()->writeKeyEvent(k, 0, true);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
    
}

void CConnectVnc::keyReleaseEvent(QKeyEvent *event)
{
    if(m_pPara && m_pPara->GetOnlyView()) return;
    if(!writer()) return;
    bool modifier = false;
    if (event->modifiers() & Qt::ShiftModifier)
        modifier = true;
    //qDebug(log) << "slotKeyReleaseEvent key:" << key << modifiers;
    uint32_t k = TranslateRfbKey(event->key(), modifier);
    
    try{
        writer()->writeKeyEvent(k, 0, false);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
    
}

QString CConnectVnc::ConnectInformation()
{
    const int len = 128;
    char buf[len];
    QString szInfo;
    szInfo = QString("Desktop name: ") + server.name() + "\n";
    szInfo += QString("Size: %1 x %2").arg(server.width()).arg(server.height()) + "\n";

    // TRANSLATORS: Will be filled in with a string describing the
    // protocol pixel format in a fairly language neutral way
    server.pf().print(buf, len);
    szInfo += QString("Pixel format: ") + buf + "\n";
    szInfo += QString("Requested encoding: ") + rfb::encodingName(getPreferredEncoding()) + "\n";
    szInfo += QString("Protocol version: %1.%2").arg(server.majorVersion).arg(server.minorVersion) + "\n";
    szInfo += QString("Security method: ") + rfb::secTypeName(csecurity->getType()) + "\n";
    szInfo += QString("Support local cursor: %1").arg(supportsLocalCursor) + "\n";
    szInfo += QString("Support cursor position: %1").arg(supportsCursorPosition) + "\n";
    szInfo += QString("Support desktop resize: %1").arg(supportsDesktopResize) + "\n";
    szInfo += QString("Support LED state: %1").arg(supportsLEDState) + "\n";
    szInfo += QString("Led state: %1").arg(server.ledState()) + "\n";
    szInfo += QString("Supports QEMU KeyEvent: %1").arg(server.supportsQEMUKeyEvent) + "\n";
    szInfo += QString("Supports Set Desktop Size: %1").arg(server.supportsSetDesktopSize) + "\n";
    szInfo += QString("Support fance: %1").arg(server.supportsFence) + "\n";
    szInfo += QString("Support continue updates: %1").arg(server.supportsContinuousUpdates) + "\n";

    return szInfo;
}

/**
 * @brief CConnectVnc::TranslateRfbKey
 * @param inkey
 * @param modifier
 * @return 
 * @see https://github.com/rfbproto/rfbproto/blob/master/rfbproto.rst#keyevent
 */
quint32 CConnectVnc::TranslateRfbKey(quint32 inkey, bool modifier)
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
        
    case Qt::Key_Hyper_L: k = XK_Hyper_L; break;
    case Qt::Key_Hyper_R: k = XK_Hyper_R; break;

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

void CConnectVnc::slotClipBoardChanged()
{
    //qDebug(log) << "CConnectVnc::slotClipBoardChanged()";
    if(!m_pPara->GetClipboard() || !getOutStream() || !writer()) return;
    QClipboard* pClip = QApplication::clipboard();
    if(pClip->ownsClipboard()) return;
    announceClipboard(true);
}

void CConnectVnc::handleClipboardRequest()
{
    //qDebug(log) << "CConnectVnc::handleClipboardRequest";
    if(!m_pPara->GetClipboard() || !getOutStream() || !writer()) return;
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    /*if (mimeData->hasImage()) {
        //        setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
    } else */if (mimeData->hasText()) {
        QString szText = mimeData->text();
        qDebug(log)
                << "CConnectVnc::handleClipboardRequest:szText:" << szText;
        try{
            sendClipboardData(szText.toStdString().c_str());
        } catch (rdr::Exception& e) {
            qCritical(log) << "sendClipboardData exception" << e.str();
        }
    } /*else if (mimeData->hasHtml()) {
        QString szHtml = mimeData->html();
        qDebug(log)
                << "CConnectVnc::handleClipboardRequest:html:" << szHtml;
        try{
            sendClipboardData(mimeData->html().toStdString().c_str());
        } catch (rdr::Exception& e) {
            qCritical(log) << "sendClipboardData exception" << e.str();
        }
    }*/ else {
        qCritical(log) << "Cannot display data";
    }
}

void CConnectVnc::handleClipboardAnnounce(bool available)
{
    //qDebug(log) << "CConnectVnc::handleClipboardAnnounce";
    if(!m_pPara->GetClipboard() || !getOutStream() || !writer()) return;
    if(available)
        this->requestClipboard();
}

void CConnectVnc::handleClipboardData(const char *data)
{
    //qDebug(log) << "CConnectVnc::handleClipboardData";
    if(!m_pPara->GetClipboard() || !getOutStream() || !writer()) return;

    QMimeData* pData = new QMimeData();
    pData->setText(QString::fromUtf8(data));
    emit sigSetClipboard(pData);
}

void CConnectVnc::authSuccess()
{
    qDebug(log) << "CConnectVnc::authSuccess";
}
