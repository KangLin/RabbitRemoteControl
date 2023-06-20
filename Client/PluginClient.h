// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINVIEWER_H_KL_2021_07_23
#define CPLUGINVIEWER_H_KL_2021_07_23

#pragma once

#include <QIcon>
#include <QObject>
#include "Connecter.h"
#include <QTranslator>
#include "PluginThread.h"

/**
 * \~chinese
 * \brief 客户端插件接口。建立 CConnecter 实例，它由协议插件实现。
 *        可以与 CConnecterDesktopThread 一起使用，用于连接是阻塞模型的。
 *
 * \~english
 * \brief The plugin interface. Create CConnecter instance.
 *        The interface only is implemented by plug-in.
 *        It may be used with CConnecterDesktopThread for connection is blocking model.
 * \~
 * \see CClient CConnecterDesktopThread CConnecter
 * \ingroup CLIENT_PLUGIN_API
 */
class CLIENT_EXPORT CPluginClient : public QObject
{
    Q_OBJECT

public:
    /**
     * \~chinese
     * \brief 初始化操作。例如：初始化资源等，例如：
     * \snippet Plugins/TigerVnc/Client/PluginTigerVnc.cpp Initialize resource
     * \note  派生类必须实现它. 
     * 
     * \~english
     * \brief The resources are initialized are loaded here. eg:
     * \snippet Plugins/TigerVnc/Client/PluginTigerVnc.cpp Initialize resource
     * \note When the derived class is implemented.
     * 
     * \~
     * \param parent
     */
    explicit CPluginClient(QObject *parent = nullptr);
    /**
     * \~chinese \brief 派生类实现它，用于释放资源。例如：
     * \~english
     * \brief When the derived class is implemented,
     *        the resources are clean are unloaded here. eg:
     * 
     * \~
     * \snippet Plugins/TigerVnc/Client/PluginTigerVnc.cpp Clean resource
     */
    virtual ~CPluginClient();

    /// ID. Default: Protocol() + ":" + Name()
    virtual const QString Id() const;
    /// Plugin Protocol
    virtual const QString Protocol() const = 0;
    /// \~chinese 插件名，这个名一定要与工程名(${PROJECT_NAME})相同。
    ///           翻译文件(${PROJECT_NAME}_*.ts)）名与其相关。
    /// \~english This name must be the same as the project name (${PROJECT_NAME}).
    ///           The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
    virtual const QString Name() const = 0;
    /// The plugin display name
    virtual const QString DisplayName() const;
    /// Plugin description
    virtual const QString Description() const = 0;
    virtual const QIcon Icon() const;
    /// Display more information in About dialog
    virtual const QString Details() const;

private:
    /**
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
    virtual CConnecter* CreateConnecter(const QString& szProtocol) = 0;

private:
    QTranslator m_Translator;
    int InitTranslator();
    
    friend class CClient;
    
    CPluginThread* m_pThread;
};

QT_BEGIN_NAMESPACE
#define CPluginClient_iid "KangLinStudio.Rabbit.RemoteControl.CPluginClient"
Q_DECLARE_INTERFACE(CPluginClient, CPluginClient_iid)
QT_END_NAMESPACE

#endif // CPLUGINVIEWER_H_KL_2021_07_23
