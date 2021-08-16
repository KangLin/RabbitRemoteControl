#ifndef CSERVICEMANAGER_H
#define CSERVICEMANAGER_H

#pragma once

#include "QtService/QtService"
#include <QMap>
#include <QDir>
#include "service_export.h"
#include <QApplication>

class CPluginService;

///
/// \~chinese 管理服务插件
/// 
/// \~english
/// \brief Manage service plugin
///
/// \~
/// \ingroup LIBAPI_SERVICE
///
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
