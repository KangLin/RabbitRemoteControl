//! @author: Kang Lin (kl222@126.com)

#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#include <QThread>
#include "ConnecterPlugins.h"

/**
 * @brief The CConnectThread class
 * @see CConnecterPlugins
 * @note The class only is used by CConnecterPlugins
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
