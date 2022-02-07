#ifndef CPLUGINVIEWERTHREAD_H
#define CPLUGINVIEWERTHREAD_H

#include "PluginViewer.h"
#include "ConnecterDesktop.h"
#include "PluginThread.h"

/**
 * \~chinese
 * \brief 它实现一个后台线程处理多个远程桌面连接。
 * \note 此接口仅由插件实现。
 *       连接者必须从 CConnecterDesktop 派生。
 * 
 * \~english
 * \brief It implements a background thread to handle
 *        multiple remote desktop connections.
 * \note The interface only is implemented by plug-in.
 *       The connecter must be derived from CConnecterDesktop 
 * \~  
 * \see CConnecterDesktop CPluginThread CManageConnect 
 */
class VIEWER_EXPORT CPluginViewerThread : public CPluginViewer
{
    Q_OBJECT
public:
    explicit CPluginViewerThread(QObject *parent = nullptr);
    virtual ~CPluginViewerThread();

    virtual CConnecter* CreateConnecter(const QString &szProtol) override;
    
protected:
    /*!
     * \brief Create Connecter
     * \param szProtol
     * \return CConnecterDesktop*
     */
    virtual CConnecterDesktop* OnCreateConnecter(const QString &szProtol) = 0;
    
private:
    CPluginThread* m_pThread;
    
};

#endif // CPLUGINVIEWERTHREAD_H
