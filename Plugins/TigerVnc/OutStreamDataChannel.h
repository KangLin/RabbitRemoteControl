#ifndef COUTSTREAMDATACHANNEL_H
#define COUTSTREAMDATACHANNEL_H

#include "rdr/BufferedOutStream.h"
#include "Channel.h"

class COutStreamDataChannel : public rdr::BufferedOutStream
{
public:
    COutStreamDataChannel(CChannel* pDataChannel);
    
private:
    // rdr::BufferedOutStream interface
    virtual bool flushBuffer() override;
    CChannel* m_pDataChannel;
};

#endif // COUTSTREAMDATACHANNEL_H
