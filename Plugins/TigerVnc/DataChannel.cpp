// Author: Kang Lin <kl222@126.com>

#include "DataChannel.h"
#include "InStreamDataChannel.h"
#include "OutStreamDataChannel.h"
#include "QSocketInStream.h"
#include "QSocketOutStream.h"
#include "RabbitCommonLog.h"

CDataChannel::CDataChannel(QTcpSocket *pSocket, QObject *parent)
    : QIODevice(parent),
      m_pSocket(pSocket)
{
    Q_ASSERT(m_pSocket);

    m_pInStream = new CInStreamDataChannel(this);
    m_pOutStream = new COutStreamDataChannel(this);

    bool check = false;
    check = connect(m_pSocket, SIGNAL(readyRead()),
            this, SIGNAL(readyRead()));
    Q_ASSERT(check);
    check = connect(m_pSocket, SIGNAL(connected()),
                    this, SLOT(slotConnected()));
    Q_ASSERT(check);
    check = connect(m_pSocket, SIGNAL(disconnected()),
                    this, SLOT(slotDisconnected()));
    Q_ASSERT(check);
    check = connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(slotError(QAbstractSocket::SocketError)));
    Q_ASSERT(check);
}

CDataChannel::~CDataChannel()
{
    LOG_MODEL_DEBUG("CDataChannel", "CDataChannel::~CDataChannel");
    if(isOpen()) close();
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

bool CDataChannel::isSequential() const
{
    return true;
}

void CDataChannel::slotConnected()
{
    if(!open(QIODevice::ReadWrite))
    {
        LOG_MODEL_ERROR("CDataChannel", "Open data channel fail");
        return;
    }
    emit sigConnected();
}

void CDataChannel::slotDisconnected()
{
    emit sigDisconnected();
    close();
}

void CDataChannel::slotError(QAbstractSocket::SocketError e)
{
    QString szError;
    if(m_pSocket)
        szError = m_pSocket->errorString();
    emit sigError(e, szError);
}
