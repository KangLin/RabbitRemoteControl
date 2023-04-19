// Author: Kang Lin <kl222@126.com>

#include "ConnectThread.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CConnectThread::CConnectThread(CConnecterDesktopThread *pConnect) : QThread(),
    m_pConnecter(pConnect)
{}

CConnectThread::~CConnectThread()
{
    qDebug(Client) << "CConnectThread::~CConnectThread";
}

/*!
 * \~chinese
 *   - 调用 CConnecterDesktopThread::InstanceConnect 实例化 CConnect
 *   - 调用 CConnect::Connect 开始一个连接
 *   - 进入事件循环
 * \~english
 *   - Call CConnecterDesktopThread::InstanceConnect to instantiate CConnect
 *   - Call CConnect::Connect start a connect
 *   - Enter event loop
 */
void CConnectThread::run()
{
    int nRet = 0;
    CConnect* pConnect = m_pConnecter->InstanceConnect();
    if(!pConnect) return;

    nRet = pConnect->Connect();
    if(nRet)
    {
        emit m_pConnecter->sigDisconnected();
        return;
    }

    exec();

    pConnect->Disconnect();
    pConnect->deleteLater();

    qDebug(Client) << "Run end";
}
