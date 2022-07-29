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

void CManageConnect::slotConnect(CConnecterDesktop *pConnecter)
{
    int nRet = 0;
    LOG_MODEL_DEBUG("CConnecterThread", "CConnecterThread::slotConnect()");
    CConnect* pConnect = pConnecter->InstanceConnect();
    if(!pConnect) return;

    nRet = pConnect->Connect();
    if(nRet)
    {
        emit pConnecter->sigDisconnected();
        return;
    }

    m_Connects.insert(pConnecter, pConnect);
}

void CManageConnect::slotDisconnect(CConnecterDesktop *pConnecter)
{
    auto it = m_Connects.find(pConnecter);
    if(m_Connects.end() == it) return;
    CConnect* pConnect = it.value();
    m_Connects.remove(pConnecter);
    pConnect->Disconnect();
    pConnect->deleteLater();
}
