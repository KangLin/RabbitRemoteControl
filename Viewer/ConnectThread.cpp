// Author: Kang Lin <kl222@126.com>

#include "ConnectThread.h"

CConnectThread::CConnectThread(CConnecterPlugins *pConnect) : QThread(pConnect),
    m_pConnecter(pConnect)
{}

void CConnectThread::run()
{
    if(m_pConnecter)
        m_pConnecter->OnRun();
}
