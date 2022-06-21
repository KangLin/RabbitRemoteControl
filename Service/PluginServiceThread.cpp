#include "PluginServiceThread.h"
#include "ServiceThread.h"

CPluginServiceThread::CPluginServiceThread(QObject *parent)
    : CPluginService(parent)
{}

CPluginServiceThread::~CPluginServiceThread()
{}

void CPluginServiceThread::Start()
{
    if(!m_Thread)
    {
        m_Thread = QSharedPointer<CServiceThread>(new CServiceThread(this, this));
    }

    if(m_Thread)
        m_Thread->start();
}

void CPluginServiceThread::Stop()
{
    if(m_Thread)
    {
        m_Thread->quit();
    }
}
