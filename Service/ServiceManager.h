#ifndef CSERVICEMANAGER_H
#define CSERVICEMANAGER_H

#pragma once

#include "QtService/QtService"
#include <QMap>
#include <QDir>
#include "service_export.h"

class CPluginService;
class SERVICE_EXPORT CServiceManager : public QtService<QCoreApplication>
{
public:
    CServiceManager(int argc, char **argv, const QString &name = "Rabbit Remote control service");
    
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
