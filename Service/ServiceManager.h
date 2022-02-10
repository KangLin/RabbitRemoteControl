// Author: Kang Lin <kl222@126.com>

#ifndef CSERVICEMANAGER_H
#define CSERVICEMANAGER_H

#pragma once

#include "QtService/QtService"
#include <QMap>
#include <QDir>
#include "service_export.h"
#include <QApplication>

class CPluginService;

/*!
 *  \~chinese
 *   \brief 管理服务插件
 *   \details 管理系统服务与服务插件。  
 *   使用：
 *   
 *        
 *  \~english
 *  \brief Manage service plugin
 *  \details Manage system service and server plugins.  
 *  Usge:
 * 
 *  \~
 *        \code
 *        CServiceManager s(argc, argv, "RabbitRemoteControlService");
 *        s.exec();
 *        \endcode
 *        
 *  \ingroup LIBAPI_SERVICE
 *  \see CPluginService
 */
class SERVICE_EXPORT CServiceManager : public QtService<QApplication>
{
public:
    explicit CServiceManager(int argc, char **argv, const QString& appName, const QString &name = "Rabbit Remote control service");
    virtual ~CServiceManager();
    
    // QtServiceBase interface
protected:
    virtual void start() override;
    virtual void stop() override;

private:    
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);
    
    QMap<QString, CPluginService*> m_Plugins;
};

#endif // CSERVICEMANAGER_H
