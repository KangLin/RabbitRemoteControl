// Author: Kang Lin <kl222@126.com>

#include "Channel.h"

CChannel::CChannel(QObject *parent)
    : QIODevice(parent),
      m_Log("Channel"),
      m_pSocket(nullptr)
{}

CChannel::~CChannel()
{
    qDebug(m_Log) << "CChannel::~CChannel";
//    if(isOpen()) close();
    if(m_pSocket) m_pSocket->deleteLater();
}

qint64 CChannel::readData(char *data, qint64 maxlen)
{
    if(m_pSocket)
        return m_pSocket->read(data, maxlen);
    return -1;
}

qint64 CChannel::writeData(const char *data, qint64 len)
{
    if(m_pSocket)
        return m_pSocket->write(data, len);
    return -1;
}

bool CChannel::isSequential() const
{
    return true;
}

void CChannel::slotConnected()
{
//    if(!isOpen())
//        if(!open(QIODevice::ReadWrite))
//        {
//            qCritical(m_Log) << "Open data channel fail";
//            return;
//        }
    emit sigConnected();
}

void CChannel::slotDisconnected()
{
    emit sigDisconnected();
//    if(!isOpen())
//        close();
}

void CChannel::slotError(QAbstractSocket::SocketError e)
{
    QString szError;
    if(m_pSocket)
        szError = m_pSocket->errorString();
    emit sigError(e, szError);
}

void CChannel::close()
{
    if(m_pSocket)
        m_pSocket->close();
    QIODevice::close();
}

bool CChannel::open(QTcpSocket *pSocket, OpenMode mode)
{
    Q_ASSERT(pSocket);
    if(m_pSocket) m_pSocket->deleteLater();
    m_pSocket = pSocket;
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
    check = connect(m_pSocket,
                #if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
                    SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
                #else
                    SIGNAL(error(QAbstractSocket::SocketError)),
                #endif
                    this, SLOT(slotError(QAbstractSocket::SocketError)));
    Q_ASSERT(check);
    return QIODevice::open(mode);
}
