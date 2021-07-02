// Author: Kang Lin <kl222@126.com>

#ifndef CMANAGECONNECTER_H
#define CMANAGECONNECTER_H

#pragma once

#include <QObject>
#include <QDir>
#include <QMap>
#include <QIcon>

#include "PluginFactory.h"

/**
 * \~english \defgroup LIBAPI Rabbit remote control library
 * \brief Rabbit remote control library
 * \image html docs/Image/PlugAPI.svg
 * 
 * \~chinese \defgroup LIBAPI 玉兔远程控制库
 * \brief 玉兔远程控制库
 * \image html docs/Image/PlugAPI.svg
 */

/**
 * \~english \defgroup API Application interface
 * \brief Application interface
 * \~chinese \defgroup API 应用程序接口
 * \brief 应用程序接口
 * \~ \ingroup LIBAPI
 */

/**
 * \~english \brief manage plugins and connecter
 * \~chinese \brief 管理插件和连接者
 * \~
 * \see   CPluginFactory CConnecter
 * \ingroup API
 */
class RABBITREMOTECONTROL_EXPORT CManageConnecter : public QObject
{
    Q_OBJECT
    
public:
    explicit CManageConnecter(QObject *parent = nullptr);
    virtual ~CManageConnecter();
    
    /// \~english New CConnecter pointer, the owner is caller.
    ///           <b>The caller must delete it, when isn't need it</b>.
    /// \~chinese 新建 CConnecter 指针，所有者是调用者。
    ///           <b>当不在使用时，调用者必须释放指针</b>。
    virtual CConnecter* CreateConnecter(const QString &id);
    /// \~english New CConnecter pointer from file, the owner is caller.
    ///           <b>The caller must delete it, when isn't need it</b>.
    /// \~chinese 从文件中新建 CConnecter 指针，所有者是调用者。
    ///           <b>当不在使用时，调用者必须释放指针</b>。
    virtual CConnecter* LoadConnecter(const QString& szFile);
    /// \~english Save parameters to file
    /// \~chinese 保存参数到文件
    virtual int SaveConnecter(const QString& szFile, CConnecter* pConnecter);

    class Handle{
    public:
        Handle(): m_bIgnoreReturn(false){}
        virtual int onProcess(const QString& id, CPluginFactory* pFactory) = 0;
        int m_bIgnoreReturn;
    };
    virtual int EnumPlugins(Handle* handle);

private:    
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);

private:
    QMap<QString, CPluginFactory*> m_Plugins;
    
    qint8 m_FileVersion;
};

#endif // CMANAGECONNECTER_H
