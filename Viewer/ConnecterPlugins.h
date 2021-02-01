//! @author: Kang Lin (kl222@126.com)

#ifndef CCONNECTERBACKTHREAD_H
#define CCONNECTERBACKTHREAD_H

#include "Connect.h"
#include "Parameter.h"
#include "FrmViewer.h"

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
    
    /**
     * @brief OnRun
     * @return >=0 成功
     *         < 0 错误
     */
    virtual int OnRun();
    // Instance connect
    virtual CConnect* InstanceConnect() = 0;
    
    virtual QWidget* GetViewer() override;
    virtual QString GetServerName() override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    virtual int OpenDialogSettings(QWidget *parent) override;

public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

protected:
    virtual int OnConnect();
    virtual int OnDisConnect();
    virtual int OnLoad(QDataStream& d);
    virtual int OnSave(QDataStream& d);
    /**
     * @brief GetDialogSettings
     * @param parent: the parent windows of the dialog of return
     * @return QDialog*: then QDialog must set attribute Qt::WA_DeleteOnClose;
     *         The ownership is caller.
     */
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) = 0;

private:
    bool m_bExit;
    CConnectThread* m_pThread;
    CFrmViewer *m_pView;

protected:
    CParameter* m_pParameter;
};

#endif // CCONNECTERBACKTHREAD_H
