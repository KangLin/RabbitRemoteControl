#include "ManageConnect.h"
#include "RabbitCommonLog.h"
#include "ConnecterDesktopThread.h"

CManageConnect::CManageConnect(QObject *parent) : QObject(parent)
{
}

CManageConnect::~CManageConnect()
{
    foreach(auto pConnect, m_Connects)
    {
        pConnect->Disconnect();
        pConnect->deleteLater();
    }
}

void CManageConnect::slotConnect(CConnecter *pConnecter)
{
    int nRet = 0;
    LOG_MODEL_DEBUG("CConnecterThread", "CConnecterThread::slotConnect()");
    CConnect* pConnect = dynamic_cast<CConnecterDesktopThread*>(pConnecter)->InstanceConnect();
    if(!pConnect) return;

    /*
      nRet < 0 : error
      nRet = 0 : emit sigConnected by caller
      nRet = 1 : emit sigConnected in CConnect
      */
    nRet = pConnect->Connect();
    if(nRet < 0) return;
    if(0 == nRet) emit pConnect->sigConnected();

    m_Connects.insert(pConnecter, pConnect);
}

void CManageConnect::slotDisconnect(CConnecter *pConnecter)
{
    auto it = m_Connects.find(pConnecter);
    if(m_Connects.end() == it) return;
    CConnect* pConnect = it.value();
    pConnect->Disconnect();
    pConnect->deleteLater();
    m_Connects.remove(pConnecter);
}
