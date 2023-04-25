//! @author Kang Lin <kl222@126.com>

#ifndef CDATACHANNELICE_H
#define CDATACHANNELICE_H

#pragma once

#include "rtc/rtc.hpp"
#include "IceSignal.h"
#include "Channel.h"
#include <memory>
#include <QMutex>
#include <QSharedPointer>

/*!
 * \~chinese
 * \brief ICE 通道类。此类实现一个 ICE 的通道。
 *
 * \~english
 * \brief The ICE channel interface class. This class implements a ICE channel.
 * 
 * \~
 * \ingroup LIBAPI_ICE LIBAPI_CHANNEL
 * \see CIceSignal
 */
class CHANNEL_EXPORT CChannelIce : public CChannel
{
    Q_OBJECT

public:
    explicit CChannelIce(CIceSignal* pSignal,
                          QObject *parent = nullptr);
    virtual ~CChannelIce();

    //! \~chinese
    //! \note 必须在调用 open 前调用此函数设置服务器属性
    //! 
    //! \~english
    //! \note These properties must be set before calling Open
    int SetConfigure(const rtc::Configuration& config);
    
    //! \~chinese
    //! 打开通道。用于激活呼叫
    //! \note 由客户端调用。调用此函数前，必须先调用 SetConfigure 设置服务器属性。
    //! 
    //! \~english
    //! Open channel. for activating calls
    //! \return true: open data channel
    //!         false: don't open data channel
    //! \note Called by client.
    //!     Must call SetConfigure set the service properties before calling it.
    virtual bool open(const QString& user,
                      const QString& peer,
                      bool bChannelId);
    //! \~chinese
    //! 打开通道。用于被动接收
    //! \note 由服务器调用。调用此函数前，必须先调用 SetConfigure 设置服务器属性。
    //! 
    //! \~english
    //! Open channel. For passive receivers
    //! \note Called by service.
    //!     Must call SetConfigure set the service properties before calling it. 
    virtual bool open(const QString& fromUser,
                      const QString& toUser,
                      const QString& channelId,
                      const QString& type,
                      const QString& sdp);
    virtual void close();

    QString GetUser();
    QString GetPeerUser();
    QString GetChannelId();

private Q_SLOTS:
    virtual void slotSignalConnected();
    virtual void slotSignalDisconnected();
    virtual void slotSignalReceiverCandidate(const QString& fromUser,
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

private:
    CChannelIce(QObject *parent = nullptr);

    virtual int SetSignal(CIceSignal* signal);
    virtual int SetDataChannel(std::shared_ptr<rtc::DataChannel>);   
    virtual int CreateDataChannel(bool bDataChannel);
    QString GenerateID(const QString& label = QString());
    
    CIceSignal* m_pSignal;
    QString m_szUser;
    QString m_szPeerUser;
    QString m_szChannelId;
    rtc::Configuration m_Config;
    std::shared_ptr<rtc::PeerConnection> m_peerConnection;
    std::shared_ptr<rtc::DataChannel> m_dataChannel;

    QByteArray m_data;
    QMutex m_MutexData;
    QLoggingCategory m_Log;

protected:
    bool isSequential() const;
    qint64 writeData(const char *data, qint64 len);
    qint64 readData(char *data, qint64 maxlen);
};

// NOTE: Don't use it!!!
class CLibDataChannelLogCallback: public QObject
{
    Q_OBJECT
public:
    explicit CLibDataChannelLogCallback(QObject *parent = nullptr);
    
public Q_SLOTS:
    void slotEnable(bool enable);

private:
    static QLoggingCategory m_Log;
    static bool m_bEnable;
    static void logCallback(rtc::LogLevel level, std::string message);
};

extern CLibDataChannelLogCallback g_LogCallback;

#endif // CDATACHANNELICE_H
