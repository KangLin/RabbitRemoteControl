#include "ManagerPlugins.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include <QPluginLoader>

CManagerPlugins::CManagerPlugins(QObject *parent)
    : QObject{parent}
{
    LoadPlugins();
}

CManagerPlugins::~CManagerPlugins()
{
    LOG_MODEL_DEBUG("CManagerPlugins", "CManagerPlugins::~CManagerPlugins");
}

int CManagerPlugins::LoadPlugins()
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

int CManagerPlugins::FindPlugins(QDir dir, QStringList filters)
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
        QString szPlugins = dir.absoluteFilePath(fileName);
        QPluginLoader loader(szPlugins);
        QObject *plugin = loader.instance();
        if (plugin) {
            CPluginService* p = qobject_cast<CPluginService*>(plugin);
            if(p)
            {
                m_Plugins.insert(p->Id(), p);
                p->InitTranslator();
//                LOG_MODEL_DEBUG("CServiceManager", "Load plugin:%s;%s",
//                                p->Id().toStdString().c_str(),
//                                p->Description().toStdString().c_str());
            }
        }else{
            QString szMsg;
            szMsg = "load plugin error: " + loader.errorString();
            LOG_MODEL_ERROR("ManageConnecter", szMsg.toStdString().c_str());
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
