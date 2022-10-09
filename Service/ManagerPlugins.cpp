#include "ManagerPlugins.h"
#include "RabbitCommonDir.h"
#include <QPluginLoader>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Service)
CManagePlugins::CManagePlugins(QObject *parent)
    : QObject{parent}
{
    LoadPlugins();
}

CManagePlugins::~CManagePlugins()
{
    qDebug(Service) << "CManagerPlugins::~CManagerPlugins";
}

int CManagePlugins::LoadPlugins()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances())
    {
        CPluginService* p = qobject_cast<CPluginService*>(plugin);
        if(p)
            m_Plugins.insert(p->Id(), p);
    }

    QString szPath = RabbitCommon::CDir::Instance()->GetDirPlugins()
            + QDir::separator() + "Service";
    QStringList filters;
#if defined (Q_OS_WINDOWS)
    filters << "*PluginService*.dll";
#else
    filters << "*PluginService*.so";
#endif
    return FindPlugins(szPath, filters);
}

int CManagePlugins::FindPlugins(QDir dir, QStringList filters)
{
    QString fileName;
    if(filters.isEmpty())
    {
#if defined (Q_OS_WINDOWS)
        filters << "*PluginService*.dll";
#else
        filters << "*PluginService*.so";
#endif
    }
    
    QString szCurrentPath = QDir::currentPath();
    QStringList files = dir.entryList(filters, QDir::Files);
    if(!files.isEmpty())
    {
        //This method is invalid
        //QCoreApplication::addLibraryPath(QDir::cleanPath(dir.absolutePath()));

        QDir::setCurrent(QDir::cleanPath(QDir::cleanPath(dir.absolutePath())));
        
        // This method is valid
//#if defined(Q_OS_WINDOWS)
//        QString szPath = QString::fromLocal8Bit(qgetenv("PATH"));
//        szPath += ";";
//        szPath += QDir::cleanPath(QDir::cleanPath(dir.absolutePath()));
//        qputenv("PATH", szPath.toLatin1());
//#endif
    }
        
    foreach (fileName, files) {
        QString szPlugin = dir.absoluteFilePath(fileName);
        QPluginLoader loader(szPlugin);
        QObject *plugin = loader.instance();
        if (plugin) {
            CPluginService* p = qobject_cast<CPluginService*>(plugin);
            if(p)
            {
                m_Plugins.insert(p->Id(), p);
                p->InitTranslator();
                qInfo(Service) << "Load plugin id:" << p->Id()
                                << ";Description:" << p->Description()
                                << ";File:" << szPlugin;
            }
        }else{
            qCritical(Service) << "load plugin error:" << loader.errorString();;
        }
        
        foreach (fileName, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QDir pluginDir = dir;
            if(pluginDir.cd(fileName))
                FindPlugins(pluginDir, filters);
        }
    }

    QDir::setCurrent(szCurrentPath);

    return 0;
}
