// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINVIEWER_H_KL_2021_07_23
#define CPLUGINVIEWER_H_KL_2021_07_23

#pragma once

#include <QIcon>
#include <QObject>
#include "Connecter.h"
#include <QTranslator>

/**
 * \~chinese \brief 控制者插件接口。建立 CConnecter 实例，它由协议插件实现。
 * 
 * \~english \brief The plugin interface. Create CConnecter instance
 *                  be implemented by the protol plugin.
 * \~
 * \ingroup VIEWER_PLUGIN_API
 * \see CConnecter CManageConnecter
 */
class VIEWER_EXPORT CPluginViewer : public QObject
{
    Q_OBJECT

public:
    /**
     * \~chinese
     * \brief 初始化操作。例如：初始化资源等，例如：
     * \snippet Plugins/TigerVnc/Viewer/PluginTigerVnc.cpp Initialize resorce
     * \note  派生类必须实现它. 
     * 
     * \~english
     * \brief the resources are initialized are loaded here. eg:
     * \snippet Plugins/TigerVnc/Viewer/PluginTigerVnc.cpp Initialize resorce
     * \note When the derived class is implemented.
     * 
     * \~
     * \param parent
     */
    explicit CPluginViewer(QObject *parent = nullptr);
    /**
     * \~chinese \brief 派生类实现它，用于释放资源。例如：
     * \~english
     * \brief When the derived class is implemented,
     *        the resources are clean are unloaded here. eg:
     * 
     * \~
     * \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Clean resource
     */
    virtual ~CPluginViewer();

    /// \~english ID. Default: Protol() + ":" + Name()
    virtual const QString Id() const;
    /// \~english Plugin protol
    virtual const QString Protol() const = 0;
    /// \~chinese 插件名，这个名一定要与翻译文件(.ts)名相同
    /// \~english Plugin name, The name is same as translation file name
    virtual const QString Name() const = 0;
    /// \~english Plugin description
    virtual const QString Description() const = 0;
    virtual const QIcon Icon() const;
    
    /**
     * \~chinese
     * \brief 新建 CConnecter 实例。仅由 CManageConnecter 调用
     * \return 返回 CConnecter 指针, 它的所有者是调用者。
     * \note 此函数新建一个堆栈对象指针，
     *       <b>调用者必须负责在用完后释放指针</b>。
     * \param szProtol 连接协议
     * 
     * \~english
     * \brief New CConnecter instance. Only is called by CManageConnecter 
     * \return Return CConnecter pointer, the owner is caller
     * \note The function new object pointer,
     *       <b>the caller must delete it when don't use it</b>.
     * \param szProtol
     * 
     * \~
     * \see CManageConnecter::CreateConnecter CManageConnecter::LoadConnecter
     * 
     */
    virtual CConnecter* CreateConnecter(const QString& szProtol) = 0;
    
private:
    QTranslator m_Translator;
    int InitTranslator();
    
    friend class CManageConnecter;
};

QT_BEGIN_NAMESPACE
#define CPluginViewer_iid "KangLinStudio.Rabbit.RemoteControl.CPluginViewer"
Q_DECLARE_INTERFACE(CPluginViewer, CPluginViewer_iid)
QT_END_NAMESPACE

#endif // CPLUGINVIEWER_H_KL_2021_07_23
