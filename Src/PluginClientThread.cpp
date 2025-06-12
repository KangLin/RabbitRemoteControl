#include "PluginClientThread.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Plugin.Thread")

CPluginClientThread::CPluginClientThread(QObject *parent)
    : CPluginClient(parent),
    m_pThread(nullptr)
{
    m_pThread = new CPluginThread(); // Note that the parent object pointer cannot be set here.
    if(m_pThread)
    {
        // The object is also deleted when the thread finishes executing.
        bool check = connect(m_pThread, SIGNAL(finished()),
                           m_pThread, SLOT(deleteLater()));
        Q_ASSERT(check);
        m_pThread->start();
    }
}

CPluginClientThread::~CPluginClientThread()
{
    qDebug(log) << "CPluginClientThread::~CPluginClientThread";
    if(m_pThread)
        m_pThread->quit(); // The don't deleteLater().
                           // because of it is connected finished signal
}

CConnecter *CPluginClientThread::CreateConnecter(
    const QString &szProtocol, CParameterClient *para)
{
    qDebug(log) << "CPluginClientThread::CreateConnecter()" << szProtocol;
    CConnecter* p = CPluginClient::CreateConnecter(szProtocol, para);
    if(!p) return nullptr;
    CConnecterConnect* pConnecter = qobject_cast<CConnecterConnect*>(p);
    if(!pConnecter) {
        QString szMsg = "Must be derived from CConnecterConnect*, Current is ";
        szMsg += p->metaObject()->className();
        szMsg += ";\nThe super class:\n";
        auto super = p->metaObject()->superClass();
        int n = 1;
        while(super){
            szMsg += QString(2 * n++, ' ') + "- ";
            szMsg += super->className();
            super = super->superClass();
        }
        qCritical(log) << szMsg;
        Q_ASSERT(false);
        return nullptr;
    }
    
    if(nullptr == m_pThread)
    {
        qDebug(log) << "The thread is nullptr";
        return nullptr;
    }

    bool check = connect(pConnecter, SIGNAL(sigOpenConnect(CConnecterConnect*)),
                         m_pThread, SIGNAL(sigConnect(CConnecterConnect*)));
    Q_ASSERT(check);
    check = connect(pConnecter, SIGNAL(sigCloseconnect(CConnecterConnect*)),
                    m_pThread, SIGNAL(sigDisconnect(CConnecterConnect*)));
    Q_ASSERT(check);
    
    return pConnecter;
}
