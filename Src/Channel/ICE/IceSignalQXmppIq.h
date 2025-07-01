//! @author Kang Lin(kl222@126.com)

#ifndef CQXMPPDATACHANNELIQ_H
#define CQXMPPDATACHANNELIQ_H

#pragma once

#include "QXmppIq.h"

class CIceSignalQXmppIq : public QXmppIq
{
public:
    CIceSignalQXmppIq();
    
    static QString ns();
    static bool isIceSignalIq(const QDomElement &element);
    
    const QString &ChannelId() const;
    void setChannelId(const QString &newChannelId);
    const QString &SignalType() const;
    void setSignalType(const QString &newType);
    const QString &Description() const;
    void setDescription(const QString &newDescription);
    const QString &Candidate() const;
    void setCandidate(const QString &newCandidate);
    const QString &mid() const;
    void setMid(const QString &newMid);
    
protected:
    virtual void parseElementFromChild(const QDomElement &element) override;
    virtual void toXmlElementFromChild(QXmlStreamWriter *writer) const override;
    
private:
    QString m_ChannelId;
    QString m_SignalType;
    QString m_Description;
    QString m_Candidate;
    QString m_mid;
};

#endif // CQXMPPDATACHANNELIQ_H
