//! @author Kang Lin(kl222@126.com)

#ifndef CQXMPPDATACHANNELMANAGER_H
#define CQXMPPDATACHANNELMANAGER_H

#include "QXmppClientExtension.h"
#include "IceSignalQXmppIq.h"

class CIceSignalQxmpp;
class CIceSignalQXmppManager : public QXmppClientExtension
{
    Q_OBJECT
public:
    CIceSignalQXmppManager(CIceSignalQxmpp* pSignal);
    
    int sendPacket(CIceSignalQXmppIq &iq);
    // QXmppClientExtension interface
public:
    virtual QStringList discoveryFeatures() const override;
    virtual bool handleStanza(const QDomElement &element) override;

private:
    CIceSignalQxmpp* m_pSignal;
};

#endif // CQXMPPDATACHANNELMANAGER_H
