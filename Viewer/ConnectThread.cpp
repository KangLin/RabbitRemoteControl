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

/*!
 * \~chinese
 *   - 调用 CConnecterDesktop::InstanceConnect 实例化 CConnect
 *   - 调用 CConnect::Connect 开始一个连接
 *   - 进入事件循环
 * \~english
 *   - Call CConnecterDesktop::InstanceConnect to instantiate CConnect
 *   - Call CConnect::Connect start a connect
 *   - Enter event loop
 */
void CConnectThread::run()
{
    int nRet = 0;
    CConnect* pConnect = m_pConnecter->InstanceConnect();
    if(!pConnect) return;

    /*
      nRet < 0 : error
      nRet = 0 : emit sigConnected by caller
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
