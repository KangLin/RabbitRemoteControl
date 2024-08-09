// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "ChannelSSHTunnelForward.h"

#if defined(Q_OS_WIN)
    #define socklen_t int
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/tcp.h>
#endif

static Q_LOGGING_CATEGORY(log, "Channel.SSH.Tunnel.Forward")

CChannelSSHTunnelForward::CChannelSSHTunnelForward(
    QSharedPointer<CParameterChannelSSH> parameter, QObject *parent)
    : CChannelSSHTunnel{parameter, parent},
    m_Server(SSH_INVALID_SOCKET),
    m_Connector(SSH_INVALID_SOCKET)
{
}

CChannelSSHTunnelForward::~CChannelSSHTunnelForward()
{
    qDebug(log) << "ChannelSSHTunnelForward::~ChannelSSHTunnelForward()";
}

bool CChannelSSHTunnelForward::open(OpenMode mode)
{
    bool bRet = false;
    int family = AF_INET;
    int type = SOCK_STREAM;
    QString szErr;

    bRet = CChannelSSHTunnel::open(mode);
    if(!bRet)
        return false;

    m_Server = socket(family, type, 0);
    if(SSH_INVALID_SOCKET == m_Server) {
        szErr = "Create server socket fail:" + QString::number(errno);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return false;
    }
    
    Channel::CEvent::SetSocketNonBlocking(m_Server);
    Channel::CEvent::EnableNagles(m_Server, false);
    
    do {
        struct sockaddr_in listen_addr;
        socklen_t size = 0;
        memset(&listen_addr, 0, sizeof(listen_addr));
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        listen_addr.sin_port = 0;	/* kernel chooses port.	 */
        if (bind(m_Server, (struct sockaddr *) &listen_addr, sizeof (listen_addr))
            == -1) {
            szErr = "Server bind fail:" + QString::number(errno);
            qCritical(log) << szErr;
            setErrorString(szErr);
            bRet = false;
            break;
        }
        if (listen(m_Server, 1) == -1) {
            szErr = "Server listen fail:" + QString::number(errno);
            qCritical(log) << szErr;
            setErrorString(szErr);
            bRet = false;
            break;
        }
        
        /* We want to find out the port number to connect to.  */
        size = sizeof(listen_addr);
        if (getsockname(m_Server, (struct sockaddr *) &listen_addr, &size) == -1)
        {
            bRet = false;
            break;
        }
        if (size != sizeof (listen_addr))
            break;
        qDebug(log) << "listener in:" << inet_ntoa(listen_addr.sin_addr) << ntohs(listen_addr.sin_port);
        emit sigServer(ntohs(listen_addr.sin_port));
    } while(0);
    
    if(!bRet) {
        CloseSocket(m_Server);
        m_Server = SSH_INVALID_SOCKET;
    }

    return bRet;
}

void CChannelSSHTunnelForward::close()
{
    qDebug(log) << "CChannelSSHTunnelForward::close()";
    if(SSH_INVALID_SOCKET != m_Server)
        CloseSocket(m_Server);
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
    if(m_Server < 0) return 0;
    
    struct sockaddr_in connect_addr;
    memset(&connect_addr, 0, sizeof(connect_addr));
    socklen_t size = sizeof(connect_addr);
    m_Connector = accept(m_Server,  (struct sockaddr *) &connect_addr, &size);
    if(SSH_INVALID_SOCKET == m_Connector)
    {
        /*
        qCritical(log) << "accept fail" << errno << "[" << strerror(errno) << "]"
                       << "m_Server" << m_Server;//*/
        if(EAGAIN == errno || EWOULDBLOCK == errno)
            return 0;
        else {
            QString szErr =  "The server accpet is fail:";
            szErr += QString::number(errno)
                     + " [" + strerror(errno) + "]";
            qCritical(log) << szErr;
            setErrorString(szErr);
            return errno;
        }
    }
    qDebug(log) << "accept from:" << inet_ntoa(connect_addr.sin_addr) << ntohs(connect_addr.sin_port);
    Channel::CEvent::SetSocketNonBlocking(m_Connector);
    Channel::CEvent::EnableNagles(m_Connector, false);
    CloseSocket(m_Server);
    m_Server = SSH_INVALID_SOCKET;
    return 0;
}

int CChannelSSHTunnelForward::ReadConnect()
{
    const int nLen = 1024;
    char buf[nLen];
    int nRet = 0;
    
    if(SSH_INVALID_SOCKET == m_Connector) {
        qDebug(log) << "The connector is invalid";
        return 0;
    }
    //qDebug(log) << "CChannelSSHTunnelForward::ReadConnect()";
    do {
        nRet = recv(m_Connector, buf, nLen, 0);
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
                                + " m_Server:" + QString::number(m_Server);
                qCritical(log) << szErr;
                setErrorString(szErr);
                return errno;
            }
        }
        if(nRet > 0) {
            int n = write(buf, nRet);
            if(n < 0) {
                QString szErr = "write ssh tunnel fail:" + errorString();
                qCritical(log) << szErr;
                setErrorString(szErr);
                return -1;
            }
        }
    } while(nLen == nRet);
    return 0;
}

int CChannelSSHTunnelForward::SSHReadyRead()
{
    const int nLen = 1024;
    char buf[nLen];
    int nRet = 0;
    
    if(SSH_INVALID_SOCKET == m_Connector) {
        //qDebug(log) << "The connector is invalid";
        return 0;
    }
    
    do {
        nRet = read(buf, nLen);
        if(nRet < 0) {
            QString szErr = "read data from ssh tunnel fail:" + errorString();
            qCritical(log) << szErr;
            setErrorString(szErr);
            return nRet;
        }
        if(nRet > 0) {
            int n = send(m_Connector, buf, nRet, 0);
            if(n < 0) {
                //qCritical(log) << "send to socket fail" << errno;
                if(EAGAIN == errno || EWOULDBLOCK == errno)
                    return 0;
                else {
                    QString szErr = "send to socket fail:" + QString::number(errno)
                                    + " - " + strerror(errno);
                    qCritical(log) << szErr;
                    setErrorString(szErr);
                    return errno;
                }
            }
        }
    } while(nLen == nRet);
    return 0;
}

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
        
    struct timeval timeout = {0, 5000000};
    ssh_channel channels[2], channel_out[2];
    channels[0] = m_Channel;
    channels[1] = nullptr;
    
    fd_set set;
    FD_ZERO(&set);
    socket_t fd = SSH_INVALID_SOCKET;
    socket_t fdSSH = ssh_get_fd(m_Session);
    //qDebug(log) << "ssh_select:" << m_Server << m_Connector;
    if(SSH_INVALID_SOCKET != m_Server) {
        //qDebug(log) << "server listen";
        FD_SET(m_Server, &set);
        fd = m_Server;
        nRet = select(fd + 1, &set, NULL, NULL, &timeout);
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

    if(nRet < 0) {
        QString szErr;
        szErr = "ssh_channel_select failed: " + QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return nRet;
    }
    
    //qDebug(log) << "recv socket" << m_Server << m_Connector;
    if(SSH_INVALID_SOCKET != m_Server && FD_ISSET(m_Server, &set)) {
        nRet = AcceptConnect();
        return nRet;
    } else if(SSH_INVALID_SOCKET != m_Connector && FD_ISSET(m_Connector, &set)) {
        nRet = ReadConnect();
        if(nRet) return nRet;
    }

    if(!channels[0]){
        qDebug(log) << "There is not channel";
        return 0;
    }
    
    if(ssh_channel_is_eof(m_Channel)) {
        qWarning(log) << "Channel is eof";
        setErrorString(tr("The channel is eof"));
        return -1;
    }
    
    nRet = ssh_channel_poll(m_Channel, 0);
    //qDebug(log) << "ssh_channel_poll:" << nRet;
    if(nRet < 0) {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return nRet;
    }
    if(nRet == 0) {
        //qDebug(log) << "There is not data in channel";
        return 0;
    }
    
    return SSHReadyRead();
}
