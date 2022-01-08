#ifndef CQSOCKET_H
#define CQSOCKET_H

#include "network/Socket.h"
#include <QTcpSocket>

class CQSocket : public QObject, network::Socket
{
    Q_OBJECT
    
public:
    CQSocket();
    
    // Socket interface
public:
    virtual char *getPeerAddress() override;
    virtual char *getPeerEndpoint() override;
    
    QTcpSocket m_Socket;
};

#endif // CQSOCKET_H
