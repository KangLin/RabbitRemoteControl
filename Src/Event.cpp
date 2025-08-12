#include <QLoggingCategory>
#include <QStandardPaths>
#include <QThread>
#include <QDir>

#include <chrono>
#include "Event.h"

#if defined(Q_OS_WIN)
    #if defined(HAVE_UNIX_DOMAIN_SOCKET)
        /* [AF_UNIX comes to Windows](https://devblogs.microsoft.com/commandline/af_unix-comes-to-windows/)
         * How can I write a Windows AF_UNIX app?
         *   - Download the Windows Insiders SDK for the Windows build 17061 — available here.
         *   - Check whether your Windows build has support for unix socket by running “sc query afunix” from a Windows admin command prompt.
         *   - #include <afunix.h> in your Windows application and write a Windows unix socket winsock application as you would write any other unix socket application, but, using Winsock API’s.
         */
        #include <afunix.h>
    #endif
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
    #if defined(HAVE_UNIX_DOMAIN_SOCKET)
        #include <sys/un.h>
    #endif
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

namespace Channel {

static Q_LOGGING_CATEGORY(log, "SSH.Event")

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
    qDebug(log) << "Use eventfd";
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
    if(nRet >= 0) {
        //qDebug(log) << "recv" << nRet;
        return 0;
    }
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

qintptr CEvent::GetFd()
{
    return fd[0];
}

int CEvent::CreateSocketPair(qintptr fd[])
{
    int family = AF_INET;
    int type = SOCK_STREAM;
    SOCKET listener = INVALID_SOCKET;
    SOCKET connector = INVALID_SOCKET;
    SOCKET acceptor = INVALID_SOCKET;
    socklen_t size = 0;
    
    if (!fd) {
        qCritical(log) << "The fd is null";
        return -1;
    }

#if defined(HAVE_UNIX_DOMAIN_SOCKET)
    struct sockaddr_un listen_addr;
    struct sockaddr_un connect_addr;
    QString szPath;
    QString szUnixDomainSocket;
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

    family = AF_UNIX;
#else
    struct sockaddr_in listen_addr;
    struct sockaddr_in connect_addr;
#endif
    
    size = sizeof(listen_addr);
    memset(&listen_addr, 0, sizeof(listen_addr));
    memset(&connect_addr, 0, sizeof(connect_addr));

    listener = socket(family, type, 0);
    if (INVALID_SOCKET == listener) {
        qCritical(log) << "Create server socket fail:"
                       << errno << "[" << strerror(errno) << "]";
        return errno;
    }
    
    do {

#if defined(HAVE_UNIX_DOMAIN_SOCKET)
        listen_addr.sun_family = AF_UNIX;
        strcpy(listen_addr.sun_path, szUnixDomainSocket.toStdString().c_str());
#else
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        listen_addr.sin_port = 0;	/* kernel chooses port.	 */
#endif
        if (-1 == bind(listener, (struct sockaddr *) &listen_addr, sizeof(listen_addr)))
        {
            qCritical(log) << "bind fail:" << strerror(errno)
                           << GetAddress(&listen_addr);
            break;
        }
        if (-1 == listen(listener, 1))
        {
            qCritical(log) << "listen fail:" << strerror(errno)
                           << GetAddress(&listen_addr);
            break;
        }

#if !defined(HAVE_UNIX_DOMAIN_SOCKET)
        /* We want to find out the port number to connect to.  */
        if (-1 == getsockname(listener, (struct sockaddr *) &listen_addr, &size))
        {
            qCritical(log) << "Get listen socket address fail";
            break;
        }
        if (sizeof(listen_addr) != size) {
            qCritical(log) << "getsockname return size greater then supplied to the call:"
                           << "return size:" << size
                           << "call size:" << sizeof(listen_addr);
            break;
        }
#endif
        qDebug(log) << "Socket listener:" << GetAddress(&listen_addr);

        connector = socket(family, type, 0);
        if (INVALID_SOCKET == connector) {
            qCritical(log) << "Create connect socket fail:"
                           << errno << "[" << strerror(errno) << "]";
            break;
        }
        
        if (-1 == ::connect(connector, (struct sockaddr *) &listen_addr, size)) {
            qDebug(log) << "connect fail:" << strerror(errno)
                        << GetAddress(&listen_addr);
            break;
        }

        acceptor = accept(listener, (struct sockaddr *) &listen_addr, &size);
        if (INVALID_SOCKET == acceptor)
        {
            qDebug(log) << "accept fail:" << strerror(errno)
                        << GetAddress(&listen_addr);
            break;
        }
        qDebug(log) << "accept:" << GetAddress(&listen_addr)
                    << "accept fd:" << acceptor << "connect fd:" << connector;
#if !defined(HAVE_UNIX_DOMAIN_SOCKET)
        if (sizeof(listen_addr) != size) {
            qCritical(log) << "accept return size greater then supplied to the call:"
                           << "return size:" << size
                           << "call size:" << sizeof(listen_addr);
            break;
        }
        /* Now check we are talking to ourself by matching port and host on the
	       two sockets.	 */
        if (getsockname(connector, (struct sockaddr *) &connect_addr, &size) == -1) {
            qCritical(log) << "Get connector socket address fail";
            break;
        }
        qDebug(log) << "Connector port:" << ntohs(connect_addr.sin_port) << "fd:" << connector;
        if (size != sizeof (connect_addr)
            || listen_addr.sin_family != connect_addr.sin_family
            || listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
            || listen_addr.sin_port != connect_addr.sin_port) {
            qCritical(log) << "Accept address"
                           << ntohs(listen_addr.sin_port)
                           << "is not same connect address"
                           << ntohs(connect_addr.sin_port);
            break;
        }
#endif
        close(listener);
        fd[0] = connector;
        fd[1] = acceptor;

        SetSocketNonBlocking(fd[0]);
        SetSocketNonBlocking(fd[1]);
#if !defined(HAVE_UNIX_DOMAIN_SOCKET)
        EnableNagles(fd[0], false);
        EnableNagles(fd[1], false);
#endif
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

QString CEvent::GetAddress(void* address)
{
    QString szAdd;
#if defined(HAVE_UNIX_DOMAIN_SOCKET)
    struct sockaddr_un* pAdd = (struct sockaddr_un*)address;
    szAdd = pAdd->sun_path;
#else
    struct sockaddr_in* pAdd = (struct sockaddr_in*)address;
    szAdd = inet_ntoa(pAdd->sin_addr) + QString(":") + QString::number(ntohs(pAdd->sin_port));
#endif

    return szAdd;
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
