//! \author Kang Lin <kl222@126.com>

#include "ChannelIce.h"
#include "rtc/rtc.hpp"
#include <QThread>

///////////////////////// Set libdatachannel log callback function ///////////////////////

QLoggingCategory CLibDataChannelLogCallback::m_Log = QLoggingCategory("Channel.ICE.LibDataChannel");
CLibDataChannelLogCallback::CLibDataChannelLogCallback(QObject *parent)
    : QObject(parent)
{
    rtc::InitLogger(rtc::LogLevel::Debug, logCallback);
}

void CLibDataChannelLogCallback::slotEnable(bool enable)
{
    m_bEnable = enable;
}

void CLibDataChannelLogCallback::logCallback(rtc::LogLevel level, std::string message)
{
    if(!m_bEnable) return;
    switch (level) {
    case rtc::LogLevel::Verbose:
    case rtc::LogLevel::Debug:
        qDebug(m_Log) << message.c_str();
        break;
    case rtc::LogLevel::Info:
        qInfo(m_Log) << message.c_str();
        break;
    case rtc::LogLevel::Warning:
        qWarning(m_Log) << message.c_str();
        break;
    case rtc::LogLevel::Error:
    case rtc::LogLevel::Fatal:
        qCritical(m_Log) << message.c_str();
        break;
    case rtc::LogLevel::None:
        break;
    }
}

bool CLibDataChannelLogCallback::m_bEnable = true;
// Set libdatachannel log callback function
CLibDataChannelLogCallback g_LogCallback;
///////////////////////// End set libdatachannel log callback function ///////////////////////

CChannelIce::CChannelIce(QObject* parent) : CChannel(parent),
    m_Log("Channel.ICE")
{}

CChannelIce::CChannelIce(CIceSignal* pSignal, QObject *parent)
    : CChannel(parent),
      m_pSignal(pSignal),
      m_Log("Channel.ICE")
{
    SetSignal(pSignal);
}

CChannelIce::~CChannelIce()
{
    qDebug(m_Log) << "CChannelIce::~CChannelIce()";
}

int CChannelIce::SetSignal(CIceSignal *signal)
{
    bool check = false;
    m_pSignal = signal;
    if(m_pSignal)
    {
        check = connect(m_pSignal, SIGNAL(sigConnected()),
                        this, SLOT(slotSignalConnected()));
        Q_ASSERT(check);
        check = connect(m_pSignal, SIGNAL(sigDisconnected()),
                        this, SLOT(slotSignalDisconnected()));
        Q_ASSERT(check);
        check = connect(m_pSignal,
                        SIGNAL(sigDescription(const QString&,
                                              const QString&,
                                              const QString&,
                                              const QString&,
                                              const QString&)),
                        this,
                        SLOT(slotSignalReceiverDescription(const QString&,
                                                           const QString&,
                                                           const QString&,
                                                           const QString&,
                                                           const QString&)));
        Q_ASSERT(check);
        check = connect(m_pSignal,
                        SIGNAL(sigCandidate(const QString&,
                                           const QString&,
                                           const QString&,
                                           const QString&,
                                           const QString&)),
                        this,
                        SLOT(slotSignalReceiverCandidate(const QString&,
                                                        const QString&,
                                                        const QString&,
                                                        const QString&,
                                                        const QString&)));
        Q_ASSERT(check);
        check = connect(m_pSignal, SIGNAL(sigError(int, const QString&)),
                        this, SLOT(slotSignalError(int, const QString&)));
        Q_ASSERT(check);
    }
    return 0;
}

QString CChannelIce::GetUser()
{
    return m_szUser;
}

QString CChannelIce::GetPeerUser()
{
    return m_szPeerUser;
}

QString CChannelIce::GetChannelId()
{
    return m_szChannelId;
}

int CChannelIce::SetConfigure(const rtc::Configuration &config)
{
    m_Config = config;
    return 0;
}

int CChannelIce::SetDataChannel(std::shared_ptr<rtc::DataChannel> dc)
{
    //qDebug(m_Log) << "onDataChannel: DataCannel label:" << dc->label().c_str();
    Q_ASSERT(dc);
    if(!dc) return -1;

    m_dataChannel = dc;

    dc->onOpen([this]() {
        qInfo(m_Log) << "Open data channel user:" << GetUser()
                      << ";peer:" << GetPeerUser()
                      << ";channelId:" << GetChannelId()
                      << ";label:" << m_dataChannel->label().c_str();
        if(QIODevice::open(QIODevice::ReadWrite))
            emit sigConnected();
        else
            qInfo(m_Log) << "Open Device fail:user:" << GetUser()
                          << ";peer:" << GetPeerUser()
                          << ";channelId:" << GetChannelId()
                          << ";label:" << m_dataChannel->label().c_str();
    });

    dc->onClosed([this]() {
        qInfo(m_Log) << "Close data channel user:" << GetUser()
                      << ";peer:" << GetPeerUser()
                      << ";channelId:" << GetChannelId()
                      << ";label:" << m_dataChannel->label().c_str();
        emit sigDisconnected();
    });

    dc->onError([this](std::string error){
        qInfo(m_Log) << "Data channel error:" << error.c_str();
        emit sigError(-1, error.c_str());
    });

    dc->onMessage([dc, this](std::variant<rtc::binary, std::string> data) {
        /*
        if (std::holds_alternative<std::string>(data))
            qDebug(m_Log) << "From remote data:"
                           << std::get<std::string>(data).c_str();
        else
            qDebug(m_Log) << "From remote Received, size="
                           << std::get<rtc::binary>(data).size(); //*/
        m_MutexData.lock();
        rtc::binary d = std::get<rtc::binary>(data);
        m_data.append((char*)d.data(), d.size());
        m_MutexData.unlock();
        emit this->readyRead();
    });

    return 0;
}

int CChannelIce::CreateDataChannel(bool bDataChannel)
{
    m_peerConnection = std::make_shared<rtc::PeerConnection>(m_Config);
    if(!m_peerConnection)
    {
        qDebug(m_Log) << "Peer connect don't open";
        return -1;
    }
    m_peerConnection->onStateChange([this](rtc::PeerConnection::State state) {
        qDebug(m_Log) << "PeerConnection State:" << (int)state;
    });
    m_peerConnection->onGatheringStateChange(
                [this](rtc::PeerConnection::GatheringState state) {
        Q_UNUSED(state)
        qDebug(m_Log) << "Gathering status:" << (int)state;
    });
    m_peerConnection->onLocalDescription(
                [this](rtc::Description description) {
        //qDebug(m_Log) << "The thread id:" << QThread::currentThreadId();
        /*
        qDebug(m_Log) << "user:" << GetUser()
                       << "peer:" << GetPeerUser() 
                       << "channel:" << GetChannelId()
                       << "onLocalDescription:"
                       << std::string(description).c_str());//*/
        // Send to the peer through the signal channel
        if(m_szPeerUser.isEmpty() || m_szChannelId.isEmpty())
        {
            qCritical(m_Log) << "Please set peer user and channel id";
            return;
        }
        m_pSignal->SendDescription(GetPeerUser(), GetChannelId(), description, GetUser());
    });
    m_peerConnection->onLocalCandidate(
                [this](rtc::Candidate candidate){
        //qDebug(m_Log) << "The thread id:" << QThread::currentThreadId();
        /*
        qDebug(m_Log) << "user:" << GetUser()
                       << "peer:" << GetPeerUser() 
                       << "channel:" << GetChannelId()
                       << "onLocalCandidate:" << std::string(candidate).c_str()
                       << "mid:" << candidate.mid().c_str();//*/
        // Send to the peer through the signal channel
        if(m_szPeerUser.isEmpty() || m_szChannelId.isEmpty())
        {
           qDebug(m_Log) << "Please set peer user and channel id";
           return;
        }
        m_pSignal->SendCandidate(m_szPeerUser, m_szChannelId, candidate);
    });
    m_peerConnection->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dc) {
        qInfo(m_Log) << "Open data channel:" << dc->label().c_str();
        if(dc->label().c_str() != GetChannelId())
        {
            qDebug(m_Log) << "Channel label different:" << GetChannelId()
                           << dc->label().c_str();
            return;
        }

        SetDataChannel(dc);
    });

    if(bDataChannel)
    {
        auto dc = m_peerConnection->createDataChannel(GetChannelId().toStdString());
        SetDataChannel(dc);
    }
    return 0;
}

bool CChannelIce::open(const QString &user, const QString &peer, bool bChannelId)
{
    bool bRet = false;
    m_szPeerUser = peer;
    m_szUser = user;
    if(bChannelId)
        m_szChannelId = GenerateID("c_");
    int nRet = CreateDataChannel(bChannelId);
    if(nRet) return false;
    return true;
}

bool CChannelIce::open(const QString &fromUser, const QString &toUser,
                           const QString &channelId, const QString &type,
                           const QString &sdp)
{
    m_szChannelId = channelId;
    if(!open(toUser, fromUser, false))
        return false;
    slotSignalReceiverDescription(fromUser, toUser, channelId, type, sdp);
    return true;
}

void CChannelIce::close()
{
    qDebug(m_Log) << "CChannelIce::Close()";

    m_pSignal->disconnect(this);

    if(m_dataChannel)
    {
        m_dataChannel->close();
        m_dataChannel.reset();
    }
    if(m_peerConnection)
    {
        m_peerConnection->close();
        m_peerConnection.reset();
    }

    CChannel::close();
    return;
}

#define DEFAULT_MAX_MESSAGE_SIZE 0x7FFF
qint64 CChannelIce::writeData(const char *data, qint64 len)
{
    if(!m_dataChannel)
    {
        qDebug(m_Log) << "m_dataChannel is nullptr";
        return -1;
    }

    if(m_dataChannel->isClosed())
    {
        qDebug(m_Log) << "m_dataChannel->isClosed():peer:" << GetPeerUser()
                       << "channel:" << GetChannelId();
        return -2;
    }
    
    if(!isOpen())
    {
        qCritical(m_Log) << "The data channel isn't open";
        return -3;
    }
    
    bool bSend = false;

    if(0 == len)
    {
        qWarning(m_Log) << "WriteData len is zero";
        return 0;
    }
    qint64 n = len;
    while(n > DEFAULT_MAX_MESSAGE_SIZE)
    {
        bSend = m_dataChannel->send((const std::byte*)data, DEFAULT_MAX_MESSAGE_SIZE);
        if(!bSend)
        {
            qCritical(m_Log) << "Send fail. len:" << len << "n:" << n;
            return -4;
        }
        n -= DEFAULT_MAX_MESSAGE_SIZE;
    }
    if(n > 0)
    {
        bSend = m_dataChannel->send((const std::byte*)data, n);
        if(bSend) return len;
    }
    qCritical(m_Log) << "Send fail. Len:" << len << "n:" << n;
    return -5;
}

qint64 CChannelIce::readData(char *data, qint64 maxlen)
{
    if(!m_dataChannel || m_dataChannel->isClosed() || !isOpen()) return -1;

    QMutexLocker lock(&m_MutexData);

    if(m_data.size() == 0)
        return 0;
    
    qint64 n = maxlen;
    if(static_cast<int>(maxlen) > m_data.size())
        n = m_data.size();
    memcpy(data, m_data.data(), n);
    if(m_data.size() == n)
        m_data.clear();
    else
        m_data.remove(0, n);
    return n;
}

bool CChannelIce::isSequential() const
{
    return true;
}

void CChannelIce::slotSignalConnected()
{
}

void CChannelIce::slotSignalDisconnected()
{
    //emit sigError(-1, tr("Signal disconnected"));
}

void CChannelIce::slotSignalReceiverCandidate(const QString& fromUser,
                                                 const QString &toUser,
                                                 const QString &channelId,
                                                 const QString& mid,
                                                 const QString& sdp)
{
    /*
    qDebug(m_Log, "slotSignalReceiverCandidate fromUser:%s; toUser:%s; channelId:%s; mid:%s; sdp:%s",
           fromUser.toStdString().c_str(),
           toUser.toStdString().c_str(),
           channelId.toStdString().c_str(),
           mid.toStdString().c_str(),
           sdp.toStdString().c_str()); //*/
    if(GetPeerUser() != fromUser || GetUser() != toUser
            || GetChannelId() != channelId) return;
    if(m_peerConnection)
    {
        rtc::Candidate Candidate(sdp.toStdString(), mid.toStdString());
        m_peerConnection->addRemoteCandidate(Candidate);
    }
}

void CChannelIce::slotSignalReceiverDescription(const QString& fromUser,
                                                    const QString &toUser,
                                                    const QString &channelId,
                                                    const QString &type,
                                                    const QString &sdp)
{
    /*
    qDebug(m_Log, "slotSignalReceiverDescription fromUser:%s; toUser:%s; channelId:%s; type:%s; sdp:%s",
           fromUser.toStdString().c_str(),
           toUser.toStdString().c_str(),
           channelId.toStdString().c_str(),
           type.toStdString().c_str(),
           sdp.toStdString().c_str()); //*/
    
    if(GetPeerUser() != fromUser
            || GetUser() != toUser
            || GetChannelId() != channelId)
        return;

    rtc::Description des(sdp.toStdString(), type.toStdString());
    if(m_peerConnection)
        m_peerConnection->setRemoteDescription(des);
}

void CChannelIce::slotSignalError(int error, const QString& szError)
{
    //emit sigError(error, tr("Signal error: %1").arg(szError));
}

QString CChannelIce::GenerateID(const QString &label)
{
    static qint64 id = 0;
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QString szId = label;
    szId += QString::number(id++);
    return szId;
}
