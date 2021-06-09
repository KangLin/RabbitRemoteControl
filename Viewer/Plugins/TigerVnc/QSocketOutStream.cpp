#include "QSocketOutStream.h"
#include <stdexcept>

CQSocketOutStream::CQSocketOutStream(QTcpSocket *pSocket)
    : rdr::BufferedOutStream(),
      m_pSocket(pSocket)
{
}

bool CQSocketOutStream::flushBuffer()
{
    qint64 n = m_pSocket->write((const char*)sentUpTo, ptr - sentUpTo);
    
    if (0 == n)
      return false;
    
    if(-1 == n)
        throw std::runtime_error(m_pSocket->errorString().toStdString());
  
    sentUpTo += n;
  
    return true;
}
