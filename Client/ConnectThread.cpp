// Author: Kang Lin <kl222@126.com>

#include "ConnectThread.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.ConnectThread")
    
CConnectThread::CConnectThread(CConnecterDesktopThread *pConnect)
    : QThread(), // Note that the parent object pointer cannot be set here.
                 // The object is also deleted when the parent object (CConnecterDesktopThread) is destroyed.
                 // See also: CConnecterDesktopThread::Connect()
    m_pConnecter(pConnect)
{
    bool check = false;
    check = connect(this, SIGNAL(finished()),
                         this, SLOT(deleteLater()));
    Q_ASSERT(check);
}

CConnectThread::~CConnectThread()
{
    qDebug(log) << "CConnectThread::~CConnectThread";
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
    qDebug(log) << "CConnectThread::run() start";

    Q_ASSERT(m_pConnecter);
    int nRet = 0;
    CConnect* pConnect = m_pConnecter->InstanceConnect();
    if(!pConnect)
    {
        qCritical(log) << "InstanceConnect fail";
        emit m_pConnecter->sigDisconnect();
    }

    if(pConnect) {
        nRet = pConnect->Connect();
        if(nRet)
            emit pConnect->sigDisconnect();
    }

    exec();

    if(pConnect) {
        pConnect->Disconnect();
        pConnect->deleteLater();
    }

    emit m_pConnecter->sigDisconnected();

    qDebug(log) << "CConnectThread::run() end";
}
