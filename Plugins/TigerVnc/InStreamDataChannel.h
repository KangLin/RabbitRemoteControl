#ifndef CINSTREAMDATACHANNEL_H
#define CINSTREAMDATACHANNEL_H

#include "rdr/BufferedInStream.h"
#include "DataChannel.h"

class CInStreamDataChannel : public rdr::BufferedInStream
{
public:
    CInStreamDataChannel(CDataChannel* pDataChannel);
    
private:
    // rdr::BufferedInStream interface
    virtual bool fillBuffer(size_t maxSize) override;
    
    CDataChannel* m_pDataChannel;
};

#endif // CINSTREAMDATACHANNEL_H
