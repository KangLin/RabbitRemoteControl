#include "Event.h"

#if defined(Q_OS_LINUX)
#if HAVE_EVENTFD
#include <sys/eventfd.h>
#endif
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#if defined(Q_OS_WIN)
#include <WinSock2.h>
//#pragma comment(lib,"ws2_32.lib")
#endif

#include <QLoggingCategory>
static Q_LOGGING_CATEGORY(log, "Channel.Semaphore")
    
CEvent::CEvent(QObject *parent)
    : QObject{parent}
{
    qDebug(log) << "CSemaphore::CSemaphore()";
    fd[0] = -1;
    fd[1] = -1;
    Init();
}

CEvent::~CEvent()
{
    qDebug(log) << "CSemaphore::~CSemaphore()";
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
    if(-1 != fd[0]) {
        ::close(fd[0]);
        fd[0] = -1;
    }

    if(-1 != fd[1]) {
        ::close(fd[1]);
        fd[1] = -1;
    }
    return 0;
}

int CEvent::Reset()
{
    int nRet = 0;
    if(-1 == fd[0])
        return -1;

#if HAVE_EVENTFD
    eventfd_t value;
    eventfd_read(fd[0], &value);   
    //qDebug(log) << "read eventfd:" << value;
#else
    char buf[1];
    nRet = ::recv(fd[0], buf, 1, 0);
    //qDebug(log) << "Reset()";
    if(nRet < 0)
        qCritical(log) << "recv fail" << errno;
#endif
    return 0;
}

int CEvent::WakeUp()
{
    int nRet = 0;
#if HAVE_EVENTFD
    if(-1 != fd[0])
        eventfd_write(fd[0], 1);
        //qDebug(log) << "CChannelSSHTunnel::writeData: wake up";
#else
    if(-1 != fd[1]) {
        char buf[10];
        nRet = ::send(fd[1], buf, 10, 0);
        //qDebug(log) << "WakeUp()";
        if(nRet >= 0)
            return 0;
        if(nRet < 0)
            qCritical(log) << "send fail" << errno;
    }
#endif
    return nRet;
}

int CEvent::GetFd()
{
    return fd[0];
}

int CEvent::CreateSocketPair(int fd[2])
{
    int family = AF_INET;
    int type = SOCK_STREAM;
    int listener = -1;
    int connector = -1;
    int acceptor = -1;
    struct sockaddr_in listen_addr;
    struct sockaddr_in connect_addr;
    socklen_t size;
    
    if (!fd) {
        qCritical(log) << "The fd is null";
        return -1;
    }
    
    listener = socket(family, type, 0);
    if (listener < 0)
        return errno;
    
    do {
        memset(&listen_addr, 0, sizeof(listen_addr));
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        listen_addr.sin_port = 0;	/* kernel chooses port.	 */
        if (::bind(listener, (struct sockaddr *) &listen_addr, sizeof (listen_addr))
            == -1)
            break;
        if (::listen(listener, 1) == -1)
            break;
        
        connector = socket(family, type, 0);
        if (connector < 0)
            break;
        
        memset(&connect_addr, 0, sizeof(connect_addr));
        
        /* We want to find out the port number to connect to.  */
        size = sizeof(connect_addr);
        if (getsockname(listener, (struct sockaddr *) &connect_addr, &size) == -1)
            break;
        if (size != sizeof (connect_addr))
            break;
        qDebug(log) << "listener port:" << connect_addr.sin_port;
        if (::connect(connector, (struct sockaddr *) &connect_addr,
                    sizeof(connect_addr)) == -1)
            break;
        
        size = sizeof(listen_addr);
        acceptor = ::accept(listener, (struct sockaddr *) &listen_addr, &size);
        if (acceptor < 0)
            break;
        if (size != sizeof(listen_addr))
            break;
        qDebug(log) << "accept port:" << listen_addr.sin_port;
        /* Now check we are talking to ourself by matching port and host on the
	   two sockets.	 */
        if (getsockname(connector, (struct sockaddr *) &connect_addr, &size) == -1)
            break;
        qDebug(log) << "Connector port:" << connect_addr.sin_port;
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
        
        return 0;
    } while(0);

    if (listener != -1)
        close(listener);
    if (connector != -1)
        close(connector);
    if (acceptor != -1)
        close(acceptor);
    
    return errno;
}

int CEvent::SetSocketNonBlocking(int fd)
{
#ifdef _WIN32
    
    unsigned long nonblocking = 1;
    if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR) {
        qCritical(log, fd, "ioctlsocket(%d, FIONBIO, &%lu)", (int)fd, (unsigned long)nonblocking);
        return -1;
    }
    
#else
    
    int flags;
    if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
        qCritical(log, "fcntl(%d, F_GETFL)", fd);
        return -1;
    }
    if (!(flags & O_NONBLOCK)) {
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            qCritical(log, "fcntl(%d, F_SETFL)", fd);
            return -1;
        }
    }
    
#endif
    return 0;
}