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
 * \~english \defgroup LIBAPI Library
 * \brief Library
 * \~chinese \defgroup LIBAPI 库
 * \brief 库
 */

/**
 * \~english \defgroup API Application interface
 * \brief Application interface
 * \~chinese \defgroup API 应用程序接口
 * \brief 应用程序接口
 * \~ \ingroup LIBAPI
 */

/**
 * \~english \brief manage plugins
 * \~chinese \brief 管理插件
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
    
    // Return CConnecter pointer, the owner is caller
    virtual CConnecter* CreateConnecter(const QString &id);
    
    virtual CConnecter* LoadConnecter(const QString& szFile);
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
