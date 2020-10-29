#include "ConnectThread.h"
#include "ConnectTigerVnc.h"
#include "ConnecterTigerVnc.h"

#include <QDebug>
#include <QApplication>

CConnectThread::CConnectThread(CFrmViewer *pView,
                               CConnecter *pConnecter,
                               QObject *parent) : QThread(parent),
    m_pView(pView),
    m_pConnecter(pConnecter),
    m_bExit(false)
{}

CConnectThread::~CConnectThread()
{
    m_bExit = true;
    qDebug() << "CConnectThread::~CConnectThread()";
}

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
    
    while (!m_bExit) {
        qApp->processEvents();
        int nRet = c.Process();
        if(nRet) break;
    }
    
    emit m_pConnecter->sigDisconnected();

    qDebug() << "CConnectThread::run() end";
}
