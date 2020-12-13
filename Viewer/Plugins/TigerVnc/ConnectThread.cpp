#include "ConnectThread.h"
#include "ConnectTigerVnc.h"
#include "ConnecterTigerVnc.h"

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
    CConnecterTigerVnc* connecter = dynamic_cast<CConnecterTigerVnc*>(m_pConnecter);
    CConnectTigerVnc c(connecter);
    
    c.Connect();
    
    while (!m_bExit) {
        int nRet = c.Process();
        if(nRet) break;
    }
    
    emit m_pConnecter->sigDisconnected();

    qDebug() << "CConnectThread::run() end";
}
