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
 * \brief The plugin interfaces
 * \~chinese
 * \defgroup PLUGIN_API 插件接口
 * \brief 插件接口
 * \~ \ingroup LIBAPI
 */

/**
 * \~english \brief Create CConnecter instance
 *                  be implemented by the protol plugin
 * \~chinese \brief CPluginFactory 接口，建立 CConnecter 实例，它由协议插件实现
 * \~ \ingroup PLUGIN_API
 */
class RABBITREMOTECONTROL_EXPORT CPluginFactory : public QObject
{
    Q_OBJECT

public:
    /**
     * \~english
     * \brief When the derived class is implemented,
     *        the resources are initialized and
     *        the translation resources are loaded here 
     * 
     * \~chinese
     * \brief 派生类必须实现它，用于初始化操作。例如：初始化资源，加载翻译资源等
     * 
     * \~ \param parent
     */
    explicit CPluginFactory(QObject *parent = nullptr);
    /**
     * \~english
     * \brief When the derived class is implemented,
     *        the resources are clean and
     *        the translation resources are unloaded here 
     * \~chinese 
     * \brief 派生类实现它，用于释放资源。
     */
    virtual ~CPluginFactory();
    
    virtual const QString Id() const;
    virtual const QString Protol() const = 0;
    virtual const QString Name() const = 0;
    virtual const QString Description() const = 0;
    virtual const QIcon Icon() const;
    
    /**
     * \~english
     * \brief Create CConnecter instance
     * @return Return CConnecter pointer, the owner is caller
     * @note The function new object pointer,
     *       the caller must delete it when don't use it.
     * @param szProtol
     * \~chinese 
     * \brief 建立 CConnecter 实例
     * @return 返回 CConnecter 指针, 它的所有者是调用者。
     * @note 此函数新建一个堆栈对象指针，
     *       调用者必须负责在用完后释放指针。
     * @param szProtol 连接协议
     */
    virtual CConnecter* CreateConnecter(const QString& szProtol) = 0;
};

QT_BEGIN_NAMESPACE
#define CPluginFactory_iid "KangLinStudio.Rabbit.RemoteControl.CPluginFactory"
Q_DECLARE_INTERFACE(CPluginFactory, CPluginFactory_iid)
QT_END_NAMESPACE

#endif // CPLUGINFACTORY_H
