#include "PluginService.h"
#include <QIcon>
#include "ServiceThread.h"
#include "RabbitCommonLog.h"

CPluginService::CPluginService(QObject *parent)
    : QObject(parent),
    m_pThread(nullptr)
{
}

CPluginService::~CPluginService()
{
}

const QString CPluginService::Id() const
{
    return Protol() + "_" + Name();
}

const QIcon CPluginService::Icon() const
{
    return QIcon(":/image/Connect");
}

void CPluginService::Start()
{
    if(!m_pThread)
    {
        m_pThread = new CServiceThread(this, this);    
    }

    if(m_pThread)
        m_pThread->start();
}

void CPluginService::Stop()
{
    if(m_pThread)
    {
        m_pThread->quit();
        m_pThread = nullptr;
    }
}
