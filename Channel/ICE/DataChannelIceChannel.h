#ifndef CDATACHANNELICECHANNEL_H
#define CDATACHANNELICECHANNEL_H

#include "DataChannelIce.h"
#include <QMap>
#include <QSharedPointer>

class CIceManager;
class CDataChannelIceChannel : public CDataChannelIce
{
    Q_OBJECT

public:
    CDataChannelIceChannel(QSharedPointer<CIceSignal> signal,
                           QSharedPointer<CIceManager> iceManager,
                           QObject *parent = nullptr);

    virtual void close() override;

protected:
    virtual int CreateDataChannel(bool bData) override;
    virtual int SetSignal(QSharedPointer<CIceSignal> signal) override;

    QMap<QString, std::shared_ptr<rtc::PeerConnection> > m_sPeerConnections;
    QSharedPointer<CIceManager> m_IceManager;

    bool m_bServer;
};

#endif // CDATACHANNELICECHANNEL_H
