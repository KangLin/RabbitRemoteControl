//! @author: Kang Lin <kl222@126.com>

#include "ConnectThreadTerminal.h"

CConnectThreadTerminal::CConnectThreadTerminal(CConnecterPluginsTerminal *pConnecter)
    : QThread(),
      m_pConnecter(pConnecter)
{}

void CConnectThreadTerminal::run()
{
    if(m_pConnecter)
        m_pConnecter->OnRun();
}
