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

bool CServiceTigerVNC::Enable()
{
    return GetParameters()->getEnable();
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
    return 0;
}

int CServiceTigerVNC::OnProcess()
{
    LOG_MODEL_DEBUG("ServiceTigerVNC", "Process ...");
    return 1;
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
    QSharedPointer<CConnection> c(new CConnection(pSocket,
                                                  m_pPlugin->GetScreen(),
              dynamic_cast<CParameterServiceTigerVNC*>(this->GetParameters())));
    m_lstConnection.push_back(c);
}
