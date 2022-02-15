#ifndef CMANAGERPLUGINS_H
#define CMANAGERPLUGINS_H

#include <QObject>
#include <QMap>
#include <QDir>
#include "PluginService.h"

class SERVICE_EXPORT CManagerPlugins : public QObject
{
    Q_OBJECT
public:
    explicit CManagerPlugins(QObject *parent = nullptr);
    virtual ~CManagerPlugins();
    
    QMap<QString, CPluginService*> m_Plugins;

private:    
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);
};

#endif // CMANAGERPLUGINS_H
