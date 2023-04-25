#include "PluginClientThread.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CPluginClientThread::CPluginClientThread(QObject *parent) : CPluginClient(parent),
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

CPluginClientThread::~CPluginClientThread()
{
    qDebug(Client) << "CPluginClientThread::~CPluginClientThread";
    if(m_pThread)
        m_pThread->quit(); // The don't deleteLater().
                           // because of it is connected finished signal
}

CConnecter *CPluginClientThread::CreateConnecter(const QString &szProtocol)
{
    CConnecterDesktop* pConnecter = OnCreateConnecter(szProtocol);
    if(!pConnecter) return nullptr;
    
    if(nullptr == m_pThread)
    {
        qDebug(Client) << "The thread is nullptr";
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
