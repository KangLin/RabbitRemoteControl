#include <QLoggingCategory>

#if defined(Q_OS_WIN)
    #define close closesocket
    #define socklen_t int
#else
    #if HAVE_EVENTFD
        #include <sys/eventfd.h>
    #endif
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <ifaddrs.h>

    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

#include "Event.h"

static Q_LOGGING_CATEGORY(log, "Channel.Event")
    
namespace Channel {

CEvent::CEvent(QObject *parent)
    : QObject{parent}
{
    qDebug(log) << "CEvent::CEvent";
    fd[0] = INVALID_SOCKET;
    fd[1] = INVALID_SOCKET;
    Init();
}

CEvent::~CEvent()
{
    qDebug(log) << "Event::~Event()";
    Clear();
}

int CEvent::Init()
{
#if HAVE_EVENTFD
    fd[0] = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC | EFD_SEMAPHORE);
#else
    return CreateSocketPair(fd);
#endif
    return 0;
}

int CEvent::Clear()
{
    if(INVALID_SOCKET != fd[0]) {
        close(fd[0]);
        fd[0] = INVALID_SOCKET;
    }

    if(INVALID_SOCKET != fd[1]) {
        close(fd[1]);
        fd[1] = INVALID_SOCKET;
    }
    return 0;
}

int CEvent::Reset()
{
    int nRet = 0;
    if(INVALID_SOCKET == fd[0])
        return -1;

    //qDebug(log) << "Reset()";
#if HAVE_EVENTFD
    eventfd_t value;
    nRet = eventfd_read(fd[0], &value);
    //qDebug(log) << "read eventfd:" << value;
    if(nRet) {
        if(EAGAIN == errno || EWOULDBLOCK == errno)
            return 0;
        qDebug(log) << "eventfd_read fail" << errno << "-" << strerror(errno);
    }
#else
    const int len = 1;
    char buf[len];
    nRet = recv(fd[0], buf, len, 0);
    if(nRet >= 0)
        return 0;
    if(nRet < 0) {
        if(EAGAIN == errno || EWOULDBLOCK == errno)
            return 0;
        qDebug(log) << "recv fail. nRet:" << nRet << "fd:" << fd[0]
                       << "error:" << errno << "-" << strerror(errno);
    }
#endif
    return nRet;
}

int CEvent::WakeUp()
{
    int nRet = 0;
    //qDebug(log) << "WakeUp()";
#if HAVE_EVENTFD
    if(INVALID_SOCKET == fd[0])
        return -1;
    nRet = eventfd_write(fd[0], 1);
    if(nRet) {
        if(EAGAIN == errno || EWOULDBLOCK == errno)
            return 0;
        qDebug(log) << "eventfd_write fail" << errno << "-" << strerror(errno);
    }
#else
    if(INVALID_SOCKET == fd[1])
        return -1;
    const int len = 1;
    char buf[len];
    nRet = send(fd[1], buf, len, 0);
    if(nRet >= 0) {
        //qDebug(log) << "send" << nRet;
        return 0;
    }
    if(nRet < 0) {
        if(EAGAIN == errno || EWOULDBLOCK == errno)
            return 0;
        qDebug(log) << "send fail" << "fd:" << fd[1]
                       << "error:" << errno << "-" << strerror(errno);
    }
#endif
    return nRet;
}

SOCKET CEvent::GetFd()
{
    return fd[0];
}

int CEvent::CreateSocketPair(SOCKET fd[])
{
    int family = AF_INET;
    int type = SOCK_STREAM;
    SOCKET listener = INVALID_SOCKET;
    SOCKET connector = INVALID_SOCKET;
    SOCKET acceptor = INVALID_SOCKET;
    struct sockaddr_in listen_addr;
    struct sockaddr_in connect_addr;
    socklen_t size;
    
    if (!fd) {
        qCritical(log) << "The fd is null";
        return -1;
    }
    
    listener = socket(family, type, 0);
    if (INVALID_SOCKET == listener) {
        qCritical(log) << "Create server socket fail:" << errno << "[" << strerror(errno) << "]";
        return errno;
    }
    
    do {
        memset(&listen_addr, 0, sizeof(listen_addr));
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        listen_addr.sin_port = 0;	/* kernel chooses port.	 */
        if (-1 == bind(listener, (struct sockaddr *) &listen_addr, sizeof (listen_addr)))
            break;
        if (-1 == listen(listener, 1))
            break;
        
        connector = socket(family, type, 0);
        if (INVALID_SOCKET == connector) {
            qCritical(log) << "Create connect socket fail:" << errno << "[" << strerror(errno) << "]";
            break;
        }
        
        memset(&connect_addr, 0, sizeof(connect_addr));
        
        /* We want to find out the port number to connect to.  */
        size = sizeof(connect_addr);
        if (-1 == getsockname(listener, (struct sockaddr *) &connect_addr, &size))
            break;
        if (size != sizeof (connect_addr))
            break;
        qDebug(log) << "listener port:" << ntohs(connect_addr.sin_port);
        if (-1 == ::connect(connector, (struct sockaddr *) &connect_addr,
                    sizeof(connect_addr)))
            break;
        
        size = sizeof(listen_addr);
        acceptor = accept(listener, (struct sockaddr *) &listen_addr, &size);
        if (INVALID_SOCKET == acceptor)
            break;
        if (size != sizeof(listen_addr))
            break;
        qDebug(log) << "accept port:" << ntohs(listen_addr.sin_port) << "fd:" << acceptor;
        /* Now check we are talking to ourself by matching port and host on the
	       two sockets.	 */
        if (getsockname(connector, (struct sockaddr *) &connect_addr, &size) == -1)
            break;
        qDebug(log) << "Connector port:" << ntohs(connect_addr.sin_port) << "fd:" << connector;
        if (size != sizeof (connect_addr)
            || listen_addr.sin_family != connect_addr.sin_family
            || listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
            || listen_addr.sin_port != connect_addr.sin_port)
            break;
        close(listener);
        fd[0] = connector;
        fd[1] = acceptor;
        SetSocketNonBlocking(fd[0]);
        SetSocketNonBlocking(fd[1]);
        EnableNagles(fd[0], false);
        EnableNagles(fd[1], false);
        return 0;
    } while(0);

    if (INVALID_SOCKET != listener)
        close(listener);
    if (INVALID_SOCKET != connector)
        close(connector);
    if (INVALID_SOCKET != acceptor)
        close(acceptor);

    return errno;
}

int CEvent::SetSocketNonBlocking(SOCKET fd)
{
#if defined(Q_OS_WIN)
    
    unsigned long nonblocking = 1;
    if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR) {
        qCritical(log, "ioctlsocket(%d, FIONBIO, &%lu)", (int)fd, (unsigned long)nonblocking);
        return -1;
    }
    
#else
    
    int flags;
    if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
        qCritical(log, "fcntl(%d, F_GETFL)", fd);
        return -1;
    }
    if (!(flags & O_NONBLOCK)) {
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == SOCKET_ERROR) {
            qCritical(log, "fcntl(%d, F_SETFL)", fd);
            return -1;
        }
    }
    
#endif
    return 0;
}

int CEvent::SetSocketBlocking(SOCKET fd, bool block)
{
#if defined(Q_OS_WIN)
    unsigned long nonblocking = 1;
    if(block)
        nonblocking = 0;
    if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR) {
        qCritical(log, "ioctlsocket(%d, FIONBIO, &%lu)", (int)fd, (unsigned long)nonblocking);
        return -1;
    }
#else
    int flags;
    if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
        qCritical(log, "fcntl(%d, F_GETFL)", fd);
        return -1;
    }

    if (block)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) == SOCKET_ERROR) {
        qCritical(log, "fcntl(%d, F_SETFL)", fd);
        return -1;
    }
#endif
    return 0;
}

bool CEvent::EnableNagles(SOCKET fd, bool enable) {
    int rc;
    int opt = enable ? 0 : 1;
    socklen_t optlen;
    optlen = sizeof(opt);
    /*
    rc = getsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, &optlen);
    if (SOCKET_ERROR == rc) {
        qCritical(log) << "unable to getsockopt TCP_NODELAY:"
                       << errno << "-" << strerror(errno);
        return false;
    }
    if (opt == 1)
        qDebug(log) << fd << "is TCP_NODELAY";
    else
        qDebug(log) << fd << "is not TCP_NODELAY";
    //*/
    opt = enable ? 0 : 1;
    rc = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, optlen);
    if (SOCKET_ERROR == rc) {
        qCritical(log) << "unable to setsockopt TCP_NODELAY:"
                       << errno << "-" << strerror(errno);
        return false;
    }
    /*
    rc = getsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, &optlen);
    if (SOCKET_ERROR == rc) {
        qCritical(log) << "unable to getsockopt TCP_NODELAY:"
                       << errno << "-" << strerror(errno);
        return false;
    }
    if (opt == 1)
        qDebug(log) << fd << "is TCP_NODELAY";
    else
        qDebug(log) << fd << "is not TCP_NODELAY";//*/
    return true;
}

} //namespace RabbitCommon
