// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#pragma once

#include <QThread>
#include "ConnecterDesktopThread.h"

/*!
 * \~chinese 此类仅用于 CConnecterDesktopThread 。 一个线程只处理一个连接
 * \~english The class only is used by CConnecterDesktopThread.
 *            One thread only handles one connect
 * \~
 * \see CConnecterDesktopThread CConnect run()
 * \ingroup LIBAPI_THREAD
 */
class CConnectThread : public QThread
{
    Q_OBJECT
public:
    CConnectThread(CConnecterDesktopThread* pConnect);
    virtual ~CConnectThread() override;
    
protected:
    virtual void run() override;

protected:
    CConnecterDesktopThread* m_pConnecter;
};

#endif // CCONNECTTHREAD_H
