#include "DataChannelIceChannel.h"
#include "RabbitCommonLog.h"
#include "IceManager.h"

CDataChannelIceChannel::CDataChannelIceChannel(
        QSharedPointer<CIceSignal> signal,
        QSharedPointer<CIceManager> iceManager,
        QObject *parent)
    : CDataChannelIce(parent),
      m_IceManager(iceManager),
      m_bServer(false)
{
    SetSignal(signal);
}

int CDataChannelIceChannel::SetSignal(QSharedPointer<CIceSignal> signal)
{
    bool check = false;
    if(!signal) return -1;
    m_Signal = signal;
    signal.get()->disconnect(this);
    check = connect(signal.get(), SIGNAL(sigError(int, const QString&)),
                    this, SLOT(slotSignalError(int, const QString&)));
    Q_ASSERT(check);
    check = connect(m_Signal.get(), SIGNAL(sigConnected()),
                    this, SLOT(slotSignalConnected()));
    Q_ASSERT(check);
    check = connect(signal.get(), SIGNAL(sigDisconnected()),
                    this, SLOT(slotSignalDisconnected()));
    Q_ASSERT(check);
    return 0;
}

int CDataChannelIceChannel::CreateDataChannel(bool bData)
{
    Q_ASSERT(m_IceManager);
    Q_ASSERT(!GetPeerUser().isEmpty());
    auto pc = m_IceManager->GetPeerConnect(m_Signal, m_Config, this);

    m_bServer = !bData;
    if(bData)
    {
        auto dc = pc->createDataChannel(GetChannelId().toStdString());
        SetDataChannel(dc);
    }

    return 0;
}

void CDataChannelIceChannel::close()
{
    m_Signal->disconnect(this);

    Q_ASSERT(m_IceManager);
    m_IceManager->CloseDataChannel(this, m_bServer);

    if(m_dataChannel)
    {
        m_dataChannel->close();
        m_dataChannel.reset();
    }

    QIODevice::close();
}
