#ifndef CQSOCKET_H
#define CQSOCKET_H

#include "network/Socket.h"
#include <QObject>

class CQSocket : public QObject, network::Socket
{
    Q_OBJECT
    
public:
    CQSocket();
};

#endif // CQSOCKET_H
