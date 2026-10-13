// Author: Kang Lin <kl222@126.com>

#include "BackendVnc.h"
#include "OperateDesktop.h"

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

#include "FramePixelBuffer.h"

#include <QDir>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QLoggingCategory>
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

#include "ConvertKeyCode.h"

static Q_LOGGING_CATEGORY(log, "VNC.Backend")
static Q_LOGGING_CATEGORY(logVNC, "VNC.Log")
static Q_LOGGING_CATEGORY(logKey, "VNC.Backend.Key")
static Q_LOGGING_CATEGORY(logInputMethod, "VNC.Backend.InputMethod")
static Q_LOGGING_CATEGORY(logMouse, "VNC.Backend.Mouse")

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

CBackendVnc::CBackendVnc(COperateDesktop *pOperate)
    : CBackendDesktop(pOperate)
    , m_pOperate(pOperate)
    , m_pPara(nullptr)
    , m_SecurityLevel(CSecurityLevel::Level::Risk)
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
    m_pPara = dynamic_cast<CParameterVnc*>(pOperate->GetParameter());
    Q_ASSERT(m_pPara);

    if(!m_pPara->GetLocalCursor())
    {
        emit sigUpdateCursor(QCursor(Qt::BlankCursor));
    }
}

CBackendVnc::~CBackendVnc()
{
    qDebug(log) << "CConnectVnc::~CConnectVnc()";
}

int CBackendVnc::SetPara()
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
 * \li OnStartReturnValue::Fail: error
 * \li OnStartReturnValue::UseOnProcess: Use OnProcess (non-Qt event loop)
 * \li OnStartReturnValue::NotUseOnProcess: Don't use OnProcess (qt event loop)
 */
CBackendVnc::OnStartReturnValue CBackendVnc::OnStart()
{
    qDebug(log) << "CConnectVnc::OnStart()";
    int nRet = 0;
    
    if(m_pPara->GetIce())
        nRet = IceInit();
    else {
#ifdef HAVE_LIBSSH
        if(m_pPara->m_Proxy.GetUsedType() == CParameterProxy::TYPE::SSHTunnel) {
            nRet = SSHInit();
            if(nRet) return OnStartReturnValue::Fail; // error
            return OnStartReturnValue::UseOnProcess;
        }
#endif
        nRet = SocketInit();
    }
    if(nRet) return OnStartReturnValue::Fail; // error
    // Don't use OnProcess (qt event loop)
    return OnStartReturnValue::NotUseOnProcess;
}

int CBackendVnc::IceInit()
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

int CBackendVnc::SocketInit()
{
    int nRet = 0;
    try{
        QTcpSocket* pSock = new QTcpSocket(this);
        if(!pSock)
            return -1;
        m_DataChannel = QSharedPointer<CChannel>(new CChannel(pSock));
        if(!m_DataChannel) {
            qCritical(log) << "New CChannel fail";
            return -1;
        }

        SetChannelConnect(m_DataChannel);

        if(!m_DataChannel->open(QIODevice::ReadWrite))
        {
            qCritical(log) << "Open channel fail";
            return -1;
        }

        QNetworkProxy::ProxyType type = QNetworkProxy::NoProxy;
        // Set sock
        switch(m_pPara->m_Proxy.GetUsedType())
        {
        case CParameterProxy::TYPE::SockesV5:
            type = QNetworkProxy::Socks5Proxy;
            break;
        case CParameterProxy::TYPE::System:
            type = QNetworkProxy::DefaultProxy;
            break;
        case CParameterProxy::TYPE::Http:
            type = QNetworkProxy::HttpProxy;
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
        case QNetworkProxy::HttpProxy:
        case QNetworkProxy::Socks5Proxy:
        {
            QNetworkProxy proxy;
            auto net = &m_pPara->m_Proxy.m_SockesV5;
            if(QNetworkProxy::HttpProxy == type) {
                net = &m_pPara->m_Proxy.m_Http;
            }
            if(net->GetHost().isEmpty())
            {
                QString szErr;
                szErr = tr("The proxy server is empty, please input it");
                qCritical(log) << szErr;
                emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
                return -1;
            }
            proxy.setHostName(net->GetHost());
            proxy.setPort(net->GetPort());
            auto &user = net->m_User;
            if(((user.GetUsedType() == CParameterUser::TYPE::UserPassword)
                 && (user.GetPassword().isEmpty() || user.GetUser().isEmpty()))
                || ((user.GetUsedType() == CParameterUser::TYPE::PublicKey)
                    && user.GetPassphrase().isEmpty())) {
                int nRet = QDialog::Rejected;
                emit sigBlockShowWidget("CDlgUserPassword", nRet, net);
                if(QDialog::Accepted != nRet)
                {
                    return -1;
                }
            }
            proxy.setUser(user.GetUser());
            proxy.setPassword(user.GetPassword());
            proxy.setType(type);
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
            return -1;
        }
        pSock->connectToHost(m_pPara->m_Net.GetHost(), m_pPara->m_Net.GetPort());
        
        QString serverHost;
        serverHost = pSock->peerName();
        qDebug(log) << "Server Host:" << serverHost;
        setServerName(serverHost.toStdString().c_str());
        
        return nRet;
    } catch (rdr::Exception& e) {
        qCritical(log) << "SocketInit exception:" << e.str();
        emit sigError(-1, e.str());
        nRet = -1;
    }
    return nRet;
}

int CBackendVnc::SSHInit()
{
#ifdef HAVE_LIBSSH
    auto channel = QSharedPointer<CChannelSSHTunnel>(
        new CChannelSSHTunnel(&m_pPara->m_Proxy.m_SSH, &m_pPara->m_Net, this));
    if(!channel) {
        qCritical(log) << "New CChannelSSHTunnel fail";
        return -1;
    }
    m_DataChannel = channel;
    SetChannelConnect(m_DataChannel);
    if(!channel->open(QIODevice::ReadWrite))
    {
        QString szErr;
        szErr = tr("Failed to open SSH tunnel:");
        szErr += "(" + m_pPara->m_Proxy.m_SSH.m_Net.GetHost();
        szErr += ":";
        szErr += QString::number(m_pPara->m_Proxy.m_SSH.m_Net.GetPort());
        szErr += " - ";
        szErr += m_pPara->m_Net.GetHost();
        szErr += ":";
        szErr += QString::number(m_pPara->m_Net.GetPort()) + ")";
        QString szMsg = szErr + "\n" + channel->errorString();
        emit sigShowMessageBox(tr("Error"), szMsg, QMessageBox::Critical);
        return -2;
    }
    m_SecurityLevel = CSecurityLevel::Level::Proxy;
#endif
    return 0;
}

int CBackendVnc::SetChannelConnect(QSharedPointer<CChannel> channel)
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
    if(m_pOperate && m_pOperate->GetStats()) {
        check = connect(channel.data(), SIGNAL(sigReceive(quint64)),
                        m_pOperate->GetStats(), SLOT(AddReceives(quint64)));
        Q_ASSERT(check);
        check = connect(channel.data(), SIGNAL(sigSend(quint64)),
                        m_pOperate->GetStats(), SLOT(AddSends(quint64)));
        Q_ASSERT(check);
    }
    return 0;
}

int CBackendVnc::OnStop()
{
    qDebug(log) << "CConnectVnc::OnStop()";
    close();
    setStreams(nullptr, nullptr);
    if(m_DataChannel) {
        m_DataChannel->close();
    }
    emit sigFinished();
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
int CBackendVnc::OnProcess()
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

int CBackendVnc::WakeUp()
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

void CBackendVnc::slotConnected()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pPara->GetIce())
        qInfo(log) << "Connected to peer " + m_pPara->GetPeerUser();
    else {
        auto &net = m_pPara->m_Net;
        QString szInfo = "Connected to "
                         + net.GetHost() + ":" + QString::number(net.GetPort());
#ifdef HAVE_LIBSSH
        if(CParameterProxy::TYPE::SSHTunnel == m_pPara->m_Proxy.GetUsedType())
        {
            auto &sshNet = m_pPara->m_Proxy.m_SSH.m_Net;
            szInfo += " with ssh turnnel: " + sshNet.GetHost()
                      + ":" + QString::number(sshNet.GetPort());
        }
#endif
        qInfo(log) << szInfo;
    }
    int nRet = SetPara();
    if(nRet)
    {
        emit sigStop();
        return;
    }
    m_InStream = QSharedPointer<rdr::InStream>(new CInStreamChannel(m_DataChannel.data()));
    m_OutStream = QSharedPointer<rdr::OutStream>(new COutStreamChannel(m_DataChannel.data()));
    if(!(m_InStream && m_OutStream))
    {
        qCritical(log) << "m_InStream or m_OutStream is null";
        emit sigStop();
        return;
    }
    setStreams(m_InStream.data(), m_OutStream.data());
    initialiseProtocol();
}

void CBackendVnc::slotDisConnected()
{
    QString szInfo;
    szInfo = "CConnectVnc::slotDisConnected() from "
             + m_pPara->m_Net.GetHost() + ":"
             + QString::number(m_pPara->m_Net.GetPort());
#ifdef HAVE_LIBSSH
    if(CParameterProxy::TYPE::SSHTunnel == m_pPara->m_Proxy.GetUsedType())
    {
        auto &net = m_pPara->m_Proxy.m_SSH.m_Net;
        szInfo += " with ssh turnnel: " + net.GetHost()
                  + ":" + QString::number(net.GetPort());
    }
#endif
    qInfo(log) << szInfo;
}

void CBackendVnc::slotReadyRead()
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
    emit sigStop();
}

void CBackendVnc::slotChannelError(int nErr, const QString& szErr)
{
    qDebug(log) << "Channel error:" << nErr << "-" << szErr;
    emit sigError(nErr, szErr);
    emit sigStop();
}

// initDone() is called when the serverInit message has been received.  At
// this point we create the desktop window and display it.  We also tell the
// server the pixel format and encodings to use and request the first update.
void CBackendVnc::initDone()
{
    Q_ASSERT(m_pPara);
    qDebug(log, "initDone: %s", ConnectInformation().toStdString().c_str());

    emit sigSetDesktopSize(server.width(), server.height());
    QString szName = QString::fromUtf8(server.name());
    emit sigServerName(szName);

    //Set viewer frame buffer
    setFramebuffer(new CFramePixelBuffer(server.width(), server.height()));

    // Force a switch to the format and encoding we'd like
    // Set Preferred Encoding
    setPreferredEncoding(m_pPara->GetPreferredEncoding());
    updatePixelFormat();

    emit sigSecurityLevel(m_SecurityLevel);
    emit sigRunning();
}

void CBackendVnc::resizeFramebuffer()
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

void CBackendVnc::setColourMapEntries(int firstColour, int nColours, uint16_t *rgbs)
{
    qCritical(log) << "Invalid SetColourMapEntries from server!";
}

void CBackendVnc::bell()
{
    qApp->beep();
}

void CBackendVnc::setLEDState(unsigned int state)
{
    qDebug(log) << "CConnectVnc::setLEDState" << state;
    CConnection::setLEDState(state);
    emit sigUpdateLedState(state);
}

void CBackendVnc::setCursor(int width, int height, const rfb::Point &hotspot, const uint8_t *data)
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

void CBackendVnc::setCursorPos(const rfb::Point &pos)
{
    //qDebug(log) << "CConnectVnc::setCursorPos x:" << pos.x << ";y:" << pos.y;
    emit sigUpdateCursorPosition(QPoint(pos.x, pos.y));
}

void CBackendVnc::fence(uint32_t flags, unsigned int len, const uint8_t data[])
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

void CBackendVnc::getUserPasswd(bool secure, std::string *user, std::string *password)
{
    if(password)
    {
        auto &user = m_pPara->m_Net.m_User;
        *password = user.GetPassword().toStdString();
        if(user.GetPassword().isEmpty())
        {
            int nRet = QDialog::Rejected;
            auto *net = &m_pPara->m_Net;
            emit sigBlockShowWidget("CDlgUserPassword", nRet, net);
            if(QDialog::Accepted == nRet)
            {
                *password = user.GetPassword().toStdString();
            }
        }
        if(!password->empty())
            m_SecurityLevel |= CSecurityLevel::Level::Authentication;
    }
}

int CBackendVnc::getX509File(std::string *ca, std::string *crl)
{
    auto &user = m_pPara->m_Net.m_User;
    if(ca)
        *ca = user.GetCAFile().toStdString().c_str();
    if(crl)
        *crl = user.GetCRLFile().toStdString().c_str();
    return 0;
}

bool CBackendVnc::showMsgBox(rfb::MsgBoxFlags flags, const char *title, const char *text)
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
void CBackendVnc::setName(const char *name)
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
void CBackendVnc::framebufferUpdateStart()
{
    //qDebug(log) << "CConnectVnc::framebufferUpdateStart()";
    CConnection::framebufferUpdateStart();
}

// framebufferUpdateEnd() is called at the end of an update.
// For each rectangle, the FdInStream will have timed the speed
// of the connection, allowing us to select format and encoding
// appropriately, and then request another incremental update.
void CBackendVnc::framebufferUpdateEnd()
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
void CBackendVnc::updatePixelFormat()
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

void CBackendVnc::mousePressEvent(QMouseEvent *event)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    
    uint8_t mask = 0;
    if(event->button() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(event->button() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(event->button() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    if(event->button() & Qt::MouseButton::BackButton)
        mask |= 0x80;

    QPoint pos = event->pos();
    rfb::Point p(pos.x(), pos.y());

    //qDebug(logMouse) << Q_FUNC_INFO << event->buttons() << event->button() << pos << mask;

    try{
        writer()->writePointerEvent(p, mask);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

void CBackendVnc::mouseReleaseEvent(QMouseEvent *event)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    uint8_t mask = 0;
    QPoint pos = event->pos();
    rfb::Point p(pos.x(), pos.y());
    //qDebug(logMouse) << Q_FUNC_INFO << event->buttons() << event->button() << pos << mask;
    try{
        writer()->writePointerEvent(p, mask);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

void CBackendVnc::mouseMoveEvent(QMouseEvent *event)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;

    QPoint pos = event->pos();
    rfb::Point p(pos.x(), pos.y());
    uint8_t mask = 0;
    if(event->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(event->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(event->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    if(event->buttons() & Qt::MouseButton::BackButton)
        mask |= 0x80;

    //qDebug(logMouse) << Q_FUNC_INFO << event->buttons() << event->button() << pos << mask;

    try{
        writer()->writePointerEvent(p, mask);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

// https://github.com/rfbproto/rfbproto/blob/master/rfbproto.rst#pointerevent
void CBackendVnc::wheelEvent(QWheelEvent *event)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    uint8_t mask = 0;
    if(event->buttons() & Qt::MouseButton::LeftButton)
        mask |= 0x1;
    if(event->buttons() & Qt::MouseButton::MiddleButton)
        mask |= 0x2;
    if(event->buttons() & Qt::MouseButton::RightButton)
        mask |= 0x4;
    if(event->buttons() & Qt::MouseButton::BackButton)
        mask |= 0x80;

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
    /*
    qDebug(logMouse) << Q_FUNC_INFO << event->buttons()
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
                     << event->button()
#endif
                     << pos << mask; //*/

    try{
        writer()->writePointerEvent(p, mask);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

void CBackendVnc::keyPressEvent(QKeyEvent *event)
{
    if(!writer()) return;
    if(m_pPara && m_pPara->GetOnlyView()) return;
    bool modifier = false;
    if (event->modifiers() & Qt::ShiftModifier)
        modifier = true;

    uint32_t k = CConvertKeyCode::QtToRfbKey(event->key(), modifier);
    //qDebug(logKey) << Q_FUNC_INFO << event << k << modifier;

    try{
        writer()->writeKeyEvent(k, 0, true);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

void CBackendVnc::keyReleaseEvent(QKeyEvent *event)
{
    if(m_pPara && m_pPara->GetOnlyView()) return;
    if(!writer()) return;
    bool modifier = false;
    if (event->modifiers() & Qt::ShiftModifier)
        modifier = true;

    uint32_t k = CConvertKeyCode::QtToRfbKey(event->key(), modifier);
    //qDebug(logKey) << Q_FUNC_INFO << event << k << modifier;

    try{
        writer()->writeKeyEvent(k, 0, false);
    } catch (rdr::Exception& e) {
        emit sigError(-1, e.str());
    }
}

void CBackendVnc::InputMethodEvent(QInputMethodEvent *event)
{
    //qDebug(logInputMethod) << Q_FUNC_INFO << event;
    if (m_pPara && m_pPara->GetOnlyView()) return;
    if (!writer()) return;

    QString szText = event->commitString();
    if (szText.isEmpty())
        return;

    //qDebug(logInputMethod) << Q_FUNC_INFO << szText;

    QVector<uint32_t> ucs4 = szText.toUcs4();
    foreach (uint32_t cp, ucs4) {
        quint32 keysym = static_cast<quint32>(cp); // 直接用 Unicode code point 作为 keysym
        try {
            writer()->writeKeyEvent(keysym, 0, true);
            writer()->writeKeyEvent(keysym, 0, false);
        } catch (rdr::Exception &e) {
            emit sigError(-1, e.str());
        }
    }
}

QString CBackendVnc::ConnectInformation()
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

void CBackendVnc::slotClipBoardChanged()
{
    //qDebug(log) << "CConnectVnc::slotClipBoardChanged()";
    if(!m_pPara->GetClipboard() || !getOutStream() || !writer()) return;
    QClipboard* pClip = QApplication::clipboard();
    if(pClip->ownsClipboard()) return;
    announceClipboard(true);
}

void CBackendVnc::handleClipboardRequest()
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

void CBackendVnc::handleClipboardAnnounce(bool available)
{
    //qDebug(log) << "CConnectVnc::handleClipboardAnnounce";
    if(!m_pPara->GetClipboard() || !getOutStream() || !writer()) return;
    if(available)
        this->requestClipboard();
}

void CBackendVnc::handleClipboardData(const char *data)
{
    //qDebug(log) << "CConnectVnc::handleClipboardData";
    if(!m_pPara->GetClipboard() || !getOutStream() || !writer()) return;

    QMimeData* pData = new QMimeData();
    pData->setText(QString::fromUtf8(data));
    emit sigSetClipboard(pData);
}

void CBackendVnc::authSuccess()
{
    qDebug(log) << "CConnectVnc::authSuccess";
}
