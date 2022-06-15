//! @author Kang Lin(kl222@126.com)

#ifndef CICESIGNAL_H
#define CICESIGNAL_H

#include "IceSignal.h"
#include "rtc/rtc.hpp"
#include <QMutex>

class CIceSignalWebSocket : public CIceSignal
{
    Q_OBJECT

public:
    explicit CIceSignalWebSocket(QObject *parent = nullptr);
    virtual ~CIceSignalWebSocket();

    virtual int Open(const std::string& szServer, quint16 nPort,
                     const std::string &user = std::string(),
                     const std::string &password = std::string()) override;
    int Open(const std::string &szUrl,
             const std::string &user = std::string(),
             const std::string &password = std::string());
    virtual int Close() override;
    virtual bool IsOpen() override;

    virtual int SendDescription(const QString& toUser,
                                const QString& channelId,
                                const rtc::Description& description,
                                const QString& fromUser = QString()) override;
    virtual int SendCandiate(const QString& toUser,
                             const QString& channelId,
                             const rtc::Candidate& candidate,
                             const QString& fromUser = QString()) override;

    virtual int Write(const char* buf, int nLen) override;
    virtual int Read(char* buf, int nLen) override;

private:
    std::shared_ptr<rtc::WebSocket> m_webSocket;
    std::string m_szUrl;
    std::string m_szUser, m_szPeerUser;
    rtc::binary m_Data;
    QMutex m_MutexSend;
};

#endif // CICESIGNAL_H
