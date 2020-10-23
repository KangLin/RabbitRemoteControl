#include "ConnecterTigerVnc.h"

CConnecterTigerVnc::CConnecterTigerVnc(QObject *parent)
    : CConnecter(parent),
      m_pThread(nullptr)
{    
    m_pView = new CFrmViewer();
}

CFrmViewer* CConnecterTigerVnc::GetViewer()
{
    return m_pView;
}

QWidget* CConnecterTigerVnc::GetDialogSettings()
{
    return nullptr;
}

int CConnecterTigerVnc::Connect()
{
    if(nullptr == m_pThread)
    {
        m_pThread = new CConnectThread(m_pView);
        bool check = connect(m_pThread, SIGNAL(destroyed()),
                           m_pThread, SLOT(deleteLater()));
        Q_ASSERT(check);
    }
    m_pThread->start();
    return 0;
}

int CConnecterTigerVnc::DisConnect()
{
    m_pThread->quit();
    return 0;
}
