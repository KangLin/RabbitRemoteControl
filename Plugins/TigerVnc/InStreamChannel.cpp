#include "InStreamChannel.h"
#include <stdexcept>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(VncStreamChannel, "VNC.Stream.Channel")

CInStreamChannel::CInStreamChannel(CChannel* pDataChannel)
    : rdr::BufferedInStream(),
      m_pDataChannel(pDataChannel)
{
}

bool CInStreamChannel::fillBuffer()
{
    //Q_ASSERT(m_pDataChannel);
    if(!m_pDataChannel || !m_pDataChannel->isOpen()) return false;

    size_t nLen = availSpace();
    if(nLen <= 0)
        return false;

    qint64 n = m_pDataChannel->read((char*)end, nLen);
    if (0 == n)
      return false;
    
    if(0 > n)
    {
        qCritical(VncStreamChannel) << "CInStreamDataChannel::fillBuffer:"
                                    << m_pDataChannel->errorString();
        //return false;
        throw std::runtime_error(m_pDataChannel->errorString().toStdString());
    }

    end += n;

    return true;
}
