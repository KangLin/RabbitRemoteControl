// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#pragma once

#include <QThread>
#include "ConnecterThread.h"

/*!
 * \~chinese 此类仅用于 CConnecterThread 。 一个线程只处理一个连接
 * \note 当此线程退出后，会自动删除线程对象。
 *
 * \~english The class only is used by CConnecterThread.
 *            One thread only handles one connect
 * \note When the thread finished, the thread will be deleted.
 * \~
 * \see CConnecterThread CConnect run()
 * \ingroup LIBAPI_THREAD
 */
class CConnectThread : public QThread
{
    Q_OBJECT
public:
    CConnectThread(CConnecterThread* pConnect);
    virtual ~CConnectThread() override;
    
protected:
    virtual void run() override;

protected:
    CConnecterThread* m_pConnecter;
};

#endif // CCONNECTTHREAD_H
