#include "ServiceTigerVNC.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonTools.h"
#include "network/Socket.h"
#include <QHostAddress>
#include <QTcpSocket>

CServiceTigerVNC::CServiceTigerVNC(QObject *parent) : CService(parent)
{
    bool check = false;
    check = connect(&m_Lister, SIGNAL(newConnection()),
                    this, SLOT(slotNewConnection()));
    Q_ASSERT(check);
}

bool CServiceTigerVNC::Enable()
{
    return true;
}

int CServiceTigerVNC::OnInit()
{
    //TODO: Use parameters
    if(!m_Lister.listen(QHostAddress::Any, 5900))
    {
        LOG_MODEL_ERROR("ServiceTigerVNC", "Lister fail:%s",
                        m_Lister.errorString().toStdString().c_str());
        return -2;
    }
        
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
    QSharedPointer<CConnection> c(new CConnection(pSocket));
    m_lstConnection.push_back(c);
}
