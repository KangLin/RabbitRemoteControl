//! @author: Kang Lin (kl222@126.com)

#ifndef CCONNECTERBACKTHREAD_H
#define CCONNECTERBACKTHREAD_H

#include "Connect.h"
#include "Parameter.h"
#include "FrmViewScroll.h"

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
    
    virtual QWidget* GetViewer() override;
    virtual QString GetServerName() override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    
public Q_SLOTS:
    int Connect();
    int DisConnect();

protected:
    virtual int OnConnect();
    virtual int OnDisConnect();
    virtual int OnLoad(QDataStream& d);
    virtual int OnSave(QDataStream& d);

private:
    bool m_bExit;
    CConnectThread* m_pThread;
    CFrmViewScroll *m_pView;

protected:
    CParameter* m_pParameter;
};

#endif // CCONNECTERBACKTHREAD_H
