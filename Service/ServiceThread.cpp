#include "ServiceThread.h"
#include "RabbitCommonLog.h"

CServiceThread::CServiceThread(CPluginService *pPlugin, QObject *parent)
    : QThread(parent),
      m_pPlugin(pPlugin)
{   
}

void CServiceThread::run()
{
    CService* pService = m_pPlugin->NewService();
    if(!pService)
    {
        LOG_MODEL_ERROR("ServiceThread", "GetService fail");
        return;
    }
            
    int nRet = pService->Init();
    if(nRet) return;
    
    exec();
    
    LOG_MODEL_INFO("ServiceThread", "The service [%s] is stop",
                   m_pPlugin->Name().toStdString().c_str());

    pService->Clean();
    pService->deleteLater();
}
