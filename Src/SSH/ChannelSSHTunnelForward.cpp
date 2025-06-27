// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QStandardPaths>
#include <QThread>
#include <QDir>

#if defined(Q_OS_WIN)
    #if defined(HAVE_UNIX_DOMAIN_SOCKET)
        #include <WinSock2.h>
        /* [AF_UNIX comes to Windows](https://devblogs.microsoft.com/commandline/af_unix-comes-to-windows/)
         * How can I write a Windows AF_UNIX app?
         *   - Download the Windows Insiders SDK for the Windows build 17061 — available here.
         *   - Check whether your Windows build has support for unix socket by running “sc query afunix” from a Windows admin command prompt.
         *   - #include <afunix.h> in your Windows application and write a Windows unix socket winsock application as you would write any other unix socket application, but, using Winsock API’s.
         */
        #include <afunix.h>
    #endif
    #define socklen_t int
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/tcp.h>
    #if defined(HAVE_UNIX_DOMAIN_SOCKET)
        #include <sys/un.h>
    #endif
#endif

#include "ChannelSSHTunnelForward.h"

static Q_LOGGING_CATEGORY(log, "SSH.Tunnel.Forward")

CChannelSSHTunnelForward::CChannelSSHTunnelForward(
    CParameterSSHTunnel *parameter, CParameterNet *remote, CBackend *pBackend, QObject *parent)
    : CChannelSSHTunnel{parameter, remote, pBackend, false, parent},
    m_Listen(SSH_INVALID_SOCKET),
    m_Connector(SSH_INVALID_SOCKET),
    m_pBuffer(nullptr)
{
    qDebug(log) << "ChannelSSHTunnelForward::ChannelSSHTunnelForward()";
    m_pBuffer = new char[m_BufferLength];
}

CChannelSSHTunnelForward::~CChannelSSHTunnelForward()
{
    qDebug(log) << "ChannelSSHTunnelForward::~ChannelSSHTunnelForward()";
    if(m_pBuffer)
        delete []m_pBuffer;
}

int CChannelSSHTunnelForward::OpenSocket()
{
    int nRet = 0;
    bool bRet = false;
    int family = AF_INET;
    int type = SOCK_STREAM;
    QString szErr;
    
    socklen_t size = 0;
    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    listen_addr.sin_port = 0;	/* kernel chooses port.	 */
    
    m_Listen = socket(family, type, 0);
    if(SSH_INVALID_SOCKET == m_Listen) {
        szErr = "Create socket fail:" + QString::number(errno);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return false;
    }
    
    Channel::CEvent::SetSocketNonBlocking(m_Listen);
    //Channel::CEvent::EnableNagles(m_Server, false);
    
    do {
        if (bind(m_Listen, (struct sockaddr *) &listen_addr, sizeof(listen_addr))
            == -1) {
            szErr = "bind fail:" + QString(inet_ntoa(listen_addr.sin_addr))
                    + QString(":") + QString::number(ntohs(listen_addr.sin_port))
                    + " - " + QString::number(errno);
            qCritical(log) << szErr;
            setErrorString(szErr);
            bRet = false;
            break;
        }
        if (listen(m_Listen, 1) == -1) {
            szErr = "listen fail:" + QString(inet_ntoa(listen_addr.sin_addr))
                    + QString(":") + QString::number(ntohs(listen_addr.sin_port))
                    + " - " + QString::number(errno);
            qCritical(log) << szErr;
            setErrorString(szErr);
            bRet = false;
            break;
        }
        /* We want to find out the port number to connect to.  */
        size = sizeof(listen_addr);
        if (getsockname(m_Listen, (struct sockaddr *) &listen_addr, &size) == -1)
        {
            bRet = false;
            break;
        }
        if (size != sizeof (listen_addr))
            break;
        qDebug(log) << "listener in:"
                    << inet_ntoa(listen_addr.sin_addr)
                           + QString(":") + QString::number(ntohs(listen_addr.sin_port));
        emit sigServer(inet_ntoa(listen_addr.sin_addr), ntohs(listen_addr.sin_port));
        return 0;
    } while(0);

    if(!bRet) {
        CloseSocket(m_Listen);
        m_Listen = SSH_INVALID_SOCKET;
    }

    return -1;
}

#if defined(HAVE_UNIX_DOMAIN_SOCKET)
int CChannelSSHTunnelForward::OpenUnixSocket()
{
    bool bRet = false;
    int family = AF_UNIX;
    int type = SOCK_STREAM;
    QString szErr;

    socklen_t size = 0;
    struct sockaddr_un listen_addr;
    QString szPath;
    QString szUnixDomainSocket;
    memset(&listen_addr, 0, sizeof(listen_addr));
    szPath = QStandardPaths::writableLocation(
                 QStandardPaths::TempLocation)
             + QDir::separator() + "RabbitRemoteControl";
    auto now = std::chrono::system_clock::now();
    auto tick = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    szUnixDomainSocket = szPath + QDir::separator() + "RRC_" + QString::number(tick) + "_"
                         + QString::number((unsigned long)QThread::currentThreadId()) + ".socket";
    QDir p(szPath);
    if(!p.exists())
        p.mkpath(szPath);
    QDir f(szUnixDomainSocket);
    if(f.exists())
        f.remove(szUnixDomainSocket);
    size = sizeof(sockaddr_un::sun_path);
    if(szUnixDomainSocket.size() > size)
    {
        qCritical(log) << "The unix domain socket length greater then" << size;
        return -2;
    }
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sun_family = AF_UNIX;
    strcpy(listen_addr.sun_path, szUnixDomainSocket.toStdString().c_str());

    m_Listen = socket(family, type, 0);
    if(SSH_INVALID_SOCKET == m_Listen) {
        szErr = "Create socket fail:" + QString::number(errno);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return false;
    }

    Channel::CEvent::SetSocketNonBlocking(m_Listen);
    //Channel::CEvent::EnableNagles(m_Server, false);

    do {
        if (bind(m_Listen, (struct sockaddr *) &listen_addr, sizeof(listen_addr))
            == -1) {
            szErr = "bind fail:" + QString(listen_addr.sun_path)
                    + " - " + QString::number(errno);
            qCritical(log) << szErr;
            setErrorString(szErr);
            bRet = false;
            break;
        }
        if (listen(m_Listen, 1) == -1) {
            szErr = "listen fail:" + QString(listen_addr.sun_path)
                    + " - " + QString::number(errno);
            qCritical(log) << szErr;
            setErrorString(szErr);
            bRet = false;
            break;
        }

        emit sigServer(szUnixDomainSocket);
        qDebug(log) << "listener in:" << listen_addr.sun_path;
        return 0;
    } while(0);

    if(!bRet) {
        CloseSocket(m_Listen);
        m_Listen = SSH_INVALID_SOCKET;
    }

    return -1;
}
#endif

bool CChannelSSHTunnelForward::open(OpenMode mode)
{
    bool bRet = false;

    bRet = CChannelSSHTunnel::open(mode);
    if(!bRet)
        return false;

    int nRet = OpenSocket();
    if(nRet) return false;

    return bRet;
}

void CChannelSSHTunnelForward::close()
{
    qDebug(log) << "CChannelSSHTunnelForward::close()";
    if(SSH_INVALID_SOCKET != m_Listen)
        CloseSocket(m_Listen);
    if(SSH_INVALID_SOCKET != m_Connector)
        CloseSocket(m_Connector);
    CChannelSSHTunnel::close();
}

int CChannelSSHTunnelForward::CloseSocket(socket_t &s)
{
    int nRet = 0;
    if(SSH_INVALID_SOCKET == s)
        return 0;
#if defined(Q_OS_WIN)
    nRet = ::closesocket(s);
#else
    nRet = ::close(s);
#endif
    if(nRet)
        qCritical(log) << "Close socket fail" << errno << ":" << strerror(errno);
    s = SSH_INVALID_SOCKET;
    return nRet;
}

int CChannelSSHTunnelForward::AcceptConnect()
{
    //qDebug(log) << "CChannelSSHTunnelForward::AcceptConnect()";
    if(SSH_INVALID_SOCKET == m_Listen) return 0;
#if defined(HAVE_UNIX_DOMAIN_SOCKET)
    struct sockaddr_un connect_addr;
#else
    struct sockaddr_in connect_addr;
#endif
    memset(&connect_addr, 0, sizeof(connect_addr));
    socklen_t size = sizeof(connect_addr);
    m_Connector = accept(m_Listen, (struct sockaddr *)&connect_addr, &size);
    if(SSH_INVALID_SOCKET == m_Connector)
    {
        /*
        qCritical(log) << "accept fail" << errno << "[" << strerror(errno) << "]"
                       << "m_Server" << m_Server;//*/
        if(EAGAIN == errno || EWOULDBLOCK == errno)
            return 0;
        else {
            QString szErr =  "The server accept is fail:";
            szErr += QString::number(errno) + " [" + strerror(errno) + "]";
            qCritical(log) << szErr;
            setErrorString(szErr);
            return errno;
        }
    }
    qDebug(log) << "accept connector fd:" << m_Connector;
    Channel::CEvent::SetSocketNonBlocking(m_Connector);
    //Channel::CEvent::EnableNagles(m_Connector, false);
    CloseSocket(m_Listen);
    m_Listen = SSH_INVALID_SOCKET;
    return 0;
}

int CChannelSSHTunnelForward::ReadConnect()
{
    int nRet = 0;
    
    if(SSH_INVALID_SOCKET == m_Connector) {
        qDebug(log) << "The connector is invalid";
        return 0;
    }

    Q_ASSERT(m_pBuffer);
    if(!m_pBuffer)
        return -1;
    
    //qDebug(log) << "CChannelSSHTunnelForward::ReadConnect()";
    do {
        nRet = recv(m_Connector, m_pBuffer, m_BufferLength, 0);
        if(nRet < 0)
        {
            /*
            qCritical(log) << "read data from socket fail" << errno<< "[" << strerror(errno) << "]"
                << "m_Server" << m_Server;//*/
            if(EAGAIN == errno || EWOULDBLOCK == errno)
                return 0;
            else {
                QString szErr = "read data from socket fail:"
                                + QString::number(errno) + " - " + strerror(errno)
                                + " m_Server:" + QString::number(m_Listen);
                qCritical(log) << szErr;
                setErrorString(szErr);
                return errno;
            }
        }
        if(nRet > 0) {
            int n = write(m_pBuffer, nRet);
            if(n < 0) {
                QString szErr = "write ssh tunnel fail:" + errorString();
                qCritical(log) << szErr;
                setErrorString(szErr);
                return -1;
            }
            // TODO: 检查发送的数据。因为系统 socket 有发送缓存，所以此情况一般不会出现，只是提醒。
            if(n < nRet)
                qWarning(log) << "The send data" << n << "<" << nRet;
        }
    } while(m_BufferLength == nRet);
    return 0;
}

int CChannelSSHTunnelForward::SSHReadyRead()
{
    int nRet = 0;

    if(SSH_INVALID_SOCKET == m_Connector) {
        //qDebug(log) << "The connector is invalid";
        return 0;
    }
    
    Q_ASSERT(m_pBuffer);
    if(!m_pBuffer)
        return -1;
    
    do {
        nRet = read(m_pBuffer, m_BufferLength);
        if(nRet < 0) {
            QString szErr = "read data from ssh tunnel fail:" + errorString();
            qCritical(log) << szErr;
            setErrorString(szErr);
            return nRet;
        }
        if(nRet > 0) {
            int n = send(m_Connector, m_pBuffer, nRet, 0);
            if(n < 0) {
                //qCritical(log) << "send to socket fail" << errno;
                if(EAGAIN == errno || EWOULDBLOCK == errno) {
                    // TODO: add resend
                    return 0;
                }
                else {
                    QString szErr = "send to socket fail, connector fd:" + QString::number(m_Connector)
                                    + " - [" + QString::number(errno) + "] - " + strerror(errno);
                    qCritical(log) << szErr;
                    setErrorString(szErr);
                    return errno;
                }
            }
        }
    } while(m_BufferLength == nRet);
    return 0;
}

/*!
 * \return
 *   \li >= 0: continue, Interval call time (msec)
 *   \li = -1: stop
 *   \li < -1: error
 */
int CChannelSSHTunnelForward::Process()
{
    int nRet = 0;

    if(!m_Session || !m_Channel || !ssh_channel_is_open(m_Channel)
        || ssh_channel_is_eof(m_Channel)) {
        QString szErr = "The channel is not open";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }

    struct timeval timeout = {0, 50000};
    ssh_channel channels[2], channel_out[2];
    channels[0] = m_Channel;
    channels[1] = nullptr;
    channel_out[0] = channel_out[1] = nullptr;

    fd_set set;
    FD_ZERO(&set);
    socket_t fd = SSH_INVALID_SOCKET;
    socket_t fdSSH = ssh_get_fd(m_Session);
    //qDebug(log) << "ssh_select:" << m_Server << m_Connector;
    if(SSH_INVALID_SOCKET != m_Listen) {
        //qDebug(log) << "server listen";
        FD_SET(m_Listen, &set);
        fd = m_Listen;
        //nRet = select(fd + 1, &set, NULL, NULL, &timeout);
        nRet = ssh_select(channels, channel_out, fd + 1, &set, &timeout);
    } else if(SSH_INVALID_SOCKET != m_Connector) {
        //qDebug(log) << "recv connect";
        FD_SET(m_Connector, &set);
        fd = m_Connector;
        if(SSH_INVALID_SOCKET != fdSSH)
        {
            FD_SET(fdSSH, &set);
            fd = std::max(fd, fdSSH);
        }
        nRet = ssh_select(channels, channel_out, fd + 1, &set, &timeout);
    }
    //qDebug(log) << "ssh_select end:" << nRet;
    if(EINTR == nRet)
        return 0;
    if(SSH_OK != nRet) {
        QString szErr;
        szErr = "ssh_channel_select failed: " + QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return nRet;
    }

    //qDebug(log) << "recv socket" << m_Server << m_Connector;
    if(SSH_INVALID_SOCKET != m_Listen && FD_ISSET(m_Listen, &set)) {
        nRet = AcceptConnect();
        return nRet;
    } else if(SSH_INVALID_SOCKET != m_Connector && FD_ISSET(m_Connector, &set)) {
        nRet = ReadConnect();
        if(nRet) return nRet;
    }

    if(!channel_out[0]) {
        //qDebug(log) << "The channel is not select";
        return 0;
    }

    if(ssh_channel_is_eof(m_Channel)) {
        qWarning(log) << "Channel is eof";
        setErrorString(tr("The channel is eof"));
        // Stop
        return -1;
    }

    // Get channel data length
    nRet = ssh_channel_poll(m_Channel, 0);
    //qDebug(log) << "Get channel data length:" << nRet;
    if(SSH_ERROR == nRet)
    {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        return -6;
    } else if(SSH_EOF == nRet) {
        // Stop
        return -1;
    } else if(0 > nRet) {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        // Error
        return -7;
    } else if(0 == nRet) {
        //qDebug(log) << "The channel has not data";
        return 0;
    }

    return SSHReadyRead();
}
