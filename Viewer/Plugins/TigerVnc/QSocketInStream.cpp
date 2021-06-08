#include "QSocketInStream.h"
#include <exception>

CQSocketInStream::CQSocketInStream(QTcpSocket *pSocket)
    : rdr::BufferedInStream(),
    m_pSocket(pSocket)
{
}

bool CQSocketInStream::fillBuffer(size_t maxSize)
{
    Q_ASSERT(m_pSocket);
    if(!m_pSocket) return false;

    qint64 n = m_pSocket->read((char*)end, maxSize);
    if (0 == n)
      return false;

    if(-1 == n)
        throw std::runtime_error(m_pSocket->errorString().toStdString().c_str());

    end += n;

    return true;
}
