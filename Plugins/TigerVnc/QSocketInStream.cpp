// Author: Kang Lin <kl222@126.com>

#include "QSocketInStream.h"
#include <stdexcept>

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
        throw std::runtime_error(m_pSocket->errorString().toStdString());

    end += n;

    return true;
}
