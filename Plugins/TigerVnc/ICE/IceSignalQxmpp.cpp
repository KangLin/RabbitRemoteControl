//! @author Kang Lin(kl222@126.com)

#include "IceSignalQxmpp.h"
#include "RabbitCommonLog.h"
#include "QXmppUtils.h"

int g_CIceSignalQXmppIq = qRegisterMetaType<CIceSignalQXmppIq>("CIceSignalQXmppIq");

CIceSignalQxmpp::CIceSignalQxmpp(QObject *parent)
    : CIceSignal(parent),
      m_Manager(this)
{
    bool check = false;

    //m_Client.logger()->setLoggingType(QXmppLogger::StdoutLogging);
    m_Client.addExtension(&m_Manager);

    check = connect(&m_Client, SIGNAL(connected()),
                    this, SIGNAL(sigConnected()));
    Q_ASSERT(check);
    check = connect(&m_Client, SIGNAL(disconnected()),
                    this, SIGNAL(sigDisconnected()));
    Q_ASSERT(check);
    check = connect(&m_Client, SIGNAL(error(QXmppClient::Error)),
                    this, SLOT(slotError(QXmppClient::Error)));
    Q_ASSERT(check);
    
    check = connect(this, SIGNAL(sigSendPackage(CIceSignalQXmppIq)),
                    this, SLOT(slotSendPackage(CIceSignalQXmppIq)));
    Q_ASSERT(check);
}

int CIceSignalQxmpp::Open(const std::string &szServer, quint16 nPort, const std::string &user, const std::string &password)
{
    QXmppConfiguration conf;
    conf.setHost(szServer.c_str());
    conf.setPort(nPort);
    conf.setJid(user.c_str());
    conf.setPassword(password.c_str());
    /*
    conf.setUser(QXmppUtils::jidToUser(user.c_str()));
    conf.setDomain(QXmppUtils::jidToDomain(user.c_str()));
    conf.setResource(QXmppUtils::jidToResource(user.c_str()));
    //*/
    m_Client.connectToServer(conf);
    return 0;
}

int CIceSignalQxmpp::Close()
{
    m_Client.disconnectFromServer();
    return 0;
}

bool CIceSignalQxmpp::IsOpen()
{
    return m_Client.isConnected();
}

bool CIceSignalQxmpp::proecssIq(CIceSignalQXmppIq iq)
{
    /*
    LOG_MODEL_DEBUG("CIceSignalQxmpp", "from:%s;to:%s;type:%s",
                    iq.from().toStdString().c_str(),
                    iq.to().toStdString().c_str(),
                    iq.SignalType().toStdString().c_str());//*/
    if(iq.SignalType() == "offer")
    {
        /*
        LOG_MODEL_DEBUG("CIceSignalQxmpp", "type:%s; sdp:%s",
                        iq.SignalType().toStdString().c_str(),
                        iq.Description().toStdString().c_str());//*/
        emit sigOffer(iq.from(),
                      iq.to(),
                      iq.ChannelId(),
                      iq.SignalType(),
                      iq.Description());
    } else if(iq.SignalType() == "answer") {
        /*
        LOG_MODEL_DEBUG("CIceSignalQxmpp", "type:%s; sdp:%s",
                        iq.SignalType().toStdString().c_str(),
                        iq.Description().toStdString().c_str());//*/
        emit sigDescription(iq.from(),
                            iq.to(),
                            iq.ChannelId(),
                            iq.SignalType(),
                            iq.Description());
    } else if (iq.SignalType()  == "candidate") {
        /*
        LOG_MODEL_DEBUG("CIceSignalQxmpp", "type:%s; mid:%s, candiate:%s",
                        iq.SignalType().toStdString().c_str(),
                        iq.mid().toStdString().c_str(),
                        iq.Candiate().toStdString().c_str());//*/
        emit sigCandiate(iq.from(),
                         iq.to(),
                         iq.ChannelId(),
                         iq.mid(),
                         iq.Candiate());
    } else {
        LOG_MODEL_ERROR("CIceSignalQxmpp", "iq type error: %s",
                        iq.SignalType().toStdString().c_str());
        return false;
    }
    return true;
}

int CIceSignalQxmpp::SendDescription(const QString &toUser,
                                     const QString &channelId,
                                     const rtc::Description &description,
                                     const QString &fromUser)
{
    CIceSignalQXmppIq iq;
    iq.setType(QXmppIq::Set);
    iq.setTo(toUser);
    iq.setChannelId(channelId);
    iq.setSignalType(description.typeString().c_str());
    iq.setDescription(std::string(description).c_str());
    emit sigSendPackage(iq);
    return 0;
}

int CIceSignalQxmpp::SendCandiate(const QString &toUser,
                                  const QString &channelId,
                                  const rtc::Candidate &candidate,
                                  const QString &fromUser)
{
    CIceSignalQXmppIq iq;
    iq.setType(QXmppIq::Set);
    iq.setTo(toUser);
    iq.setChannelId(channelId);
    iq.setSignalType("candidate");
    iq.setMid(candidate.mid().c_str());
    iq.setCandiate(std::string(candidate).c_str());
    emit sigSendPackage(iq);
    return 0;
}

void CIceSignalQxmpp::slotSendPackage(CIceSignalQXmppIq iq)
{
    m_Manager.sendPacket(iq);
}

int CIceSignalQxmpp::Write(const char *buf, int nLen)
{
    return 0;
}

int CIceSignalQxmpp::Read(char *buf, int nLen)
{
    return 0;
}

void CIceSignalQxmpp::slotError(QXmppClient::Error e)
{
    LOG_MODEL_ERROR("CIceSignalQxmpp", "Error:%d", e);
    emit sigError(e, "");
}
