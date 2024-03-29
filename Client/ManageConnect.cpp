#include "ManageConnect.h"
#include "ConnecterDesktopThread.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CManageConnect::CManageConnect(QObject *parent) : QObject(parent)
{}

CManageConnect::~CManageConnect()
{
    qDebug(Client) << "CManageConnect::~CManageConnect()";
    foreach(auto pConnect, m_Connects)
    {
        pConnect->Disconnect();
        pConnect->deleteLater();
    }
}

void CManageConnect::slotConnect(CConnecterDesktop *pConnecter)
{
    int nRet = 0;
    qDebug(Client) << "CManageConnect::slotConnect()";
    CConnect* pConnect = pConnecter->InstanceConnect();
    if(!pConnect)
    {
        qCritical(Client) << "InstanceConnect fail";
        emit pConnecter->sigDisconnect();
        return;
    }

    m_Connects.insert(pConnecter, pConnect);

    nRet = pConnect->Connect();
    if(nRet)
        emit pConnecter->sigDisconnect();
}

void CManageConnect::slotDisconnect(CConnecterDesktop *pConnecter)
{
    qDebug(Client) << "CManageConnect::slotDisconnect()";
    auto it = m_Connects.find(pConnecter);
    if(m_Connects.end() == it) return;
    CConnect* pConnect = it.value();
    m_Connects.remove(pConnecter);
    pConnect->Disconnect();
    pConnect->deleteLater();
}
