// Author: Kang Lin <kl222@126.com>

#include "libssh/libssh.h"
#include "libssh/callbacks.h"

#include "ChannelSSHTunnel.h"
#include <QLoggingCategory>
#include <QThread>

#include <QAbstractEventDispatcher>
#include <QScopedArrayPointer>
#include <QtGlobal>
#if defined(Q_OS_LINUX)
    #include <sys/eventfd.h>
#endif
#if defined(Q_OS_WIN)
    #include <WinSock2.h>
    //#pragma comment(lib,"ws2_32.lib")
#endif

static Q_LOGGING_CATEGORY(log, "SSH.Tunnel")

CChannelSSHTunnel::CChannelSSHTunnel(
    CParameterSSHTunnel* parameter,
    CParameterNet *remote,
    CBackend *pBackend,
    bool bWakeUp,
    QObject *parent)
    : CChannelSSH(pBackend, parameter, bWakeUp, parent)
    , m_pRemoteNet(remote)
/*
    m_pSocketRead(nullptr),
    m_pSocketWrite(nullptr),
    m_pSocketException(nullptr),
*/
{
    qDebug(log) << "CChannelSSHTunnel::CChannelSSHTunnel()";
    qDebug(log) << "libssh version:" << ssh_version(0);
}

CChannelSSHTunnel::~CChannelSSHTunnel()
{
    qDebug(log) << "CChannelSSHTunnel::~CChannelSSHTunnel()";
    if(m_pEvent)
        delete m_pEvent;
}

int CChannelSSHTunnel::GetSocket()
{
    if(m_Session)
        return ssh_get_fd(m_Session);
    return SSH_INVALID_SOCKET;
}

void CChannelSSHTunnel::OnClose()
{
    qDebug(log) << "CChannelSSHTunnel::close()";

    if(!isOpen()) return;

    WakeUp();

    /*
    QAbstractEventDispatcher* pDispatcher = QAbstractEventDispatcher::instance();
    if(m_pSocketRead) {
        pDispatcher->unregisterSocketNotifier(m_pSocketRead);
        m_pSocketRead->deleteLater();
        m_pSocketRead = nullptr;
    }
    if(m_pSocketWrite) {
        pDispatcher->unregisterSocketNotifier(m_pSocketWrite);
        m_pSocketWrite->deleteLater();
        m_pSocketWrite = nullptr;
    }
    if(m_pSocketException) {
        pDispatcher->unregisterSocketNotifier(m_pSocketException);
        m_pSocketException->deleteLater();
        m_pSocketException = nullptr;
    }//*/

    if(m_Channel) {
        if(ssh_channel_is_open(m_Channel)) {
            ssh_channel_close(m_Channel);
        }
        ssh_channel_free(m_Channel);
        m_Channel = NULL;
    }
}

int CChannelSSHTunnel::OnOpen(ssh_session session)
{
    int nRet = 0;

    Q_ASSERT(session);
    
    m_Channel = ssh_channel_new(session);
    if(NULL == m_Channel) {
        qCritical(log) << "ssh_channel_new fail." << ssh_get_error(session);
        return -1;
    }
    
    CParameterSSHTunnel* pPara = qobject_cast<CParameterSSHTunnel*>(m_pParameter);
    Q_ASSERT(pPara);
    nRet = ssh_channel_open_forward(
        m_Channel,
        m_pRemoteNet->GetHost().toStdString().c_str(),
        m_pRemoteNet->GetPort(),
        pPara->GetSourceHost().toStdString().c_str(),
        pPara->GetSourcePort());
    if(SSH_OK != nRet) {
        ssh_channel_free(m_Channel);
        m_Channel = NULL;

        QString szErr;
        szErr = tr("SSH failed: open forward.") + ssh_get_error(session);
        szErr += "(" + m_pRemoteNet->GetHost()
                + ":" + QString::number(m_pRemoteNet->GetPort()) + ")";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return nRet;
    }

    qInfo(log) << "Connected:"
               << m_pRemoteNet->GetHost()
                      + ":" + QString::number(m_pRemoteNet->GetPort())
               << "with ssh turnnel:"
               << m_pParameter->m_Net.GetHost()
                      + ":" + QString::number(m_pParameter->m_Net.GetPort());

    //ssh_channel_set_blocking(m_Channel, 0);

    return nRet;
}

/*!
 * \return
 *   \li >= 0: continue, Interval call time (msec)
 *   \li = -1: stop
 *   \li < -1: error
 */
int CChannelSSHTunnel::Process()
{
    int nRet = 0;
    
    if(!m_Channel || !ssh_channel_is_open(m_Channel)
        || ssh_channel_is_eof(m_Channel)) {
        QString szErr = "The channel is not open";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }

    struct timeval timeout = {0, DEFAULT_TIMEOUT};
    ssh_channel channels[2], channel_out[2];
    channels[0] = m_Channel;
    channels[1] = nullptr;
    
    fd_set set;
    FD_ZERO(&set);
    socket_t fd = SSH_INVALID_SOCKET;
    if(m_pEvent)
        fd = m_pEvent->GetFd();
    if(SSH_INVALID_SOCKET != fd)
        FD_SET(fd, &set);
    
    //qDebug(log) << "ssh_select:" << fd;
    nRet = ssh_select(channels, channel_out, fd + 1, &set, &timeout);
    //qDebug(log) << "ssh_select end:" << nRet;
    if(EINTR == nRet)
        return 0;

    if(SSH_OK != nRet) {
        QString szErr;
        szErr = "ssh_channel_select failed: " + QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -3;
    }

    if(SSH_INVALID_SOCKET != fd && FD_ISSET(fd, &set)) {
        //qDebug(log) << "fires event";
        if(m_pEvent) {
            nRet = m_pEvent->Reset();
            if(nRet) return -4;
        }
    }

    if(!channel_out[0]) {
        //qDebug(log) << "The channel is not select";
        return 0;
    }

    if(ssh_channel_is_eof(m_Channel)) {
        qWarning(log) << "Channel is eof";
        setErrorString(tr("The channel is eof"));
        // Stop
        return -1;
    }

    // Get channel data length
    nRet = ssh_channel_poll(m_Channel, 0);
    //qDebug(log) << "Get channel data length:" << nRet;
    if(SSH_ERROR == nRet)
    {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        return -6;
    } else if(SSH_EOF == nRet) {
        // Stop
        return -1;
    } else if(0 > nRet) {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        // Error
        return -7;
    } else if(0 == nRet) {
        //qDebug(log) << "The channel has not data";
        return 0;
    }

    emit readyRead();

    return 0;
}

// Because is same thread
qint64 CChannelSSHTunnel::readData(char *data, qint64 maxlen)
{
    qint64 nRet = 0;

    //qDebug(log) << Q_FUNC_INFO << maxlen;

    Q_ASSERT(data && maxlen >= 0);
    if(nullptr == data || 0 > maxlen) {
        qCritical(log) << Q_FUNC_INFO << "The parameters is invalid" << maxlen;
        return -1;
    }

    if(0 == maxlen) {
        qCritical(log) << Q_FUNC_INFO << "maxlen:" << maxlen;
        return 0;
    }

    if(!m_Channel || !ssh_channel_is_open(m_Channel))
    {
        QString szErr;
        szErr = "The channel is not opened";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }

    nRet = ssh_channel_read_nonblocking(m_Channel, data, maxlen, 0);
    if(SSH_AGAIN == nRet) {
        qDebug(log) << Q_FUNC_INFO << "ssh again read";
        return 0;
    } else if(0 > nRet) {
        QString szErr;
        szErr = "Read data from channel failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        return nRet;
    }

    return nRet;
}

qint64 CChannelSSHTunnel::writeData(const char *data, qint64 len)
{
    qint64 nRet = 0;

    Q_ASSERT(data && len >= 0);
    if(nullptr == data || 0 > len) {
        qCritical(log) << Q_FUNC_INFO << "The parameters is invalid" << len;
        return -1;
    }

    if(0 == len) {
        qCritical(log) << Q_FUNC_INFO << "len:" << len;
        return 0;
    }

    if(!m_Channel || !ssh_channel_is_open(m_Channel) || ssh_channel_is_eof(m_Channel))
    {
        QString szErr;
        szErr = "The channel is not opened";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }

    nRet = ssh_channel_write(m_Channel, data, len);
    if(SSH_AGAIN == nRet) {
        qDebug(log) << Q_FUNC_INFO << "ssh again write";
        return 0;
    } else if(nRet < 0) {
        QString szErr;
        szErr = "Write data from channel failed:";
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -2;
    }

    return nRet;
}

int CChannelSSHTunnel::DoWait(bool bWrite, int timeout)
{
    int nRet = 0;
    if(!m_Channel || !ssh_channel_is_open(m_Channel)
        || ssh_channel_is_eof(m_Channel)) {
        QString szErr = "The channel is not open";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }
    
    fd_set set;
    FD_ZERO(&set);

    struct timeval tm = {0, timeout};
    ssh_channel channels[2], channel_out[2];
    channels[0] = m_Channel;
    channels[1] = nullptr;
    
    if(bWrite) {
        socket_t fd = SSH_INVALID_SOCKET;
        if(m_pEvent)
            fd = GetSocket();
        if(SSH_INVALID_SOCKET != fd)
            FD_SET(fd, &set);
        nRet = select(fd + 1, nullptr, &set, nullptr, &tm);
        if(0 > nRet) return nRet;
        return 0;
    }

    //qDebug(log) << "ssh_select:" << fd;
    nRet = ssh_select(channels, channel_out, 1, &set, &tm);
    //qDebug(log) << "ssh_select end:" << nRet;
    if(EINTR == nRet)
        return 0;
    
    if(SSH_OK != nRet) {
        QString szErr;
        szErr = "ssh_channel_select failed: " + QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -3;
    }
    
    if(!channel_out[0]) {
        //qDebug(log) << "The channel is not select";
        return 0;
    }

    if(ssh_channel_is_eof(m_Channel)) {
        qWarning(log) << "Channel is eof";
        setErrorString(tr("The channel is eof"));
        // Stop
        return -1;
    }
    
    // Get channel data length
    nRet = ssh_channel_poll(m_Channel, 0);
    //qDebug(log) << "Get channel data length:" << nRet;
    if(SSH_ERROR == nRet)
    {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        return -6;
    } else if(SSH_EOF == nRet) {
        // Stop
        return -1;
    } else if(0 > nRet) {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        // Error
        return -7;
    } else if(0 == nRet) {
        //qDebug(log) << "The channel has not data";
        return 0;
    }
    return 0;
}

/*
int CChannelSSHTunnel::ProcessSocket()
{
    int nRet = 0;
    bool check = false;
    socket_t fd = ssh_get_fd(m_Session);
    m_pSocketRead = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    if(m_pSocketRead) {
        check = connect(
            m_pSocketRead, &QSocketNotifier::activated,
            this, [&](int fd) {
                qDebug(log) << "QSocketNotifier::activated: read";
                Q_UNUSED(fd)
                emit this->readyRead();
            });
        Q_ASSERT(check);
    }
    
    // m_pSocketWrite = new QSocketNotifier(fd, QSocketNotifier::Write, this);
    // if(m_pSocketWrite) {
    //     check = connect(
    //         m_pSocketWrite, &QSocketNotifier::activated,
    //         this, [&](int fd){
    //             Q_UNUSED(fd)
    //             qDebug(log) << "QSocketNotifier::activated: write";
    //         });
    //     Q_ASSERT(check);
    // }
    
    m_pSocketException = new QSocketNotifier(fd, QSocketNotifier::Exception, this);
    if(m_pSocketException) {
        check = connect(
            m_pSocketException, &QSocketNotifier::activated,
            this, [&](int) {
                qDebug(log) << "QSocketNotifier::activated: Exception";
                QString szErr;
                szErr = "Channel exception:";
                szErr += ssh_get_error(m_Session);
                qCritical(log) << szErr;
                emit sigError(-1, szErr);
            });
        Q_ASSERT(check);
    }
    
    return nRet;
}
//*/
