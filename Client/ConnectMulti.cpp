#include "ConnectMulti.h"
#include <QLoggingCategory>
#include "ConnectWakeOnLan.h"

static Q_LOGGING_CATEGORY(log, "Client.ConnectThread.Multi")

CConnectMulti::CConnectMulti(QObject *parent)
    : QObject(parent)
    , m_pCurrent(nullptr)
{}

CConnectMulti::~CConnectMulti()
{
    qDebug(log) << "CConnectMulti::~CConnectMulti";
}

int CConnectMulti::Init(CConnecterConnect *pConnecter)
{
    bool check = false;
    CParameterBase* para = pConnecter->GetParameter();
    if(!para) return -1;
    if(para->m_WakeOnLan.GetEnable())
    {
        CConnectWakeOnLan* p = new CConnectWakeOnLan(pConnecter);
        if(p) {
            m_Connects.enqueue(p);
            check = connect(p, SIGNAL(sigDisconnect()), this, SLOT(slotNext()));
            Q_ASSERT(check);
        }
    }

    CConnect* pConnect = pConnecter->InstanceConnect();
    if(pConnect)
        m_Connects.enqueue(pConnect);

    slotNext();
    return 0;
}

int CConnectMulti::Clean()
{
    qDebug(log) << "CConnectMulti::Clean()";
    if(m_pCurrent) {
        m_pCurrent->Disconnect();
        m_pCurrent->deleteLater();
        m_pCurrent = nullptr;
    }
    
    while(!m_Connects.isEmpty())
    {
        auto p = m_Connects.dequeue();
        p->Disconnect();
        p->deleteLater();
    }
    return 0;
}

void CConnectMulti::slotNext()
{
    qDebug(log) << "CConnectMulti::slotNext()";
    if(m_pCurrent) {
        m_pCurrent->Disconnect();
        m_pCurrent->deleteLater();
        m_pCurrent = nullptr;
    }

    if(!m_Connects.isEmpty()) {
        m_pCurrent = m_Connects.dequeue();
        if(m_pCurrent) {
            int nRet = m_pCurrent->Connect();
            if(nRet) {
                qCritical(log) << "m_pCurrent->Connect() fail:" << nRet;
                emit m_pCurrent->sigDisconnect();
            }
        }
    }
}
