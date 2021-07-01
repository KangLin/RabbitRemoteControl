// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#pragma once

#include <QThread>
#include "ConnecterPlugins.h"

/**
 * \~chinese 此类仅用于 CConnecterPlugins
 * \~english The class only is used by CConnecterPlugins
 * 
 * \~
 * \see CConnecterPlugins
 * \ingroup PLUGIN_API
 */
class CConnectThread : public QThread
{
    Q_OBJECT
public:
    CConnectThread(CConnecterPlugins* pConnect);
    
protected:
    virtual void run() override;
    
private:
    CConnecterPlugins* m_pConnecter;
};

#endif // CCONNECTTHREAD_H
