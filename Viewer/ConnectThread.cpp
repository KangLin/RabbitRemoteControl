// Author: Kang Lin <kl222@126.com>

#include "ConnectThread.h"
#include <QDebug>
#include <QTimer>
#include "RabbitCommonLog.h"

CConnectThread::CConnectThread(CConnecterDesktop *pConnect) : QThread(),
    m_pConnecter(pConnect)
{}

CConnectThread::~CConnectThread()
{
    qDebug() << "CConnectThread::~CConnectThread";
}

void CConnectThread::run()
{
    CConnect* pConnect = m_pConnecter->InstanceConnect();
    if(!pConnect) return;
    int nRet = pConnect->Initialize();
    if(nRet) return;

    /*
      nRet < 0 : error
      nRet = 0 : emit sigConnected
      nRet = 1 : emit sigConnected in CConnect
      */
    nRet = pConnect->Connect();
    if(nRet < 0) return;
    if(0 == nRet) emit m_pConnecter->sigConnected();
    
    QTimer::singleShot(0, pConnect, SLOT(slotTimeOut()));

    exec();

    pConnect->Disconnect();
    pConnect->Clean();

    pConnect->deleteLater();

    LOG_MODEL_DEBUG("CConnectThread", "Run end");
}
