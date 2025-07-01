//! @author Kang Lin(kl222@126.com)

#include "IceSignalQXmppIq.h"
#include <QDomComment>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(ChannelICE, "Channel.ICE")

CIceSignalQXmppIq::CIceSignalQXmppIq()
{   
}

QString CIceSignalQXmppIq::ns()
{
    return "KangLinStudio.Rabbit.DataChannel";
}

bool CIceSignalQXmppIq::isIceSignalIq(const QDomElement &element)
{
    QDomElement domElement = element.firstChildElement("query");
    return (domElement.namespaceURI() == ns());
}

void CIceSignalQXmppIq::parseElementFromChild(const QDomElement &element)
{
    QDomElement dataChannelElement = element.firstChildElement("query");
    if(dataChannelElement.isNull())
    {
        qCritical(ChannelICE) << "webrtcElement is null";
        return;
    }
    m_ChannelId = dataChannelElement.attribute("channelid");
    m_SignalType = dataChannelElement.attribute("type");
    if("candidate" == SignalType())
    {
        m_Candidate = dataChannelElement.firstChildElement("candidate").text();
        m_mid = dataChannelElement.firstChildElement("mid").text();
    } else {
        m_Description = dataChannelElement.firstChildElement("description").text();
    }
}

void CIceSignalQXmppIq::toXmlElementFromChild(QXmlStreamWriter *writer) const
{
    writer->writeStartElement("query");
    writer->writeAttribute("xmlns", ns());
    writer->writeAttribute("channelid", ChannelId());
    writer->writeAttribute("type", SignalType());
    if("candidate" == SignalType())
    {
        writer->writeTextElement("candidate", Candidate());
        writer->writeTextElement("mid", mid());
    } else {
        writer->writeTextElement("description", Description());
    }
    writer->writeEndElement();    
}

const QString &CIceSignalQXmppIq::ChannelId() const
{
    return m_ChannelId;
}

void CIceSignalQXmppIq::setChannelId(const QString &newChannelId)
{
    m_ChannelId = newChannelId;
}

const QString &CIceSignalQXmppIq::SignalType() const
{
    return m_SignalType;
}

void CIceSignalQXmppIq::setSignalType(const QString &newType)
{
    m_SignalType = newType;
}

const QString &CIceSignalQXmppIq::Description() const
{
    return m_Description;
}

void CIceSignalQXmppIq::setDescription(const QString &newDescription)
{
    m_Description = newDescription;
}

const QString &CIceSignalQXmppIq::Candidate() const
{
    return m_Candidate;
}

void CIceSignalQXmppIq::setCandidate(const QString &newCandidate)
{
    m_Candidate = newCandidate;
}

const QString &CIceSignalQXmppIq::mid() const
{
    return m_mid;
}

void CIceSignalQXmppIq::setMid(const QString &newMid)
{
    m_mid = newMid;
}
