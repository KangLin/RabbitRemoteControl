// Author: Kang Lin <kl222@126.com>

#ifndef CQSOCKETINSTREAM_H
#define CQSOCKETINSTREAM_H

#include "rdr/BufferedInStream.h"
#include <QTcpSocket>

class CQSocketInStream : public rdr::BufferedInStream
{
public:
    CQSocketInStream(QTcpSocket* pSocket);

private:
    // rdr::BufferedInStream interface
    virtual bool fillBuffer(size_t maxSize) override;
    
    QTcpSocket* m_pSocket;
};

#endif // CQSOCKETINSTREAM_H
