#ifndef CCONNECTERBACKTHREAD_H
#define CCONNECTERBACKTHREAD_H

#include "Connect.h"
#include "Paramter.h"

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
    
    virtual QString GetServerName() override;
    
public Q_SLOTS:
    int Connect();
    int DisConnect();

    virtual int OnConnect();
    virtual int OnDisConnect();
    
private:
    bool m_bExit;
    CConnectThread* m_pThread;
    
protected:
    CParamter* m_pParamter;
};

#endif // CCONNECTERBACKTHREAD_H
