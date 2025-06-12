#include "ManageConnect.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.MangeConnect")

CManageConnect::CManageConnect(QObject *parent) : QObject(parent)
{}

CManageConnect::~CManageConnect()
{
    qDebug(log) << "CManageConnect::~CManageConnect()";
    foreach(auto pConnect, m_Connects)
    {
        pConnect->Disconnect();
        pConnect->deleteLater();
    }
}

void CManageConnect::slotConnect(CConnecterConnect *pConnecter)
{
    int nRet = 0;
    qDebug(log) << "CManageConnect::slotConnect()";
    CConnect* pConnect = pConnecter->InstanceConnect();
    if(!pConnect)
    {
        qCritical(log) << "InstanceConnect fail";
        emit pConnecter->sigDisconnect();
        return;
    }

    m_Connects.insert(pConnecter, pConnect);

    nRet = pConnect->Connect();
    if(nRet)
        emit pConnecter->sigDisconnect();
}

void CManageConnect::slotDisconnect(CConnecterConnect *pConnecter)
{
    qDebug(log) << "CManageConnect::slotDisconnect()";
    auto it = m_Connects.find(pConnecter);
    if(m_Connects.end() == it) return;
    CConnect* pConnect = it.value();
    m_Connects.remove(pConnecter);
    pConnect->Disconnect();
    pConnect->deleteLater();
    emit pConnecter->sigDisconnected();
}
