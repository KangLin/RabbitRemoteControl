// Author: Kang Lin <kl222@126.com>

#include "ServiceManager.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"
#include "Service.h"

#include "PluginService.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>

CServiceManager::CServiceManager(int argc, char **argv, const QString& appName, const QString &name)
    : QtService<QCoreApplication>(argc, argv, name)
{
    //add default parameters 
    QStringList args;
    for (int i = 0; i < argc; ++i)
         args.append(QString::fromLocal8Bit(argv[i]));
    if(args.size() >= 2)
    {
        QString a = args.at(1);
        if (a == QLatin1String("-s") || a == QLatin1String("-save"))
        {
            createApplication(argc, argv);
            application()->setApplicationName(appName);
            QString szDir;
            if(args.size() >= 3)
                szDir = args.at(2);
            if(szDir.isEmpty())
                szDir = RabbitCommon::CDir::Instance()->GetDirUserConfig();
            if(!szDir.isEmpty())
            {
                // Make sure the plugin is loaded only after the application is created
                if(!m_Plugins)
                    m_Plugins = QSharedPointer<CManagePlugins>(new CManagePlugins());
                foreach(auto p, m_Plugins->m_Plugins)
                {
                    if(p)
                    {
                        CService* pService = p->NewService();
                        if(pService)
                            pService->SaveConfigure(szDir);
                    }
                }
                application()->quit();
                return;
            }
        } else if (a == QLatin1String("-h") || a == QLatin1String("-help")) {
            exec();
            printf("\t-s(ave) [Directory] \t: Generate configuration file in [Directory]\n");
            throw std::invalid_argument("Help argument");
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
    // Make sure the plugin is loaded only after the application is created
    if(!m_Plugins)
        m_Plugins = QSharedPointer<CManagePlugins>(new CManagePlugins());
    foreach(auto p, m_Plugins->m_Plugins)
        if(p) p->Start();
}

void CServiceManager::stop()
{
    LOG_MODEL_DEBUG("Service", "Stop ...");
    if(!m_Plugins) return;
    foreach(auto p, m_Plugins->m_Plugins)
        if(p) p->Stop();
}
