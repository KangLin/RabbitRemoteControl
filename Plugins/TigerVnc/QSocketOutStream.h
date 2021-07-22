#ifndef CQSOCKETOUTSTREAM_H
#define CQSOCKETOUTSTREAM_H

#include "rdr/BufferedOutStream.h"
#include <QTcpSocket>

class CQSocketOutStream : public rdr::BufferedOutStream
{    
public:
    CQSocketOutStream(QTcpSocket* pSocket = nullptr);
    
private:
    // rdr::BufferedOutStream interface
    virtual bool flushBuffer() override;
    
    QTcpSocket* m_pSocket;
};

#endif // CQSOCKETOUTSTREAM_H
