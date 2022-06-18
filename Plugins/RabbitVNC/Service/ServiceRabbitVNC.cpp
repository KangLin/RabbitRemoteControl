// Author: Kang Lin <kl222@126.com>

#include "ServiceRabbitVNC.h"
#include "RabbitCommonLog.h"
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

CServiceRabbitVNC::CServiceRabbitVNC(CPluginService *plugin) : CService(plugin)
{
    bool check = false;
    check = connect(&m_Lister, SIGNAL(newConnection()),
                    this, SLOT(slotNewConnection()));
    Q_ASSERT(check);
    
    m_pPara = new CParameterServiceRabbitVNC(this);

#if defined(HAVE_ICE)
    m_Signal = CICE::Instance()->GetSignal();
    if(m_Signal)
    {
        check = connect(m_Signal.data(), SIGNAL(sigConnected()),
                        this, SLOT(slotSignalConnected()));
        Q_ASSERT(check);
        check = connect(m_Signal.data(), SIGNAL(sigDisconnected()),
                        this, SLOT(slotSignalDisConnected()));
        Q_ASSERT(check);
        check = connect(m_Signal.data(), SIGNAL(sigError(int, const QString&)),
                        this, SLOT(slotSignalError(int, const QString&)));
        Q_ASSERT(check);
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
    }
#endif
}

CServiceRabbitVNC::~CServiceRabbitVNC()
{
    LOG_MODEL_DEBUG("CServiceRabbitVNC", "CServiceRabbitVNC::~CServiceRabbitVNC");
}

#ifdef HAVE_GUI
QWidget* CServiceRabbitVNC::GetParameterWidget(void *p)
{
    return new CFrmParameterRabbitVNC(
                dynamic_cast<CParameterServiceRabbitVNC*>(GetParameters()));
}
#endif

int CServiceRabbitVNC::OnInit()
{
    int nRet = 0;
    CParameterServiceRabbitVNC* p =
            dynamic_cast<CParameterServiceRabbitVNC*>(GetParameters());
    if(!p)
        Q_ASSERT(false);
    
    if(!m_Lister.listen(QHostAddress::Any, p->getPort()))
    {
        LOG_MODEL_ERROR("ServiceRabbitVNC", "Lister fail: Port [%d]; %s",
                        GetParameters()->getPort(),
                        m_Lister.errorString().toStdString().c_str());
        return -1;
    }
    LOG_MODEL_INFO("ServiceRabbitVNC", "Lister at: %d", p->getPort());
    
#ifdef HAVE_ICE
    if(m_Signal && p->getIce())
    {
        nRet = m_Signal->Open(p->getSignalServer(), p->getSignalPort(),
                              p->getSignalUser(), p->getSignalPassword());
        if(nRet) return -2;
    }
#endif
    return 1; // Don't use OnProcess (qt event loop)
}

int CServiceRabbitVNC::OnClean()
{
    m_Lister.close();
#ifdef HAVE_ICE
    if(m_Signal)
    {
        m_Signal->Close();
    }
#endif
    m_lstConnection.clear();
    return 0;
}

void CServiceRabbitVNC::slotNewConnection()
{
    if(!m_Lister.hasPendingConnections())
        return;
    QTcpSocket* pSocket = m_Lister.nextPendingConnection();
    if(!pSocket) return;
    LOG_MODEL_INFO("ServiceRabbitVNC", "New connection: %s:%d",
                   pSocket->peerAddress().toString().toStdString().c_str(),
                   pSocket->peerPort());
    try {
        QSharedPointer<CChannel> channel(new CChannel(), &QObject::deleteLater);
        if(!channel->isOpen())
            if(!channel->open(pSocket, QIODevice::ReadWrite))
            {
                LOG_MODEL_ERROR("ServiceRabbitVNC", "Don't open channel");
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
        LOG_MODEL_ERROR("ServiceRabbitVNC", e.what());
    }  catch(...) {
        LOG_MODEL_ERROR("ServiceRabbitVNC", "New connection exception");
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
    CParameterServiceRabbitVNC* p =
            dynamic_cast<CParameterServiceRabbitVNC*>(GetParameters());
    if(!p) return;
    LOG_MODEL_INFO("ServiceRabbitVNC", "Connected to signal server: %s:%d; user:%s",
                   p->getSignalServer().toStdString().c_str(),
                   p->getSignalPort(),
                   p->getSignalUser().toStdString().c_str());
}

void CServiceRabbitVNC::slotSignalDisConnected()
{
    CParameterServiceRabbitVNC* p =
            dynamic_cast<CParameterServiceRabbitVNC*>(GetParameters());
    if(!p) return;
    LOG_MODEL_INFO("ServiceRabbitVNC", "Disconnect signal server: %s:%d; user:%s",
                   p->getSignalServer().toStdString().c_str(),
                   p->getSignalPort(),
                   p->getSignalUser().toStdString().c_str());
}

void CServiceRabbitVNC::slotSignalError(int nErr, const QString& szErr)
{
    CParameterServiceRabbitVNC* p =
            dynamic_cast<CParameterServiceRabbitVNC*>(GetParameters());
    if(!p) return;
    LOG_MODEL_ERROR("ServiceRabbitVNC", "signal: %s:%d; user:%s; error: %d: %s",
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
        LOG_MODEL_INFO("ServiceRabbitVNC",
                       "New connection: from:%s; to:%s; channelId:%s",
                       fromUser.toStdString().c_str(),
                       toUser.toStdString().c_str(),
                       channelId.toStdString().c_str());
        CParameterServiceRabbitVNC* p =
                dynamic_cast<CParameterServiceRabbitVNC*>(GetParameters());
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
                LOG_MODEL_ERROR("ServiceRabbitVNC", "Don't open channel");
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
        LOG_MODEL_ERROR("ServiceRabbitVNC", e.what());
    }  catch(...) {
        LOG_MODEL_ERROR("ServiceRabbitVNC", "New connection exception");
    }
}
#endif //HAVE_ICE
 
