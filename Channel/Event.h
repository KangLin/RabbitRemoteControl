#ifndef CSEMAPHORE_H
#define CSEMAPHORE_H

#include <QObject>

#if defined(Q_OS_WIN)
    #include <WinSock2.h>
#else
    #define SOCKET int
#endif

#include "channel_export.h"

namespace Channel {

class CHANNEL_EXPORT CEvent : public QObject
{
    Q_OBJECT
public:
    explicit CEvent(QObject *parent = nullptr);
    virtual ~CEvent();
    
    int Reset();
    int WakeUp();
    
    SOCKET GetFd();
    
    static int SetSocketNonBlocking(SOCKET fd);
    static int SetSocketBlocking(SOCKET fd, bool block);
    static bool EnableNagles(SOCKET fd, bool enable);

    static QString GetAddress(void* address);

private:
    SOCKET fd[2];
    
    int Init();
    int Clear();
    int CreateSocketPair(SOCKET fd[2]);
};

} //namespace RabbitCommon

#endif // CSEMAPHORE_H
