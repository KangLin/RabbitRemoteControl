#ifndef __EVENT_H_KL_2025_05_23__
#define __EVENT_H_KL_2025_05_23__

#pragma once

#include <QObject>

#if defined(Q_OS_WIN)
    #include <WinSock2.h>
#else
    #define SOCKET int
#endif

#include "plugin_export.h"

namespace Channel {

class PLUGIN_EXPORT CEvent : public QObject
{
    Q_OBJECT
public:
    explicit CEvent(QObject *parent = nullptr);
    virtual ~CEvent();
    
    int Reset();
    int WakeUp();
    
    qintptr GetFd();
    
    static int SetSocketNonBlocking(SOCKET fd);
    static int SetSocketBlocking(SOCKET fd, bool block);
    static bool EnableNagles(SOCKET fd, bool enable);

    static QString GetAddress(void* address);

private:
    qintptr fd[2];
    
    int Init();
    int Clear();
    int CreateSocketPair(qintptr fd[]);
};

} //namespace Channel

#endif // __EVENT_H_KL_2025_05_23__
