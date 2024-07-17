// Author: Kang Lin <kl222@126.com>

#include "Channel.h"
#include <QtGlobal>
#include <QCoreApplication>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Channel")

CChannel::CChannel(QObject *parent)
    : QIODevice(parent),
      m_pSocket(nullptr)
{}

CChannel::~CChannel()
{
    qDebug(log) << "CChannel::~CChannel";
//    if(isOpen()) close();
    if(m_pSocket) m_pSocket->deleteLater();
}

qint64 CChannel::readData(char *data, qint64 maxlen)
{
    /*
    qDebug(log) << "CChannel::readData:"
                << maxlen << "nLen:" << m_readData.size();//*/
    QMutexLocker locker(&m_readMutex);
    if(m_readData.isEmpty())
        return 0;
    
    qint64 nLen = qMin((qint64)m_readData.size(), maxlen);
    if(nLen > 0) {
        memcpy(data, m_readData.data(), nLen);
        m_readData.remove(0, nLen);
    }
    return nLen;
}

qint64 CChannel::writeData(const char *data, qint64 len)
{
    //qDebug(log) << "CChannel::writeData:" << len;
    if(nullptr == data || 0 >= len) {
        qCritical(log) << "writeData fail. len:" << len;
        return 0;
    }

    m_writeMutex.lock();
    m_writeData.append(data, len);
    m_writeMutex.unlock();
    
    WakeUp();
    
    return len;
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
    /*
    qDebug(log) << "CChannel::slotError()" << e
                << m_pSocket->errorString();//*/
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
    if(m_pSocket && m_pSocket != pSocket)
        m_pSocket->deleteLater();

    m_pSocket = pSocket;
    bool check = false;
    check = connect(m_pSocket, SIGNAL(readyRead()),
            this, SLOT(slotReadyRead()));
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

void CChannel::slotReadyRead()
{
    if(!m_pSocket)
        return;
    
    const int len = 1024;
    QSharedPointer<char> buf(new char[len]);
    int nRet = 0;
    
    do {
        nRet = m_pSocket->read(buf.data(), len);
        if(nRet == 0)
            break;
        if(nRet < 0) {
            qDebug(log) << "CChannel::slotReadyRead()" << m_pSocket->error()
                           << m_pSocket->errorString();
            emit sigError(m_pSocket->error(), m_pSocket->errorString());
            return;
        }
        QMutexLocker locker(&m_readMutex);
        m_readData.append(buf.data(), nRet);
    } while(nRet == len);
    
    if(m_readData.size() > 0)
        emit readyRead();
}

int CChannel::WakeUp()
{
    QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    return 0;
}

bool CChannel::event(QEvent *event)
{
    //qDebug(log) << "CChannel::event:" << event;
    if(QEvent::User == event->type()) {
        if(m_pSocket)
        {
            QMutexLocker lock(&m_writeMutex);
            if(m_writeData.size() > 0){
                int nLen = m_pSocket->write(m_writeData.data(), m_writeData.size());
                if(nLen > 0)
                    m_writeData.remove(0, nLen);
                else if(nLen < 0)
                {
                    qDebug(log) << "write fail:" << m_pSocket->error()
                                   << m_pSocket->errorString();
                    emit sigError(m_pSocket->error(), m_pSocket->errorString());
                }
            }
        }
        return true;
    }
    return QIODevice::event(event);
}