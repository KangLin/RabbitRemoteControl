// Author: Kang Lin <kl222@126.com>

#include "ServiceThread.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Service)

CServiceThread::CServiceThread(CPluginService *pPlugin, QObject *parent)
    : QThread(parent),
      m_pPlugin(pPlugin)
{   
}

CServiceThread::~CServiceThread()
{
    qDebug(Service) << "CServiceThread::~CServiceThread()";
}

void CServiceThread::run()
{
    CService* pService = m_pPlugin->NewService();
    if(!pService)
    {
        qCritical(Service) << "GetService fail";
        return;
    }
    qInfo(Service) << "The service" << m_pPlugin->Name() << "is start";

    int nRet = pService->Init();
    if(nRet)
    {
        qWarning(Service) <<  "The service" << m_pPlugin->Name() << "initial fail";
        return;
    }
    
    exec();
    
    qInfo(Service) << "The service" << m_pPlugin->Name() << "is stop";

    pService->Clean();
    pService->deleteLater();
}
