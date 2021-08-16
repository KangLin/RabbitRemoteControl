// Author: Kang Lin <kl222@126.com>

#include "DataChannel.h"
#include "QSocketInStream.h"
#include "QSocketOutStream.h"
#include "RabbitCommonLog.h"

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
    check = connect(m_pSocket, SIGNAL(connected()),
                    this, SIGNAL(sigConnected()));
    Q_ASSERT(check);
    check = connect(m_pSocket, SIGNAL(disconnected()),
                    this, SIGNAL(sigDisconnected()));
    Q_ASSERT(check);
    check = connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(slotError(QAbstractSocket::SocketError)));
    Q_ASSERT(check);
}

CDataChannel::~CDataChannel()
{
    LOG_MODEL_DEBUG("CDataChannel", "CDataChannel::~CDataChannel");
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
    if(m_pSocket)
        return m_pSocket->read(data, maxlen);
    return -1;
}

qint64 CDataChannel::writeData(const char *data, qint64 len)
{
    if(m_pSocket)
        return m_pSocket->write(data, len);
    return -1;
}

void CDataChannel::slotError(QAbstractSocket::SocketError e)
{
    QString szError;
    if(m_pSocket)
        szError = m_pSocket->errorString();
    emit sigError(e, szError);
}
