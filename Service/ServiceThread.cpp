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
    
    if(!pService->Enable())
    {
        LOG_MODEL_INFO("ServiceThread", "The service [%s] is disable",
                       m_pPlugin->Name().toStdString().c_str());
        return;
    }
    pService->Init();
    
    exec();
    
    pService->Clean();
    pService->deleteLater();
}
