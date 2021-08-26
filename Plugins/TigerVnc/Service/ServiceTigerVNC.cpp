// Author: Kang Lin <kl222@126.com>

#include "ServiceTigerVNC.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonTools.h"
#include "network/Socket.h"
#include <QHostAddress>
#include <QTcpSocket>
#include "ParameterServiceTigerVNC.h"
#include "Connection.h"
#include "PluginService.h"

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
    LOG_MODEL_DEBUG("CServiceTigerVNC", "CServiceTigerVNC::~CServiceTigerVNC");
}

int CServiceTigerVNC::OnInit()
{
    if(!GetParameters())
        Q_ASSERT(false);
    
    if(!m_Lister.listen(QHostAddress::Any, GetParameters()->getPort()))
    {
        LOG_MODEL_ERROR("ServiceTigerVNC", "Lister fail: Port [%d]; %s",
                        GetParameters()->getPort(),
                        m_Lister.errorString().toStdString().c_str());
        return -2;
    }
    
    LOG_MODEL_INFO("ServiceTigerVNC", "Lister at: %d", GetParameters()->getPort());
    return 0;
}

int CServiceTigerVNC::OnClean()
{
    m_lstConnection.clear();
    return 0;
}

int CServiceTigerVNC::OnProcess()
{
    LOG_MODEL_DEBUG("ServiceTigerVNC", "Process ...");
    return -1;
}

void CServiceTigerVNC::slotNewConnection()
{
    if(!m_Lister.hasPendingConnections())
        return;
    QTcpSocket* pSocket = m_Lister.nextPendingConnection();
    if(!pSocket) return;
    LOG_MODEL_INFO("ServiceTigerVNC", "New connection: %s:%d",
                   pSocket->peerAddress().toString().toStdString().c_str(),
                   pSocket->peerPort());
    try {
        QSharedPointer<CChannel> channel(new CChannel());
        if(!channel->isOpen())
            if(!channel->open(pSocket, QIODevice::ReadWrite))
            {
                LOG_MODEL_ERROR("ServiceTigerVNC", "Don't open channel");
                throw std::runtime_error("Don't open channel");
            }
        QSharedPointer<CConnection> c(new CConnection(channel,
                  dynamic_cast<CParameterServiceTigerVNC*>(this->GetParameters())));
        m_lstConnection.push_back(c);
        bool check = connect(c.data(), SIGNAL(sigDisconnected()),
                             this, SLOT(slotDisconnected()));
        Q_ASSERT(check);
        check = connect(c.data(), SIGNAL(sigError(int, QString)),
                        this, SLOT(slotError(int, QString)));
        Q_ASSERT(check);
    }  catch (std::exception e) {
        LOG_MODEL_ERROR("ServiceTigerVNC", e.what());
    }  catch(...) {
        LOG_MODEL_ERROR("ServiceTigerVNC", "New connection exception");
    }
}

void CServiceTigerVNC::slotDisconnected()
{
    CConnection* pConnect = dynamic_cast<CConnection*>(sender());
    foreach(auto c, m_lstConnection)
    {
        if(c == pConnect)
        {
            m_lstConnection.removeOne(c);
        }
    }
}

void CServiceTigerVNC::slotError(int nErr, QString szErr)
{
    slotDisconnected();
}
