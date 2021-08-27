//! @author Kang Lin(kl222@126.com)

#ifndef CDATACHANNELICE_H
#define CDATACHANNELICE_H

#pragma once

#include "rtc/rtc.hpp"
#include "IceSignal.h"
#include "Channel.h"
#include <memory>
#include <QMutex>
#include <QSharedPointer>

class CDataChannelIce : public CChannel
{
    Q_OBJECT

public:
    explicit CDataChannelIce(QSharedPointer<CIceSignal> signal,
                          QObject *parent = nullptr);
    virtual ~CDataChannelIce();

    //! @note These properties must be set before calling Open
    int SetConfigure(const rtc::Configuration& config);
    
    //! Open channel
    //! @note Must call SetConfigure set the service properties before calling it
    //!        Called by client
    virtual bool open(const QString& user,
                      const QString& peer,
                      bool bData = true);
    //! Open channel
    //! @note Must call SetConfigure set the service properties before calling it
    //! @note Called by service
    virtual bool open(const QString& fromUser,
                      const QString& toUser,
                      const QString& channelId,
                      const QString& type,
                      const QString& sdp);
    virtual void close();

    QString GetUser();
    QString GetPeerUser();
    QString GetChannelId();
    
    virtual int SetDataChannel(std::shared_ptr<rtc::DataChannel>);

private Q_SLOTS:
    virtual void slotSignalConnected();
    virtual void slotSignalDisconnected();
    virtual void slotSignalReceiverCandiate(const QString& fromUser,
                                            const QString& toUser,
                                            const QString& channelId,
                                            const QString& mid,
                                            const QString& sdp);
    virtual void slotSignalError(int error, const QString& szError);
    virtual void slotSignalReceiverDescription(const QString& fromUser,
                                               const QString& toUser,
                                               const QString& channelId,
                                               const QString& type,
                                               const QString& sdp);

protected:
    CDataChannelIce(QObject *parent = nullptr);

    virtual int SetSignal(QSharedPointer<CIceSignal> signal);
    virtual int CreateDataChannel(bool bData);
    QString GenerateID(const QString& lable = QString());
    
    QSharedPointer<CIceSignal> m_Signal;
    QString m_szUser;
    QString m_szPeerUser;
    QString m_szChannelId;
    rtc::Configuration m_Config;
    std::shared_ptr<rtc::PeerConnection> m_peerConnection;
    std::shared_ptr<rtc::DataChannel> m_dataChannel;

    QByteArray m_data;
    QMutex m_MutexData;

protected:
    bool isSequential() const;
    qint64 writeData(const char *data, qint64 len);
    qint64 readData(char *data, qint64 maxlen);
};

#endif // CDATACHANNELICE_H
