#ifndef CMANAGERPLUGINS_H
#define CMANAGERPLUGINS_H

#include <QObject>
#include <QMap>
#include <QDir>
#include "PluginService.h"

/*!
 * \~chinese 管理插件
 * \details 从文件加载插件并管理
 * 
 * \~english
 * \brief Manage plugin.
 * \details Load plugins from file, and manage plugins
 *
 * \~ 
 * \ingroup LIBAPI_SERVICE
 */
class SERVICE_EXPORT CManagePlugins : public QObject
{
    Q_OBJECT
public:
    explicit CManagePlugins(QObject *parent = nullptr);
    virtual ~CManagePlugins();
    
    QMap<QString, CPluginService*> m_Plugins;

private:
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);
};

#endif // CMANAGERPLUGINS_H
