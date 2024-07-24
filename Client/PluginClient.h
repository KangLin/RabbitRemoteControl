// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINCLIENT_H_KL_2021_07_23
#define CPLUGINCLIENT_H_KL_2021_07_23

#pragma once

#include <QIcon>
#include "Connecter.h"
#include <QTranslator>

/*!
 * \~chinese
 * \brief 客户端插件接口。用于建立 CConnecter 实例，它由协议插件实现。
 *  - 阻塞模型：可以与 CConnecterDesktopThread 一起使用，用于连接是阻塞模型的。
 *  - 非阻塞模型：CPluginClientThread 与 CConnecterDesktop
 *    一起可实现一个后台线程处理多个远程桌面连接。(暂时未使用)
 *
 * \~english
 * \brief The plugin interface. Create CConnecter instance.
 *  - blocking model: The interface only is implemented by plug-in.
 *        It may be used with CConnecterDesktopThread for connection is blocking model.
 *  - no-blocking model: CPluginClientThread with CConnecterDesktop
 *    for a single background thread handles multiple remote desktop connections.
 *    (Not used yet)
 *
 * \~
 * \see CClient CConnecterDesktopThread CConnecter CPluginClientThread
 * \ingroup CLIENT_PLUGIN_API
 */
class CLIENT_EXPORT CPluginClient : public QObject
{
    Q_OBJECT

public:
    /*!
     * \~chinese
     * \brief 初始化操作。例如：派生类实现它，初始化插件全局资源等，例如：
     *
     * \~english
     * \brief When the derived class is implemented,
     *        The plugin global resources are initialized are loaded here. eg:
     *
     * \~
     * \snippet Plugins/FreeRDP/Client/PluginFreeRDP.cpp Initialize resource
     */
    explicit CPluginClient(QObject *parent = nullptr);
    /*!
     * \~chinese \brief 派生类实现它，用于释放资源。例如：
     * \~english
     * \brief When the derived class is implemented,
     *        the resources are clean are unloaded here. eg:
     * 
     * \~
     * \snippet Plugins/FreeRDP/Client/PluginFreeRDP.cpp Clean resource
     */
    virtual ~CPluginClient();
    
    /// \~chinese 标识。默认： Protocol() + ":" + Name()
    /// \~english ID. Default: Protocol() + ":" + Name()
    virtual const QString Id() const;
    /// \~chinese 协议
    /// \~english Plugin Protocol
    virtual const QString Protocol() const = 0;
    /// \~chinese 插件名，这个名一定要与工程名(${PROJECT_NAME})相同。
    ///           翻译文件(${PROJECT_NAME}_*.ts)）名与其相关。
    /// \~english This name must be the same as the project name (${PROJECT_NAME}).
    ///           The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
    virtual const QString Name() const = 0;
    /// \~chinese 在界面上显示的名称
    /// \~english The plugin display name
    virtual const QString DisplayName() const;
    /// \~chinese 描述
    /// \~english Plugin description.
    virtual const QString Description() const = 0;
    virtual const QIcon Icon() const;
    /*!
     * \~chinese 显示更多细节。
     *    例如： 在关于对话框或日志中显示。
     *    包括插件的依赖库的版本信息和描述
     * \~english Display more information in About dialog or log.
     *    eg: Include the plugin depends on the library version and description
     * \~
     * \see CPluginFreeRDP::Details()
     */
    virtual const QString Details() const;

private:
    /*!
     * \~chinese
     * \brief 新建 CConnecter 实例。仅由 CClient 调用
     * \return 返回 CConnecter 指针, 它的所有者是调用者。
     * \note 此函数新建一个堆栈对象指针，
     *       <b>调用者必须负责在用完后释放指针</b>。
     * \param szProtocol 连接协议
     * 
     * \~english
     * \brief New CConnecter instance. Only is called by CClient 
     * \return Return CConnecter pointer, the owner is caller
     * \note The function new object pointer,
     *       <b>the caller must delete it when don't use it</b>.
     * \param szProtocol
     * 
     * \~
     * \see CClient::CreateConnecter CClient::LoadConnecter
     * 
     */
    Q_INVOKABLE virtual CConnecter* CreateConnecter(const QString& szId) = 0;

private:
    QSharedPointer<QTranslator> m_Translator;
    /*!
     * \~chinese 初始化翻译资源，仅由 CClient 调用。
     *   因为它调用了 Name() ，所以不能在此类的构造函数中直接调用。
     *
     * \~english Initialize the translation resource,
     *   which is only called by CClient.
     *   Because it calls Name(),
     *   So it can't be called directly in the constructor of this class.
     *
     * \see CClient::AppendPlugin
     */
    Q_INVOKABLE int InitTranslator();
};

QT_BEGIN_NAMESPACE
#define CPluginClient_iid "KangLinStudio.Rabbit.RemoteControl.CPluginClient"
Q_DECLARE_INTERFACE(CPluginClient, CPluginClient_iid)
QT_END_NAMESPACE

#endif // CPLUGINCLIENT_H_KL_2021_07_23
