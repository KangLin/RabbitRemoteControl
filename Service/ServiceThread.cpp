// Author: Kang Lin <kl222@126.com>

#include "ServiceThread.h"
#include "RabbitCommonLog.h"

CServiceThread::CServiceThread(CPluginService *pPlugin, QObject *parent)
    : QThread(parent),
      m_pPlugin(pPlugin)
{   
}

CServiceThread::~CServiceThread()
{
    LOG_MODEL_DEBUG("CServiceThread", "CServiceThread::~CServiceThread()");
}

void CServiceThread::run()
{
    CService* pService = m_pPlugin->NewService();
    if(!pService)
    {
        LOG_MODEL_ERROR("ServiceThread", "GetService fail");
        return;
    }
    
    LOG_MODEL_INFO("ServiceThread", "The service [%s] is start",
                   m_pPlugin->Name().toStdString().c_str());
    
    int nRet = pService->Init();
    if(nRet)
    {
        LOG_MODEL_WARNING("ServiceThread", "The service [%s] initial fail",
                       m_pPlugin->Name().toStdString().c_str());
        return;
    }
    
    exec();
    
    LOG_MODEL_INFO("ServiceThread", "The service [%s] is stop",
                   m_pPlugin->Name().toStdString().c_str());

    pService->Clean();
    pService->deleteLater();
}
