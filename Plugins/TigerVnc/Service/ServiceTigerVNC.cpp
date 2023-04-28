// Author: Kang Lin <kl222@126.com>

#include "ServiceTigerVNC.h"
#include "RabbitCommonTools.h"
#include "network/Socket.h"
#include <QHostAddress>
#include <QTcpSocket>
#include "ParameterServiceTigerVNC.h"
#include "Connection.h"
#include "PluginService.h"
#include <stdexcept>

#ifdef HAVE_GUI
#include "FrmParameterTigerVNC.h"
#endif

#ifdef HAVE_ICE
    #include "Ice.h"
    #include "ICE/ChannelIce.h"
#endif

Q_DECLARE_LOGGING_CATEGORY(TigerVNC)
CServiceTigerVNC::CServiceTigerVNC(CPluginService *plugin) : CService(plugin)
{
    bool check = false;
    check = connect(&m_Lister, SIGNAL(newConnection()),
                    this, SLOT(slotNewConnection()));
    Q_ASSERT(check);
    
    m_pPara = new CParameterServiceTigerVNC(this);
}

CServiceTigerVNC::~CServiceTigerVNC()
{
    qDebug(TigerVNC) << "CServiceTigerVNC::~CServiceTigerVNC";
}

#ifdef HAVE_GUI
QWidget* CServiceTigerVNC::GetParameterWidget(QWidget* parent)
{
    return new CFrmParameterTigerVNC(
                dynamic_cast<CParameterServiceTigerVNC*>(GetParameters()), parent);
}
#endif

int CServiceTigerVNC::OnInit()
{
    bool check = false;
    CParameterServiceTigerVNC* p =
            dynamic_cast<CParameterServiceTigerVNC*>(GetParameters());
    if(!p)
        Q_ASSERT(false);
    
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
            qInfo(TigerVNC) << "Connect ICE signal";
        }
    }
#endif
    
    if(p->GetEnableSocket())
    {
        if(!m_Lister.listen(QHostAddress::Any, p->getPort()))
        {
            qCritical(TigerVNC, "Lister fail: Port [%d]; %s",
                            GetParameters()->getPort(),
                            m_Lister.errorString().toStdString().c_str());
            return -1;
        }
        qInfo(TigerVNC) << "Lister at:" << p->getPort();
    }

    return 1; //Don't use OnProcess (qt event loop)
}

int CServiceTigerVNC::OnClean()
{
#if defined(HAVE_ICE)
    CParameterServiceTigerVNC* p =
            dynamic_cast<CParameterServiceTigerVNC*>(GetParameters());
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

void CServiceTigerVNC::slotNewConnection()
{
    if(!m_Lister.hasPendingConnections())
        return;
    QTcpSocket* pSocket = m_Lister.nextPendingConnection();
    if(!pSocket) return;
    qInfo(TigerVNC) << "New connection:"
                    << pSocket->peerAddress() << ":" << pSocket->peerPort();
    try {
        QSharedPointer<CChannel> channel(new CChannel(), &QObject::deleteLater);
        if(!channel->isOpen())
            if(!channel->open(pSocket, QIODevice::ReadWrite))
            {
                qCritical(TigerVNC) << "Don't open channel";
                throw std::runtime_error("Don't open channel");
            }
        QSharedPointer<CConnection> c(new CConnection(channel,
                  dynamic_cast<CParameterServiceTigerVNC*>(this->GetParameters())),
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
        qCritical(TigerVNC) << "New connection exception" << e.what();
    }  catch(...) {
        qCritical(TigerVNC) << "New connection exception";
    }
}

void CServiceTigerVNC::slotDisconnected()
{
    CConnection* pConnect = dynamic_cast<CConnection*>(sender());
    pConnect->close(tr("Exit").toStdString().c_str());
    foreach(auto c, m_lstConnection)
        if(c == pConnect)
            m_lstConnection.removeOne(c);
}

void CServiceTigerVNC::slotError(int nErr, QString szErr)
{
    slotDisconnected();
}

#ifdef HAVE_ICE
void CServiceTigerVNC::slotSignalConnected()
{
    CParameterICE* p = CICE::Instance()->GetParameter();
    if(!p) return;
    qInfo(TigerVNC, "Connected to signal server: %s:%d; user:%s",
                   p->getSignalServer().toStdString().c_str(),
                   p->getSignalPort(),
                   p->getSignalUser().toStdString().c_str());
}

void CServiceTigerVNC::slotSignalDisConnected()
{
    CParameterICE* p = CICE::Instance()->GetParameter();
    if(!p) return;
    qInfo(TigerVNC, "Disconnect signal server: %s:%d; user:%s",
                   p->getSignalServer().toStdString().c_str(),
                   p->getSignalPort(),
                   p->getSignalUser().toStdString().c_str());
}

void CServiceTigerVNC::slotSignalError(int nErr, const QString& szErr)
{
    CParameterICE* p = CICE::Instance()->GetParameter();
    if(!p) return;
    qCritical(TigerVNC, "signal: %s:%d; user:%s; error: %d: %s",
                    p->getSignalServer().toStdString().c_str(),
                    p->getSignalPort(),
                    p->getSignalUser().toStdString().c_str(),
                    nErr, szErr.toStdString().c_str());
}

void CServiceTigerVNC::slotSignalOffer(const QString& fromUser,
                                       const QString& toUser,
                                       const QString& channelId,
                                       const QString& type,
                                       const QString& sdp)
{
    try {
        qInfo(TigerVNC,
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
                qCritical(TigerVNC) << "Don't open channel";
                throw std::runtime_error("Don't open channel");
            }
        }
        QSharedPointer<CConnection> c(new CConnection(channel,
              dynamic_cast<CParameterServiceTigerVNC*>(this->GetParameters())),
                                       &QObject::deleteLater);
        m_lstConnection.push_back(c);
        bool check = connect(c.data(), SIGNAL(sigDisconnected()),
                             this, SLOT(slotDisconnected()));
        Q_ASSERT(check);
        check = connect(c.data(), SIGNAL(sigError(int, QString)),
                        this, SLOT(slotError(int, QString)));
        Q_ASSERT(check);
    }  catch (std::exception e) {
        qCritical(TigerVNC) << "New connection exception" << e.what();
    }  catch(...) {
        qCritical(TigerVNC) << "New connection exception";
    }
}
#endif //HAVE_ICE
 
