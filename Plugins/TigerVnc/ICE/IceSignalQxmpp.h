//! @author Kang Lin(kl222@126.com)

#ifndef CICESIGNALQXMPP_H
#define CICESIGNALQXMPP_H

#include "IceSignal.h"
#include "QXmppClient.h"
#include "IceSignalQXmppManager.h"

class CIceSignalQxmpp : public CIceSignal
{
    Q_OBJECT

public:
    explicit CIceSignalQxmpp(QObject *parent = nullptr);

    virtual int Open(const std::string &szServer,
                     quint16 nPort,
                     const std::string &user,
                     const std::string &password) override;
    virtual int Close() override;
    virtual bool IsOpen() override;
    virtual int SendDescription(const QString &toUser,
                                const QString &channelId,
                                const rtc::Description &description,
                                const QString &fromUser) override;
    virtual int SendCandiate(const QString &toUser,
                             const QString &channelId,
                             const rtc::Candidate &candidate,
                             const QString &fromUser) override;
    virtual int Write(const char *buf, int nLen) override;
    virtual int Read(char *buf, int nLen) override;

    bool proecssIq(CIceSignalQXmppIq iq);
    
private Q_SLOTS:
    void slotError(QXmppClient::Error e);
    void slotSendPackage(CIceSignalQXmppIq iq);
    
Q_SIGNALS:
    void sigSendPackage(CIceSignalQXmppIq iq);

private:
    QXmppClient m_Client;
    CIceSignalQXmppManager m_Manager;
};

#endif // CICESIGNALQXMPP_H
