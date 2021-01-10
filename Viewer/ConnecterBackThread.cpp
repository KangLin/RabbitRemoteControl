#include "ConnecterBackThread.h"
#include <QDebug>

CConnecterBackThread::CConnecterBackThread(CPluginFactory *parent)
    : CConnecter(parent),
      m_bExit(false)
{}

int CConnecterBackThread::OnRun()
{
    CConnect* pConnect = InstanceConnect();
        
    int nRet = 0;
    nRet = pConnect->Initialize();
    if(nRet) return nRet;

    nRet = pConnect->Connect();
    return nRet;

    while (!m_bExit) {
        int nRet = pConnect->Process();
        if(nRet) break;
    }

    emit sigDisconnected();

    pConnect->Clean();

    qDebug() << "CConnectThreadLibVnc::run() end";
}

int CConnecterBackThread::Connect()
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterBackThread::DisConnect()
{
    int nRet = 0;
    m_bExit = true;
    return nRet;
}
