#include "OutStreamChannel.h"

#include <stdexcept>

Q_DECLARE_LOGGING_CATEGORY(VncStreamChannel)

COutStreamChannel::COutStreamChannel(CChannel* pDataChannel)
    : rdr::BufferedOutStream(),
      m_pDataChannel(pDataChannel)
{
}

bool COutStreamChannel::flushBuffer()
{
    //Q_ASSERT(m_pDataChannel);
    if(!m_pDataChannel || !m_pDataChannel->isOpen()) return false;
    qint64 n = m_pDataChannel->write((const char*)sentUpTo, ptr - sentUpTo);

    if (0 == n)
      return false;

    if(-1 == n)
    {
        qCritical(VncStreamChannel) << "COutStreamDataChannel::fillBuffer:"
                                    << m_pDataChannel->errorString();
        return false;
        throw std::runtime_error(m_pDataChannel->errorString().toStdString());
    }

    sentUpTo += n;

    return true;
}
