#include "PluginViewerThread.h"
#include "RabbitCommonLog.h"

CPluginViewerThread::CPluginViewerThread(QObject *parent) : CPluginViewer(parent),
    m_pThread(nullptr)
{
    m_pThread = new CPluginThread();
    if(m_pThread)
    {
        bool check = connect(m_pThread, SIGNAL(finished()),
                           m_pThread, SLOT(deleteLater()));
        Q_ASSERT(check);
        m_pThread->start();
    }
}

CPluginViewerThread::~CPluginViewerThread()
{
    LOG_MODEL_DEBUG("CPluginViewerThread", "CPluginViewerThread::~CPluginViewerThread");
    if(m_pThread)
        m_pThread->quit(); // The don't deleteLater().
                           // because of it is connected finished signal
}

CConnecter *CPluginViewerThread::CreateConnecter(const QString &szProtol)
{
    CConnecterDesktop* pConnecter = OnCreateConnecter(szProtol);
    if(!pConnecter) return nullptr;
    
    if(nullptr == m_pThread)
    {
        LOG_MODEL_DEBUG("CPluginViewerThread", "The thread is nullptr");
        return nullptr;
    }

    bool check = connect(pConnecter, SIGNAL(sigConnect(CConnecterDesktop*)),
                         m_pThread, SIGNAL(sigConnect(CConnecterDesktop*)));
    Q_ASSERT(check);
    check = connect(pConnecter, SIGNAL(sigDisconnect(CConnecterDesktop*)),
                    m_pThread, SIGNAL(sigDisconnect(CConnecterDesktop*)));
    Q_ASSERT(check);
    
    return pConnecter;
}
