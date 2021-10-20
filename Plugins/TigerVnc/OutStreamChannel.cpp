#include "OutStreamChannel.h"
#include "RabbitCommonLog.h"
#include <stdexcept>

COutStreamChannel::COutStreamChannel(CChannel* pDataChannel)
    : rdr::BufferedOutStream(),
      m_pDataChannel(pDataChannel)
{
}

bool COutStreamChannel::flushBuffer()
{
    Q_ASSERT(m_pDataChannel);
    qint64 n = m_pDataChannel->write((const char*)sentUpTo, ptr - sentUpTo);
    
    if (0 == n)
      return false;
    
    if(-1 == n)
    {
        LOG_MODEL_ERROR("COutStreamDataChannel",
                        "COutStreamDataChannel::fillBuffer: %s",
                        m_pDataChannel->errorString().toStdString().c_str());
        //return false;
        throw std::runtime_error(m_pDataChannel->errorString().toStdString());
    }
    
    sentUpTo += n;
  
    return true;
}
