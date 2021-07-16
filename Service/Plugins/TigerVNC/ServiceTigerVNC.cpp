#include "ServiceTigerVNC.h"
#include "RabbitCommonLog.h"
#include <unistd.h>
#include "RabbitCommonTools.h"
#include "network/Socket.h"

CServiceTigerVNC::CServiceTigerVNC(QObject *parent) : CService(parent)
{
}

bool CServiceTigerVNC::Enable()
{
    return true;
}

int CServiceTigerVNC::OnInit()
{
    QString name = RabbitCommon::CTools::GetHostName()
            + "@" + RabbitCommon::CTools::GetCurrentUser();
    m_Server = QSharedPointer<rfb::VNCServerST>(
                new rfb::VNCServerST(name.toStdString().c_str(), this));
    if(!m_Server)
    {
        LOG_MODEL_ERROR("ServiceTigerVNC", "new rfb::VNCServerST");
        return -1;
    }
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
    //LOG_MODEL_DEBUG("ServiceTigerVNC", "Process ...");
    // Client list could have been changed.
    
    std::list<network::Socket*> sockets;
    m_Server->getSockets(&sockets);

    // Nothing more to do if there are no client connections.
    if (sockets.empty())
      return 0;

    // Process events on existing VNC connections
    for (i = sockets.begin(); i != sockets.end(); i++) {
      
        server.processSocketReadEvent(*i);
      
        server.processSocketWriteEvent(*i);
    }
    return 0;
}
