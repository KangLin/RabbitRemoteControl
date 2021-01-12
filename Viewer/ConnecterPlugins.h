#ifndef CCONNECTERBACKTHREAD_H
#define CCONNECTERBACKTHREAD_H

#include "Connect.h"

class CConnectThread;

/**
 * @group PLUGIN_API
 * @brief The plugin API
 */

/**
 * @brief The CConnecterPlugins class. it is implement a background connect thread.
 * @note The interface only is implemented by plugin
 * @see CConnectThread
 * @addtogroup PLUGIN_API
 */
class RABBITREMOTECONTROL_EXPORT CConnecterPlugins : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterPlugins(CPluginFactory *parent = nullptr);
    virtual ~CConnecterPlugins();
    
    virtual int OnRun();
    // Instance connect
    virtual CConnect* InstanceConnect() = 0;
    
public Q_SLOTS:
    int Connect();
    int DisConnect();

    virtual int OnConnect();
    virtual int OnDisConnect();
    
private:
    bool m_bExit;
    CConnectThread* m_pThread;
};

#endif // CCONNECTERBACKTHREAD_H
