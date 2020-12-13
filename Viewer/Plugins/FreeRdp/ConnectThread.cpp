//! @author: Kang Lin(kl222@126.com)

#include "ConnectThread.h"
#include "ConnectFreeRdp.h"
#include "ConnecterFreeRdp.h"

#include <QDebug>

CConnectThread::CConnectThread(CFrmViewer *pView,
                               CConnecter *pConnecter,
                               QObject *parent)
    : QThread(parent),
    m_bExit(false),
    m_pView(pView),
    m_pConnecter(pConnecter)
{}

CConnectThread::~CConnectThread()
{
    m_bExit = true;
    qDebug() << "CConnectThread::~CConnectThread()";
}

void CConnectThread::run()
{
    CConnecterFreeRdp* connecter = dynamic_cast<CConnecterFreeRdp*>(m_pConnecter);
    CConnectFreeRdp c( connecter);

    int nRet = c.Connect();
    
    while (!m_bExit && !nRet) {
        nRet = c.Process();
    }
    
    emit m_pConnecter->sigDisconnected();
    
    c.Clean();
    
    qDebug() << "CConnectThread::run() end";
}
