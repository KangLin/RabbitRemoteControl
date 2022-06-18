#include "PluginServicThread.h"
#include "ServiceThread.h"

CPluginServicThread::CPluginServicThread(QObject *parent)
    : CPluginService(parent)
{}

CPluginServicThread::~CPluginServicThread()
{}

void CPluginServicThread::Start()
{
    if(!m_Thread)
    {
        m_Thread = QSharedPointer<CServiceThread>(new CServiceThread(this, this));
    }

    if(m_Thread)
        m_Thread->start();
}

void CPluginServicThread::Stop()
{
    if(m_Thread)
    {
        m_Thread->quit();
    }
}
