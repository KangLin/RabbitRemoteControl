// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTERCONNECT_H
#define CONNECTERCONNECT_H

#include <QMenu>

#include "Connecter.h"
#include "Connect.h"

/*!
 * \~chinese 插件的连接者接口，此接口仅由插件使用。
 *   所有的需要 CConnect 的插件连接者均需要从此接口派生。
 *   - 默认实例化 CConnect* , 并调用 CConnect::Connect() 和 CConnect::Disconnect()
 *   - 客户端插件接口由 CPluginClientThread 派生，则实现一个后台线程处理多个连接。
 *   - 客户端插件接口不是由 CPluginClientThread 派生，则实现在主线程处理一个连接。
 *
 * \~english The connector interface of the plug-in,
 *           which is only used by the plug-in.
 *           All plug-in connectors that require CConnect
 *           need to derive from this interface.
 *  - Default instance CConnect, and call CConnect::Connect()
 *    and CConnect::Disconnect()
 *  - The plugin interface is derived from CPluginClientThread,
 *    Implements a background thread to handle multiple connections.
 *  - The plugin interface is not derived from CPluginClientThread,
 *    Implements handle a connection in main thread.
 *
 * \~
 * \see CConnect
 * \ingroup CLIENT_PLUGIN_API
 */
class CLIENT_EXPORT CConnecterConnect : public CConnecter
{
    Q_OBJECT

public:
    CConnecterConnect(CPluginClient *plugin);
    virtual ~CConnecterConnect();
    virtual const QString Id() override;
    /*!
     * \~chinese
     * \brief 显示顺序：
     *        - 用户参数设置的名称
     *        - 如果允许，远程服务名。
     *        - 远程地址
     *
     * \~english
     *  Display order:
     *  - User parameter Name()
     *  - if enable, Server name
     *  - Host and port
     *
     * \~
     * \see ServerName()
     */
    virtual const QString Name() override;
    virtual const QString Description() override;

    /*!
     * \~chinese
     * 新建 CConnect 对象。它的所有者是调用者，
     * 如果调用者不再使用它，调用者必须负责释放它。
     *
     * \~english New connect. the ownership is caller.
     *        if don't use, the caller must delete it.
     */
    virtual CConnect* InstanceConnect() = 0;

    /*!
     * \brief Get parameter
     */
    virtual CParameterBase* GetParameter();
    virtual int SetParameter(CParameterBase* p);

public Q_SLOTS:
    /*!
     * \~chinese
     *  - 同步调用 CConnect::Connect() ，则在此函数中触发 sigConnected()
     *  - 异步调用 CConnect::Connect() ，则在 CConnect 中触发 sigConnected()
     *
     * \~english
     *  - Call CConnect::Connect() synchronously,
     *    then sigConnected() is triggered in this function
     *  - Asynchronous call to CConnect::Connect() triggers sigConnected() in CConnect
     */
    virtual int Connect() override;
    /*!
     * \~chinese
     *  在此函数中触发 sigDisconnected()
     * \~english
     *  emit sigDisconnected() in here
     * \~
     * \see CManageConnect::slotDisconnect()
     */
    virtual int DisConnect() override;

Q_SIGNALS:
    void sigOpenConnect(CConnecterConnect*);
    void sigCloseconnect(CConnecterConnect*);

protected:
    /*!
     * \brief Initial
     * \~
     * \see CClient::CreateConnecter
     */
    Q_INVOKABLE virtual int Initial(CParameterClient *pPara) override;
    /*!
     * \~chinese
     * \brief 当前连接名（远程桌面的名称，如果没有，则是 IP:端口）。例如：服务名或 IP:端口
     * \return 返回服务名
     *
     * \~english
     * \brief Current connect server name
     *        (remote desktop name, if not present, then IP:PORT).
     *        eg: Server name or Ip:Port
     * \return Current connect server name.
     */
    virtual QString ServerName();

private:
    /*!
     * \brief Set CParameterClient
     * \note If CParameterConnecter isn't need CParameterClient.
     *       please overload this function.
     * \see CClient::CreateConnecter CParameterConnecter CParameterClient
     */
    virtual int SetParameterClient(CParameterClient* pPara);

private Q_SLOTS:
    //! \~chinese \note 仅由 CConnectDesktop::SetConnecter() 使用
    //! \~english \note The slot only is used by CConnectDesktop::SetConnecter()
    virtual void slotSetServerName(const QString &szName);

private:
    CConnect* m_pConnect;
    QString m_szServerName;
};

#endif // CONNECTERCONNECT_H
