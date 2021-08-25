#ifndef COUTSTREAMDATACHANNEL_H
#define COUTSTREAMDATACHANNEL_H

#include "rdr/BufferedOutStream.h"
#include "Channel.h"

class COutStreamChannel : public rdr::BufferedOutStream
{
public:
    COutStreamChannel(CChannel* pDataChannel);
    
private:
    // rdr::BufferedOutStream interface
    virtual bool flushBuffer() override;
    CChannel* m_pDataChannel;
};

#endif // COUTSTREAMDATACHANNEL_H
