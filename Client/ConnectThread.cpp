// Author: Kang Lin <kl222@126.com>

#include "ConnectThread.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CConnectThread::CConnectThread(CConnecterDesktopThread *pConnect)
    : QThread(), // Note that the parent object pointer cannot be set here.
                 // The object is also deleted when the parent object (CConnecterDesktopThread) is destroyed.
                 // See also: CConnecterDesktopThread::Connect()
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
    qDebug(Client) << "CConnectThread::run() start";

    Q_ASSERT(m_pConnecter);
    int nRet = 0;
    CConnect* pConnect = m_pConnecter->InstanceConnect();
    if(!pConnect)
    {
        qCritical(Client) << "InstanceConnect fail";
        emit m_pConnecter->sigDisconnect();
    }

    if(pConnect){
        nRet = pConnect->Connect();
        if(nRet)
            emit pConnect->sigDisconnect();
    }

    exec();

    if(pConnect) {
        pConnect->Disconnect();
        pConnect->deleteLater();
    } else
        emit m_pConnecter->sigDisconnected();

    qDebug(Client) << "CConnectThread::run() end";
}
