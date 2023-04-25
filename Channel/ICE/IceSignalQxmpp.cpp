//! @author Kang Lin(kl222@126.com)

#include "IceSignalQxmpp.h"
#include "QXmppUtils.h"
#include <QCoreApplication>

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

int CIceSignalQxmpp::Open(const QString &szServer, quint16 nPort, const QString &user, const QString &password)
{
    QXmppConfiguration conf;
    
    if(QXmppUtils::jidToDomain(user).isEmpty())
    {
        QString szMsg = "The user name is error. please use format: user@domain/resource";
        qCritical(m_Log) << szMsg;
        emit sigError(-100, szMsg);
        return -1;
    }
    if(QXmppUtils::jidToResource(user).isEmpty())
    {
        qWarning(m_Log) << "The user name is error. please use format: user@domain/resource";
        conf.setResource(qApp->applicationName()
                 #if defined(Q_OS_ANDROID)
                         + "_android"
                 #elif defined(Q_OS_LINUX)
                        + "_linux"
                 #elif defined(Q_OS_WIN)
                        + "_windows"
                 #elif defined(Q_OS_OSX)
                         + "_osx"
                 #elif defined(Q_OS_IOS)
                         + "_ios"
                 #endif
                         );
    }
    
    conf.setHost(szServer);
    conf.setPort(nPort);
    conf.setJid(user);
    conf.setPassword(password);
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

bool CIceSignalQxmpp::IsConnected()
{
    return m_Client.isConnected();
}

bool CIceSignalQxmpp::proecssIq(CIceSignalQXmppIq iq)
{
    /*
    qDebug(m_Log) << "from:" << iq.from()
                   << ";to:" << iq.to()
                   << ";type:" << iq.SignalType();//*/
    if(iq.SignalType() == "offer")
    {
        /*
        qDebug(m_Log) << "type:" << iq.SignalType()
                        << "SDP:" << iq.Description();//*/
        emit sigOffer(iq.from(),
                      iq.to(),
                      iq.ChannelId(),
                      iq.SignalType(),
                      iq.Description());
    } else if(iq.SignalType() == "answer") {
        /*
        qDebug(m_Log) << "type:" << iq.SignalType()
                       << "Description:" << iq.Description();//*/
        emit sigDescription(iq.from(),
                            iq.to(),
                            iq.ChannelId(),
                            iq.SignalType(),
                            iq.Description());
    } else if (iq.SignalType()  == "candidate") {
        /*
        qDebug(m_Log) << "type:" << iq.SignalType()
                        << "Mid:" << iq.mid()
                        << "Candidate:" << iq.Candidate();//*/
        emit sigCandidate(iq.from(),
                         iq.to(),
                         iq.ChannelId(),
                         iq.mid(),
                         iq.Candidate());
    } else {
        qCritical(m_Log) << "iq type error:" << iq.SignalType();
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

int CIceSignalQxmpp::SendCandidate(const QString &toUser,
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
    iq.setCandidate(std::string(candidate).c_str());
    emit sigSendPackage(iq);
    return 0;
}

void CIceSignalQxmpp::slotSendPackage(CIceSignalQXmppIq iq)
{
    m_Manager.sendPacket(iq);
}

void CIceSignalQxmpp::slotError(QXmppClient::Error e)
{
    QString szError;
    switch (e) {
    case QXmppClient::Error::SocketError:
        szError = tr("Socket error");
        break;
    case QXmppClient::Error::KeepAliveError:
        szError = tr("Keep alive error");
        break;
    case QXmppClient::Error::XmppStreamError:
        szError = tr("xmpp stream error");
        break;
    default:
        break;
    }
    qCritical(m_Log) << "Error:" << szError;
    emit sigError(e, szError);
}
