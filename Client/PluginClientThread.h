#ifndef CPLUGINVIEWERTHREAD_H
#define CPLUGINVIEWERTHREAD_H

#include "PluginClient.h"
#include "ConnecterDesktop.h"
#include "PluginThread.h"

/**
 * \~chinese
 * \brief 它默认启动一个后台线程。
 *        它与 CConnecterDesktop 一起可实现一个后台线程处理多个远程桌面连接。
 * \image html docs/Image/PluginClientNoBlockSequenDiagram.svg
 * \note 此接口仅由插件实现。
 *       连接者必须从 CConnecterDesktop 派生。
 * 
 * \~english
 * \brief It starts a background thread by default.
 *        Together with CConnecterDesktop, it enables a background thread to
 *        handle multiple remote desktop connections.
 * \image html docs/Image/PluginClientNoBlockSequenDiagram.svg
 * \note The interface only is implemented by plug-in.
 *       The connecter must be derived from CConnecterDesktop 
 * \~
 * \see CConnecterDesktop CPluginThread CManageConnect
 */
class CLIENT_EXPORT CPluginClientThread : public CPluginClient
{
    Q_OBJECT
public:
    explicit CPluginClientThread(QObject *parent = nullptr);
    virtual ~CPluginClientThread();
private:
    virtual CConnecter* CreateConnecter(const QString &szProtocol) override;
    
protected:
    /*!
     * \brief Create Connecter
     * \param szProtocol
     * \return CConnecterDesktop*
     */
    virtual CConnecterDesktop* OnCreateConnecter(const QString &szProtocol) = 0;
    
private:
    CPluginThread* m_pThread;
    
};

#endif // CPLUGINVIEWERTHREAD_H
