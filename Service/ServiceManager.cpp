#include "ServiceManager.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"
#include "Service.h"
#include <QPluginLoader>
#include "PluginService.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>

CServiceManager::CServiceManager(int argc, char **argv, const QString& appName, const QString &name)
    : QtService<QApplication>(argc, argv, name)
{
    application()->setApplicationName(appName);

    LoadPlugins();

    QCommandLineParser parser;
    //add default parameters 
    QCommandLineOption oConfigFile(QStringList() << "s" << "save",
                                   "Save default parameters to files in <directory>",
                                   "directory",
                                   RabbitCommon::CDir::Instance()->GetDirUserConfig());

    parser.addOption(oConfigFile);
    QStringList paras;
    for(int n = 0; n < argc; n++)
        paras << argv[n];
    parser.parse(paras);
    bool bSave = parser.isSet(oConfigFile);
    if(bSave)
    {
        QString szDir = parser.value(oConfigFile);
        if(!szDir.isEmpty())
        {
            foreach(auto p, m_Plugins)
            {
                if(p)
                {
                    CService* pService = p->NewService();
                    if(pService)
                        pService->SaveConfigure(szDir);
                }
            }
            application()->quit();
        }
    }
}

CServiceManager::~CServiceManager()
{
    LOG_MODEL_DEBUG("CServiceManager", "CServiceManager::~CServiceManager");
}

void CServiceManager::start()
{
    LOG_MODEL_DEBUG("Service", "Start ...");
    foreach(auto p, m_Plugins)
    {
        if(p) p->Start();
    }
}

void CServiceManager::stop()
{
    LOG_MODEL_DEBUG("Service", "Stop ...");
    foreach(auto p, m_Plugins)
    {
        if(p) p->Stop();
    }
}

int CServiceManager::LoadPlugins()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances())
    {
        CPluginService* p = qobject_cast<CPluginService*>(plugin);
        if(p)
        {
            m_Plugins.insert(p->Id(), p);
        }
    }

    QString szPath = RabbitCommon::CDir::Instance()->GetDirPlugins();
    QStringList filters;
#if defined (Q_OS_WINDOWS)
        filters << "*PluginService*.dll";
#else
        filters << "*PluginService*.so";
#endif
    return FindPlugins(szPath, filters);
}

int CServiceManager::FindPlugins(QDir dir, QStringList filters)
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
//                LOG_MODEL_DEBUG("CServiceManager", "Load plugin:%s;%s",
//                                p->Id().toStdString().c_str(),
//                                p->Description().toStdString().c_str());
            }
        }else{
            QString szMsg;
            szMsg = "load plugin error: " + loader.errorString();
            LOG_MODEL_ERROR("ManageConnecter", szMsg.toStdString().c_str());
        }
    }

    QDir::setCurrent(szCurrentPath);

    return 0;
}
