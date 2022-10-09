// Author: Kang Lin <kl222@126.com>

#include "ServiceRabbitVNC.h"
#include "RabbitCommonTools.h"
#include "network/Socket.h"
#include <stdexcept>
#include <QHostAddress>
#include <QTcpSocket>
#include "ParameterServiceRabbitVNC.h"
#ifdef HAVE_GUI
    #include "FrmParameterRabbitVNC.h"
#endif
#include "Connection.h"
#include "PluginService.h"

#ifdef HAVE_ICE
#include "ICE/ChannelIce.h"
#include "Ice.h"
#endif

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(RabbitVNC)

CServiceRabbitVNC::CServiceRabbitVNC(CPluginService *plugin) : CService(plugin)
{
    bool check = false;
    check = connect(&m_Lister, SIGNAL(newConnection()),
                    this, SLOT(slotNewConnection()));
    Q_ASSERT(check);
    
    m_pPara = new CParameterServiceRabbitVNC(this);
}

CServiceRabbitVNC::~CServiceRabbitVNC()
{
    qDebug(RabbitVNC) << "CServiceRabbitVNC::~CServiceRabbitVNC";
}

#ifdef HAVE_GUI
QWidget* CServiceRabbitVNC::GetParameterWidget(QWidget *parent)
{
    return new CFrmParameterRabbitVNC(
                dynamic_cast<CParameterServiceRabbitVNC*>(GetParameters()), parent);
}
#endif

int CServiceRabbitVNC::OnInit()
{
    bool check = false;
    CParameterServiceRabbitVNC* p =
            dynamic_cast<CParameterServiceRabbitVNC*>(GetParameters());
    if(!p)
        Q_ASSERT(false);
    
    if(p->GetEnableSocket())
    {
        if(!m_Lister.listen(QHostAddress::Any, p->getPort()))
        {
            qCritical(RabbitVNC) << "Lister fail: Port" << GetParameters()->getPort()
                      << "; %s" << m_Lister.errorString().toStdString().c_str();
            return -1;
        }
        qInfo(RabbitVNC) << "Lister at:", p->getPort();
    }

#if defined(HAVE_ICE)
    if(p->getIce())
    {
        m_Signal = CICE::Instance()->GetSignal();
        if(m_Signal)
        {
            //        check = connect(m_Signal.data(), SIGNAL(sigConnected()),
            //                        this, SLOT(slotSignalConnected()));
            //        Q_ASSERT(check);
            //        check = connect(m_Signal.data(), SIGNAL(sigDisconnected()),
            //                        this, SLOT(slotSignalDisConnected()));
            //        Q_ASSERT(check);
            //        check = connect(m_Signal.data(), SIGNAL(sigError(int, const QString&)),
            //                        this, SLOT(slotSignalError(int, const QString&)));
            //        Q_ASSERT(check);
            check = connect(m_Signal.data(), SIGNAL(sigOffer(const QString&,
                                                             const QString&,
                                                             const QString&,
                                                             const QString&,
                                                             const QString&)),
                            this, SLOT(slotSignalOffer(const QString&,
                                                       const QString&,
                                                       const QString&,
                                                       const QString&,
                                                       const QString&)));
            Q_ASSERT(check);
            qInfo(RabbitVNC) << "Connect ICE signal";
        }
    }
#endif
    
    return 1; // Don't use OnProcess (qt event loop)
}

int CServiceRabbitVNC::OnClean()
{
#if defined(HAVE_ICE)
    CParameterServiceRabbitVNC* p =
            dynamic_cast<CParameterServiceRabbitVNC*>(GetParameters());
    if(p)
    {
        m_Signal = CICE::Instance()->GetSignal();
        if(m_Signal)
            m_Signal->disconnect(this);
    }
#endif

    m_Lister.close();
    m_lstConnection.clear();
    return 0;
}

void CServiceRabbitVNC::slotNewConnection()
{
    if(!m_Lister.hasPendingConnections())
        return;
    QTcpSocket* pSocket = m_Lister.nextPendingConnection();
    if(!pSocket) return;
    qInfo(RabbitVNC) << "New connection:"
                   << pSocket->peerAddress() << ":" << pSocket->peerPort();
    try {
        QSharedPointer<CChannel> channel(new CChannel(), &QObject::deleteLater);
        if(!channel->isOpen())
            if(!channel->open(pSocket, QIODevice::ReadWrite))
            {
                qCritical(RabbitVNC) << "Don't open channel";
                throw std::runtime_error("Don't open channel");
            }
        QSharedPointer<CConnection> c(new CConnection(channel,
                  dynamic_cast<CParameterServiceRabbitVNC*>(this->GetParameters())),
                                      &QObject::deleteLater);
        m_lstConnection.push_back(c);
        bool check = connect(c.data(), SIGNAL(sigDisconnected()),
                             this, SLOT(slotDisconnected()));
        Q_ASSERT(check);
        check = connect(c.data(), SIGNAL(sigError(int, QString)),
                        this, SLOT(slotError(int, QString)));
        Q_ASSERT(check);
        // Because the socket is connected, so emit sigConnected()
        emit channel->sigConnected();
    }  catch (std::exception e) {
        qCritical(RabbitVNC) << "New connection exception:" << e.what();
    }  catch(...) {
        qCritical(RabbitVNC) << "New connection exception";
    }
}

void CServiceRabbitVNC::slotDisconnected()
{
    CConnection* pConnect = dynamic_cast<CConnection*>(sender());
    pConnect->close(tr("Exit").toStdString().c_str());
    foreach(auto c, m_lstConnection)
        if(c == pConnect)
            m_lstConnection.removeOne(c);
}

void CServiceRabbitVNC::slotError(int nErr, QString szErr)
{
    slotDisconnected();
}

#ifdef HAVE_ICE
void CServiceRabbitVNC::slotSignalConnected()
{
    CParameterICE* p = CICE::Instance()->GetParameter();
    if(!p) return;
    qInfo(RabbitVNC, "Connected to signal server: %s:%d; user:%s",
                   p->getSignalServer().toStdString().c_str(),
                   p->getSignalPort(),
                   p->getSignalUser().toStdString().c_str());
}

void CServiceRabbitVNC::slotSignalDisConnected()
{
    CParameterICE* p = CICE::Instance()->GetParameter();
    if(!p) return;
    qInfo(RabbitVNC, "Disconnect signal server: %s:%d; user:%s",
                   p->getSignalServer().toStdString().c_str(),
                   p->getSignalPort(),
                   p->getSignalUser().toStdString().c_str());
}

void CServiceRabbitVNC::slotSignalError(int nErr, const QString& szErr)
{
    CParameterICE* p = CICE::Instance()->GetParameter();
    if(!p) return;
    qCritical(RabbitVNC, "signal: %s:%d; user:%s; error: %d: %s",
                    p->getSignalServer().toStdString().c_str(),
                    p->getSignalPort(),
                    p->getSignalUser().toStdString().c_str(),
                    nErr, szErr.toStdString().c_str());
}

void CServiceRabbitVNC::slotSignalOffer(const QString& fromUser,
                                       const QString& toUser,
                                       const QString& channelId,
                                       const QString& type,
                                       const QString& sdp)
{
    try {
        qInfo(RabbitVNC,
                       "New connection: from:%s; to:%s; channelId:%s",
                       fromUser.toStdString().c_str(),
                       toUser.toStdString().c_str(),
                       channelId.toStdString().c_str());
        CParameterICE* p = CICE::Instance()->GetParameter();
        if(!p) return;
        QSharedPointer<CChannelIce> channel(new CChannelIce(m_Signal.data()),
                                                &QObject::deleteLater);
        if(!channel->isOpen())
        {
            rtc::IceServer stun(p->getStunServer().toStdString().c_str(),
                                p->getStunPort());
            rtc::IceServer turn(p->getTurnServer().toStdString().c_str(),
                                p->getTurnPort(),
                                p->getTurnUser().toStdString().c_str(),
                                p->getTurnPassword().toStdString().c_str());
            rtc::Configuration config;
            config.iceServers.push_back(stun);
            config.iceServers.push_back(turn);
            channel->SetConfigure(config);
            if(!channel->open(fromUser, toUser, channelId, type, sdp))
            {
                qCritical(RabbitVNC) << "Don't open channel";
                throw std::runtime_error("Don't open channel");
            }
        }
        QSharedPointer<CConnection> c(new CConnection(channel,
              dynamic_cast<CParameterServiceRabbitVNC*>(this->GetParameters())),
                                       &QObject::deleteLater);
        m_lstConnection.push_back(c);
        bool check = connect(c.data(), SIGNAL(sigDisconnected()),
                             this, SLOT(slotDisconnected()));
        Q_ASSERT(check);
        check = connect(c.data(), SIGNAL(sigError(int, QString)),
                        this, SLOT(slotError(int, QString)));
        Q_ASSERT(check);
    }  catch (std::exception e) {
        qCritical(RabbitVNC) << "New connection exception" << e.what();
    }  catch(...) {
        qCritical(RabbitVNC) << "New connection exception";
    }
}
#endif //HAVE_ICE
 
