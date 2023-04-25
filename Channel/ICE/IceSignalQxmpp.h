//! @author Kang Lin(kl222@126.com)

#ifndef CICESIGNALQXMPP_H
#define CICESIGNALQXMPP_H

#include "IceSignal.h"
#include "QXmppClient.h"
#include "IceSignalQXmppManager.h"

/*!
 * \~chinese
 * \brief QXMPP 实现 ICE 信令类
 * \details 此信令基于 [libdatachannel](https://github.com/paullouisageneau/libdatachannel)
 * 
 * \~english
 * \brief QXMPP implements ICE signaling class
 * \details The signal is base on [libdatachannel](https://github.com/paullouisageneau/libdatachannel)
 * 
 * \~
 * \ingroup LIBAPI_ICE
 */
class CIceSignalQxmpp : public CIceSignal
{
    Q_OBJECT

public:
    explicit CIceSignalQxmpp(QObject *parent = nullptr);

    virtual int Open(const QString &szServer,
                     quint16 nPort,
                     const QString &user = QString(),
                     const QString &password = QString()) override;
    virtual int Close() override;
    virtual bool IsConnected() override;
    virtual int SendDescription(const QString &toUser,
                                const QString &channelId,
                                const rtc::Description &description,
                                const QString &fromUser) override;
    virtual int SendCandidate(const QString &toUser,
                             const QString &channelId,
                             const rtc::Candidate &candidate,
                             const QString &fromUser) override;

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
