#include "ConnectThreadLibVnc.h"
#include "ConnecterLibVnc.h"
#include "ConnectLibVnc.h"
#include <QDebug>

CConnectThreadLibVnc::CConnectThreadLibVnc(CFrmViewer *pView,
                                           CConnecter *pConnecter,
                                           QObject *parent)
    : QThread(parent),
    m_bExit(false),
    m_pView(pView),
    m_pConnecter(pConnecter)
{
}

CConnectThreadLibVnc::~CConnectThreadLibVnc()
{
    m_bExit = true;
    qDebug() << "CConnectThreadLibVnc::~CConnectThreadLibVnc()";
}

void CConnectThreadLibVnc::run()
{
    CConnecterLibVnc* connecter = dynamic_cast<CConnecterLibVnc*>(m_pConnecter);
    CConnectLibVnc c(connecter);
    
    int nRet = 0;
    nRet = c.Initialize();
    if(nRet) return;

    c.Connect();

    while (!m_bExit) {
        int nRet = c.Process();
        if(nRet) break;
    }

    emit m_pConnecter->sigDisconnected();

    c.Clean();

    qDebug() << "CConnectThreadLibVnc::run() end";
}
