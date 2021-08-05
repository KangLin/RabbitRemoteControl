// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#pragma once

#include <QThread>
#include "ConnecterDesktop.h"

///
/// \~chinese 此类仅用于 CConnecterDesktop
///   一个线程一个连接
/// \~english The class only is used by CConnecterDesktop
///   One thread one connect
///
/// \~
/// \see CConnecterDesktop
///
class CConnectThread : public QThread
{
    Q_OBJECT
public:
    CConnectThread(CConnecterDesktop* pConnect);
    virtual ~CConnectThread();
    
protected:
    virtual void run() override;

protected:
    CConnecterDesktop* m_pConnecter;
};

#endif // CCONNECTTHREAD_H
