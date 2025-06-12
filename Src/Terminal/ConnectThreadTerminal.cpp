// Author: Kang Lin <kl222@126.com>

#include "ConnectThreadTerminal.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Terminal)

CConnectThreadTerminal::CConnectThreadTerminal(CConnecterTerminal *pConnecter)
    : QThread(),
      m_pConnecter(pConnecter)
{}

void CConnectThreadTerminal::run()
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
    if(nRet < 0) return;
    if(0 == nRet) emit m_pConnecter->sigConnected();

    exec();

    pConnect->Disconnect();

    pConnect->deleteLater();

    qDebug(Terminal) << "Run end";
}
