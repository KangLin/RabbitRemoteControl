#ifndef COUTSTREAMDATACHANNEL_H
#define COUTSTREAMDATACHANNEL_H

#include "rdr/BufferedOutStream.h"
#include "DataChannel.h"

class COutStreamDataChannel : public rdr::BufferedOutStream
{
public:
    COutStreamDataChannel(CDataChannel* pDataChannel);
    
private:
    // rdr::BufferedOutStream interface
    virtual bool flushBuffer() override;
    CDataChannel* m_pDataChannel;
};

#endif // COUTSTREAMDATACHANNEL_H
