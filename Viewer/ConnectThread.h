// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#pragma once

#include <QThread>
#include "ConnecterDesktop.h"

/*!
 * \~chinese 此类仅用于 CConnecterDesktop 。 一个线程只处理一个连接
 * \~english The class only is used by CConnecterDesktop.
 *            One thread only handles one connect
 * \~
 * \see CConnecterDesktop CConnect run()
 * \ingroup LIBAPI_THREAD
 */
class CConnectThread : public QThread
{
    Q_OBJECT
public:
    CConnectThread(CConnecterDesktop* pConnect);
    virtual ~CConnectThread() override;
    
protected:
    virtual void run() override;

protected:
    CConnecterDesktop* m_pConnecter;
};

#endif // CCONNECTTHREAD_H
