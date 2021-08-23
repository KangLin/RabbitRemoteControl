#include "InStreamDataChannel.h"
#include "RabbitCommonLog.h"

CInStreamDataChannel::CInStreamDataChannel(CChannel* pDataChannel)
    : rdr::BufferedInStream(),
      m_pDataChannel(pDataChannel)
{
}

bool CInStreamDataChannel::fillBuffer(size_t maxSize)
{
    Q_ASSERT(m_pDataChannel);
    if(!m_pDataChannel) return false;

    qint64 n = m_pDataChannel->read((char*)end, maxSize);
    if (0 == n)
      return false;

    if(-1 == n)
    {
        LOG_MODEL_ERROR("CInStreamDataChannel",
                        "CInStreamDataChannel::fillBuffer: %s",
                        m_pDataChannel->errorString().toStdString().c_str());
        //return false;
        throw std::runtime_error(m_pDataChannel->errorString().toStdString());
    }

    end += n;

    return true;
}
