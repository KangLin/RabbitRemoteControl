//! @author Kang Lin <kl222@126.com>

#include "ChannelIce.h"
#include "rtc/rtc.hpp"
#include "RabbitCommonLog.h"
#include <QDebug>
#include <QThread>

CChannelIce::CChannelIce(QObject* parent) : CChannel(parent)
{}

void logCallback(rtc::LogLevel level, std::string message)
{
    switch (level) {
    case rtc::LogLevel::Verbose:
    case rtc::LogLevel::Debug:
        LOG_MODEL_DEBUG("Libdatachannel", message.c_str());
        break;
    case rtc::LogLevel::Info:
        LOG_MODEL_INFO("Libdatachannel", message.c_str());
        break;
    case rtc::LogLevel::Warning:
        LOG_MODEL_WARNING("Libdatachannel", message.c_str());
        break;
    case rtc::LogLevel::Error:
    case rtc::LogLevel::Fatal:
        LOG_MODEL_ERROR("Libdatachannel", message.c_str());
        break;
    case rtc::LogLevel::None:
        break;
    }
}

static bool gInitLog = false;

CChannelIce::CChannelIce(CIceSignal* pSignal, QObject *parent)
    : CChannel(parent),
      m_pSignal(pSignal)
{
    //*
    if(!gInitLog) {
        gInitLog = true;
        rtc::InitLogger(rtc::LogLevel::Debug, logCallback);//*/
    }

    SetSignal(pSignal);
}

CChannelIce::~CChannelIce()
{
    qDebug() << "CDataChannel::~CDataChannel()";
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
                        SIGNAL(sigCandiate(const QString&,
                                           const QString&,
                                           const QString&,
                                           const QString&,
                                           const QString&)),
                        this,
                        SLOT(slotSignalReceiverCandiate(const QString&,
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
//    LOG_MODEL_DEBUG("DataChannel", "onDataChannel: DataCannel label: %s",
//                    dc->label().c_str());
    Q_ASSERT(dc);
    if(!dc) return -1;

    m_dataChannel = dc;

    dc->onOpen([this]() {
        LOG_MODEL_INFO("DataChannel", "Open data channel user:%s;peer:%s;channelId:%s:lable:%s",
                        GetUser().toStdString().c_str(),
                        GetPeerUser().toStdString().c_str(),
                        GetChannelId().toStdString().c_str(),
                        m_dataChannel->label().c_str());
        
        emit sigConnected();
    });

    dc->onClosed([this]() {
        LOG_MODEL_INFO("DataChannel", "Close data channel: user:%s;peer:%s;channelId:%s:lable:%s",
                        GetUser().toStdString().c_str(),
                        GetPeerUser().toStdString().c_str(),
                        GetChannelId().toStdString().c_str(),
                        m_dataChannel->label().c_str());
        emit sigDisconnected();
    });

    dc->onError([this](std::string error){
        LOG_MODEL_ERROR("DataChannel", "Data channel error:%s", error.c_str());
        emit sigError(-1, error.c_str());
    });

    dc->onMessage([dc, this](std::variant<rtc::binary, std::string> data) {
//        if (std::holds_alternative<std::string>(data))
//            LOG_MODEL_DEBUG("DataChannel", "From remote data: %s",
//                            std::get<std::string>(data).c_str());
//        else
//            LOG_MODEL_DEBUG("DataChannel", "From remote Received, size=%d",
//                            std::get<rtc::binary>(data).size());
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
        LOG_MODEL_ERROR("DataChannel", "Peer connect don't open");
        return -1;
    }
    m_peerConnection->onStateChange([this](rtc::PeerConnection::State state) {
        LOG_MODEL_DEBUG("DataChannel", "PeerConnection State: %d", state);
        switch (state) {
        case rtc::PeerConnection::State::Failed:
            emit sigError(-1, "peer connection state is fail");
            break;
        case rtc::PeerConnection::State::Closed:
            emit sigDisconnected();
            break;
        default:
            break;
        }
    });
    m_peerConnection->onGatheringStateChange(
                [](rtc::PeerConnection::GatheringState state) {
        Q_UNUSED(state)
        LOG_MODEL_DEBUG("DataChannel", "Gathering status: %d", state);
    });
    m_peerConnection->onLocalDescription(
                [this](rtc::Description description) {
        //LOG_MODEL_DEBUG("CDataChannelIce", "The thread id: 0x%X", QThread::currentThreadId());
        /*
        LOG_MODEL_DEBUG("DataChannel", "user:%s; peer:%s; channel:%s; onLocalDescription: %s",
                        GetUser().toStdString().c_str(),
                        GetPeerUser().toStdString().c_str(),
                        GetChannelId().toStdString().c_str(),
                        std::string(description).c_str());//*/
        // Send to the peer through the signal channel
        if(m_szPeerUser.isEmpty() || m_szChannelId.isEmpty())
           LOG_MODEL_ERROR("DataChannel", "Please set peer user and channel id");
        m_pSignal->SendDescription(GetPeerUser(), GetChannelId(), description, GetUser());
    });
    m_peerConnection->onLocalCandidate(
                [this](rtc::Candidate candidate){
        //LOG_MODEL_DEBUG("CDataChannelIce", "The thread id: 0x%X", QThread::currentThreadId());
        /*
        LOG_MODEL_DEBUG("DataChannel", "user:%s; peer:%s; channel:%s; onLocalCandidate: %s, mid: %s",
                        GetUser().toStdString().c_str(),
                        GetPeerUser().toStdString().c_str(),
                        GetChannelId().toStdString().c_str(),
                        std::string(candidate).c_str(),
                        candidate.mid().c_str());//*/
        // Send to the peer through the signal channel
        if(m_szPeerUser.isEmpty() || m_szChannelId.isEmpty())
           LOG_MODEL_ERROR("DataChannel", "Please set peer user and channel id");
        m_pSignal->SendCandiate(m_szPeerUser, m_szChannelId, candidate);
    });
    m_peerConnection->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dc) {
        LOG_MODEL_INFO("DataChannel", "Open data channel:%s",
                        dc->label().c_str());
        if(dc->label().c_str() != GetChannelId())
        {
            LOG_MODEL_ERROR("DataChannel", "Channel label diffent: %s; %s",
                            GetChannelId().toStdString().c_str(),
                            dc->label().c_str());
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
    bRet = QIODevice::open(QIODevice::ReadWrite);
    if(!bRet) return false;
    
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
    LOG_MODEL_DEBUG("CDataChannelIce", "CDataChannelIce::Close()");

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
        LOG_MODEL_ERROR("CDataChannelIce", "m_dataChannel is nullptr");
        return -1;
    }

    if(m_dataChannel->isClosed())
    {
        LOG_MODEL_ERROR("CDataChannelIce",
                        "m_dataChannel->isClosed():peer:%s;channel:%s",
                        GetPeerUser().toStdString().c_str(),
                        GetChannelId().toStdString().c_str());
        return -1;
    }
    
    if(!isOpen())
    {
        LOG_MODEL_ERROR("CDataChannelIce", "The data channel isn't open");
        return -1;
    }
    
    bool bSend = false;

    if(0 == len)
    {
        LOG_MODEL_WARNING("CDataChannelIce", "WriteData len is zero");
        return 0;
    }
    qint64 n = len;
    while(n > DEFAULT_MAX_MESSAGE_SIZE)
    {
        bSend = m_dataChannel->send((const std::byte*)data, DEFAULT_MAX_MESSAGE_SIZE);
        if(!bSend)
        {
            LOG_MODEL_ERROR("CDataChannelIce", "Send fail. len:%d;n:%d", len, n);
            return -1;
        }
        n -= DEFAULT_MAX_MESSAGE_SIZE;
    }
    if(n > 0)
    {
        bSend = m_dataChannel->send((const std::byte*)data, n);
        if(bSend) return len;
    }
    LOG_MODEL_ERROR("CDataChannelIce", "Send fail. Len:%d; n:0X%X", len, n);
    return -1;
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
    emit sigError(-1, tr("Signal disconnected"));
}

void CChannelIce::slotSignalReceiverCandiate(const QString& fromUser,
                                                 const QString &toUser,
                                                 const QString &channelId,
                                                 const QString& mid,
                                                 const QString& sdp)
{
    /*
    LOG_MODEL_DEBUG("CDataChannelIce",
                    "slotSignalReceiverCandiate fromUser:%s; toUser:%s; channelId:%s; mid:%s; sdp:%s",
                    fromUser.toStdString().c_str(),
                    toUser.toStdString().c_str(),
                    channelId.toStdString().c_str(),
                    mid.toStdString().c_str(),
                    sdp.toStdString().c_str()); //*/
    if(GetPeerUser() != fromUser || GetUser() != toUser
            || GetChannelId() != channelId) return;
    if(m_peerConnection)
    {
        rtc::Candidate candiate(sdp.toStdString(), mid.toStdString());
        m_peerConnection->addRemoteCandidate(candiate);
    }
}

void CChannelIce::slotSignalReceiverDescription(const QString& fromUser,
                                                    const QString &toUser,
                                                    const QString &channelId,
                                                    const QString &type,
                                                    const QString &sdp)
{
    /*
    LOG_MODEL_DEBUG("CDataChannelIce",
                    "slotSignalReceiverDescription fromUser:%s; toUser:%s; channelId:%s; type:%s; sdp:%s",
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

QString CChannelIce::GenerateID(const QString &lable)
{
    static qint64 id = 0;
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QString szId = lable;
    szId += QString::number(id++);
    return szId;
}
