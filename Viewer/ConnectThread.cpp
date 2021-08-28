// Author: Kang Lin <kl222@126.com>

#include "ConnectThread.h"
#include "RabbitCommonLog.h"

CConnectThread::CConnectThread(CConnecterDesktop *pConnect) : QThread(),
    m_pConnecter(pConnect)
{}

CConnectThread::~CConnectThread()
{
    LOG_MODEL_DEBUG("CConnectThread", "CConnectThread::~CConnectThread");
}

void CConnectThread::run()
{
    int nRet = 0;
    CConnect* pConnect = m_pConnecter->InstanceConnect();
    if(!pConnect) return;

    /*
      nRet < 0 : error
      nRet = 0 : emit sigConnected
      nRet = 1 : emit sigConnected in CConnect
      */
    nRet = pConnect->Connect();
    if(nRet < 0)
    {
        emit m_pConnecter->sigDisconnected();
        return;
    }
    if(0 == nRet) emit m_pConnecter->sigConnected();

    exec();

    emit m_pConnecter->sigDisconnected();
    pConnect->Disconnect();

    pConnect->deleteLater();

    LOG_MODEL_DEBUG("CConnectThread", "Run end");
}
