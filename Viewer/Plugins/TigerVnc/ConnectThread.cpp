#include "ConnectThread.h"
#include "ConnectTigerVnc.h"
#include "ConnecterTigerVnc.h"

#include <QDebug>

CConnectThread::CConnectThread(CFrmViewer *pView,
                               CConnecter *pConnecter,
                               QObject *parent) : QThread(parent),
    m_pView(pView),
    m_pConnecter(pConnecter)
{}

void CConnectThread::run()
{
    CConnectTigerVnc c(m_pView);

    bool check = connect(&c, SIGNAL(sigConnected()),
                         m_pConnecter, SIGNAL(sigConnected()));
    Q_ASSERT(check);
    check = connect(&c, SIGNAL(sigDisconnected()),
                    m_pConnecter, SIGNAL(sigDisconnected()));
    Q_ASSERT(check);
    
    CConnecterTigerVnc* connecter = dynamic_cast<CConnecterTigerVnc*>(m_pConnecter);
    c.SetParamter(&connecter->m_Para);
    
    c.Connect();
    c.Exec();
}
