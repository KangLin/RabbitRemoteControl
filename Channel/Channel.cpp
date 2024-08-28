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

CChannel::CChannel(QTcpSocket *pSocket, QObject *parent)
    : QIODevice(parent),
      m_pSocket(pSocket)
{
    qDebug(log) << "CChannel::CChannel";
    Q_ASSERT(m_pSocket);
}

CChannel::~CChannel()
{
    qDebug(log) << "CChannel::~CChannel";
}

qint64 CChannel::readData(char *data, qint64 maxlen)
{
    int nRet = 0;
    /*
    qDebug(log) << "CChannel::readData:"
                << maxlen; //*/
    if(m_pSocket)
        nRet = m_pSocket->read(data, maxlen);
    if(nRet < 0) {
        setErrorString(m_pSocket->errorString());
    }
    return nRet;
}

qint64 CChannel::writeData(const char *data, qint64 len)
{
    //qDebug(log) << "CChannel::writeData:" << len;
    if(nullptr == data || 0 >= len) {
        qCritical(log) << "writeData fail. len:" << len;
        return 0;
    }
    
    int nRet = 0;
    if(m_pSocket)
        nRet = m_pSocket->write(data, len);
    if(nRet < 0)
        setErrorString(m_pSocket->errorString());
    return nRet;
}

bool CChannel::isSequential() const
{
    return true;
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

bool CChannel::open(OpenMode mode)
{
    bool check = false;
    
    if(!m_pSocket) return false;
    
    check = connect(m_pSocket, SIGNAL(readyRead()),
            this, SIGNAL(readyRead()));
    Q_ASSERT(check);
    check = connect(m_pSocket, SIGNAL(connected()),
                    this, SIGNAL(sigConnected()));
    Q_ASSERT(check);
    check = connect(m_pSocket, SIGNAL(disconnected()),
                    this, SIGNAL(sigDisconnected()));
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

void CChannel::close()
{
    if(m_pSocket)
        m_pSocket->close();
    QIODevice::close();
}

QString CChannel::GetDetails()
{
    return QString();
}
