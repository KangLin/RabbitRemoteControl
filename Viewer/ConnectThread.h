// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#pragma once

#include <QThread>
#include "ConnecterDesktop.h"

/**
 * \~chinese 此类仅用于 CConnecterPlugins
 * \~english The class only is used by CConnecterPlugins
 * 
 * \~
 * \see CConnecterPlugins
 */
class CConnectThread : public QThread
{
    Q_OBJECT
public:
    CConnectThread(CConnecterDesktop* pConnect);
    
protected:
    virtual void run() override;
    
private:
    CConnecterDesktop* m_pConnecter;
};

#endif // CCONNECTTHREAD_H
