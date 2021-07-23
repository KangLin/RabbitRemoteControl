#include "DataChannel.h"
#include "../Viewer/QSocketInStream.h"
#include "../Viewer/QSocketOutStream.h"

CDataChannel::CDataChannel(QTcpSocket *pSocket, QObject *parent)
    : QIODevice(parent),
      m_pSocket(pSocket)
{
    Q_ASSERT(m_pSocket);

    bool check = false;
    m_pInStream = new CQSocketInStream(m_pSocket);
    m_pOutStream = new CQSocketOutStream(m_pSocket);
    check = connect(m_pSocket, SIGNAL(readyRead()),
            this, SIGNAL(readyRead()));
    Q_ASSERT(check);
}

CDataChannel::~CDataChannel()
{
    if(m_pInStream) delete m_pInStream;
    if(m_pOutStream) delete m_pOutStream;
    if(m_pSocket) m_pSocket->deleteLater();
}

rdr::InStream* CDataChannel::InStream()
{
    return m_pInStream;
}

rdr::OutStream* CDataChannel::OutStream()
{
    return m_pOutStream;
}

qint64 CDataChannel::readData(char *data, qint64 maxlen)
{
    return 0;
}

qint64 CDataChannel::writeData(const char *data, qint64 len)
{
    return 0;
}
