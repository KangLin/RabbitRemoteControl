#include "InStreamChannel.h"
#include <stdexcept>

Q_LOGGING_CATEGORY(VncStreamChannel, "VNC.Stream.Channel")

CInStreamChannel::CInStreamChannel(CChannel* pDataChannel)
    : rdr::BufferedInStream(),
      m_pDataChannel(pDataChannel)
{
}

bool CInStreamChannel::fillBuffer(size_t maxSize)
{
    //Q_ASSERT(m_pDataChannel);
    if(!m_pDataChannel || !m_pDataChannel->isOpen()) return false;

    qint64 n = m_pDataChannel->read((char*)end, maxSize);
    if (0 == n)
      return false;

    if(-1 == n)
    {
        qCritical(VncStreamChannel) << "CInStreamDataChannel::fillBuffer:"
                                    << m_pDataChannel->errorString();
        //return false;
        throw std::runtime_error(m_pDataChannel->errorString().toStdString());
    }

    end += n;

    return true;
}
