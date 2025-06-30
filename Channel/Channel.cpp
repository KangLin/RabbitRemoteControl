// Author: Kang Lin <kl222@126.com>

#include "Channel.h"
#include <QtGlobal>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QTranslator>
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "Channel")

QSharedPointer<QTranslator> g_Translator = nullptr;

const qint64 CChannel::DefaultTimeout = 500000;
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
    //qDebug(log) << Q_FUNC_INFO << maxlen;
    qint64 nRet = 0;
    if(nullptr == data || 0 > maxlen) {
        qCritical(log) << Q_FUNC_INFO << "The parameters is invalid" << maxlen;
        return -1;
    }
    if(0 == maxlen) {
        qDebug(log) << Q_FUNC_INFO << "maxlen:" << maxlen;
        return 0;
    }
    if(!m_pSocket) return nRet;
    nRet = m_pSocket->read(data, maxlen);
    if(nRet < 0)
        setErrorString(m_pSocket->errorString());
    return nRet;
}

qint64 CChannel::writeData(const char *data, qint64 len)
{
    //qDebug(log) << Q_FUNC_INFO << len;
    if(nullptr == data || 0 > len) {
        qCritical(log) << Q_FUNC_INFO << "The parameters is invalid" << len;
        return -1;
    }

    if(0 == len) {
        qDebug(log) << Q_FUNC_INFO << "len:" << len;
        return 0;
    }

    qint64 nRet = 0;
    if(!m_pSocket) return nRet;
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
    if(m_pSocket) {
        szError = m_pSocket->errorString();
        setErrorString(szError);
    }
    // qDebug(log) << "CChannel::slotError()" << e << szError;
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
    qDebug(log) << Q_FUNC_INFO;
    if(!isOpen()) return;
    if(m_pSocket) {
        m_pSocket->close();
        m_pSocket = nullptr;
    }
    QIODevice::close();
}

QString CChannel::GetDetails()
{
    return QString();
}

int CChannel::InitTranslation()
{
    if(!g_Translator)
        g_Translator = RabbitCommon::CTools::Instance()->InstallTranslator(
            "Channel", RabbitCommon::CTools::TranslationType::Library);
    return 0;
}

int CChannel::RemoveTranslation()
{
    if(g_Translator)
        RabbitCommon::CTools::Instance()->RemoveTranslator(g_Translator);
    return 0;
}
