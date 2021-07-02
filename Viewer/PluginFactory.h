// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINFACTORY_H
#define CPLUGINFACTORY_H

#pragma once

#include <QIcon>
#include <QObject>
#include "Connecter.h"

/**
 * \~english
 * \defgroup PLUGIN_API The plugin interfaces
 * \brief The plugin interfaces.
 * 
 * \~chinese
 * \defgroup PLUGIN_API 插件接口
 * \brief 插件接口.
 * \details
 * 写一个插件：
 * - 实现 CPluginFactory 。例如：\ref CPluginFactoryTigerVnc 
 *   - 在类声明中实现Qt接口:
 *     \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.h Qt plugin interface
 *   - 在构造函数中初始化操作。例如：初始化资源，加载翻译资源等
 *     \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Initialize resorce
 *   - 在析构函数中释放资源。
 *     \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Clean resource
 *   - 实现属性、函数
 *     \include Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp
 * - 实现 \ref CConnecter 。例如： \ref CConnecterTigerVnc
 *   - 实现远程桌面，可以从 \ref CConnecterPlugins 派生
 *   - 实现远程控制台，可以从 \ref CConnecterPluginsTerminal 派生
 *   - 如果上面两个不能满足你的需要，你可以直接从 \ref CConnecter
 * - 实现具体的连接，从 \ref CConnect 派生 。例如：\ref CConnectTigerVnc
 * \~ \ingroup LIBAPI
 */

/**
 * \~english \brief The plugin interface. Create CConnecter instance
 *                  be implemented by the protol plugin.
 * \~chinese \brief 插件接口。建立 CConnecter 实例，它由协议插件实现。
 * \~ \ingroup PLUGIN_API
 * \see CConnecter
 */
class RABBITREMOTECONTROL_EXPORT CPluginFactory : public QObject
{
    Q_OBJECT

public:
    /**
     * \~english
     * \brief the resources are initialized and
     *        the translation resources are loaded here. eg:
     * \note When the derived class is implemented.
     * 
     * \~chinese
     * \brief 初始化操作。例如：初始化资源，加载翻译资源等，例如：
     * \note  派生类必须实现它.
     * 
     * \~
     * \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Initialize resorce
     * \param parent
     */
    explicit CPluginFactory(QObject *parent = nullptr);
    /**
     * \~english
     * \brief When the derived class is implemented,
     *        the resources are clean and
     *        the translation resources are unloaded here. eg:
     * \~chinese 
     * \brief 派生类实现它，用于释放资源。例如：
     * 
     * \~
     * \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Clean resource
     */
    virtual ~CPluginFactory();
    
    /// \~chinese 插件ID。默认为: Protol() + ":" + Name()
    /// \~english ID. Default: Protol() + ":" + Name()
    virtual const QString Id() const;
    /// \~chinese 插件实现的协议
    /// \~english Plugin protol
    virtual const QString Protol() const = 0;
    /// \~chinese 插件名称
    /// \~english Plugin name
    virtual const QString Name() const = 0;
    /// \~chinese 插件描述
    /// \~english Plugin description
    virtual const QString Description() const = 0;
    /// \~chinese 插件图标
    virtual const QIcon Icon() const;
    
    /**
     * \~english
     * \brief New CConnecter instance. Only is called by CManageConnecter 
     * @return Return CConnecter pointer, the owner is caller
     * @note The function new object pointer,
     *       <b>the caller must delete it when don't use it</b>.
     * @param szProtol
     * \~chinese 
     * \brief 新建 CConnecter 实例。仅由 CManageConnecter 调用
     * @return 返回 CConnecter 指针, 它的所有者是调用者。
     * @note 此函数新建一个堆栈对象指针，
     *       <b>调用者必须负责在用完后释放指针</b>。
     * @param szProtol 连接协议
     * 
     * \~ \see CManageConnecter::CreateConnecter CManageConnecter::LoadConnecter
     * 
     */
    virtual CConnecter* CreateConnecter(const QString& szProtol) = 0;
};

QT_BEGIN_NAMESPACE
#define CPluginFactory_iid "KangLinStudio.Rabbit.RemoteControl.CPluginFactory"
Q_DECLARE_INTERFACE(CPluginFactory, CPluginFactory_iid)
QT_END_NAMESPACE

#endif // CPLUGINFACTORY_H
