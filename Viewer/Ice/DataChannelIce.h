//! @author Kang Lin(kl222@126.com)

#ifndef CDATACHANNELICE_H
#define CDATACHANNELICE_H

#pragma once

#include "rtc/rtc.hpp"
#include "IceSignal.h"
#include <memory>
#include <QIODevice>
#include <QMutex>
#include <QSharedPointer>

class CDataChannelIce : public QIODevice
{
    Q_OBJECT

public:
    explicit CDataChannelIce(QSharedPointer<CIceSignal> signal,
                          QObject *parent = nullptr);
    virtual ~CDataChannelIce();

    //! @note These properties must be set before calling Open
    int SetConfigure(const rtc::Configuration& config);
    //! @note The above properties must be set before calling Open
    virtual int open(const QString& user, const QString& peer, const QString& id, bool bData);
    virtual void close();

    QString GetUser();
    QString GetPeerUser();
    QString GetChannelId();

    virtual int SetDataChannel(std::shared_ptr<rtc::DataChannel>);

Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    void sigError(int nErr, const QString& szError);

private Q_SLOTS:
    virtual void slotSignalConnected();
    virtual void slotSignalDisconnected();
    virtual void slotSignalReceiverCandiate(const QString& fromUser,
                                            const QString& toUser,
                                            const QString& channelId,
                                            const QString& mid,
                                            const QString& sdp);
    virtual void slotSignalError(int error, const QString& szError);

public Q_SLOTS:
    virtual void slotSignalReceiverDescription(const QString& fromUser,
                                               const QString& toUser,
                                               const QString& channelId,
                                               const QString& type,
                                               const QString& sdp);

protected:
    CDataChannelIce(QObject *parent = nullptr);

    virtual int SetSignal(QSharedPointer<CIceSignal> signal);
    virtual int CreateDataChannel(bool bData);

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
