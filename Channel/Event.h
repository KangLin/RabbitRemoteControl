#ifndef CSEMAPHORE_H
#define CSEMAPHORE_H

#include <QObject>

class CEvent : public QObject
{
    Q_OBJECT
public:
    explicit CEvent(QObject *parent = nullptr);
    virtual ~CEvent();
    
    int Reset();
    int WakeUp();
    
    int GetFd();

private:
    int fd[2];
    
    int Init();
    int Clear();
    int CreateSocketPair(int fd[2]);
    int SetSocketNonBlocking(int fd);
};

#endif // CSEMAPHORE_H
