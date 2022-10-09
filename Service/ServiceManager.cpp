// Author: Kang Lin <kl222@126.com>

#include "ServiceManager.h"
#include "RabbitCommonDir.h"
#include "Service.h"
#ifdef HAVE_ICE
    #include "Ice.h"
#endif

#include "PluginService.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QSettings>
#include <QLoggingCategory>
#include <stdexcept>

Q_LOGGING_CATEGORY(Service, "Service")

CServiceManager::CServiceManager(int argc, char **argv, const QString& appName, const QString &name)
    : QtService<QCoreApplication>(argc, argv, name)
{
    // Add default parameters 
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

#ifdef HAVE_ICE
                QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                              QSettings::IniFormat);
                CICE::Instance()->GetParameter()->Save(set);
                set.sync();
#endif
                application()->quit();
                return;
            }
        } else if (a == QLatin1String("-h") || a == QLatin1String("-help")) {
            exec();
            printf("\t-s(ave) [Directory] \t: Generate configuration file in [Directory]\n");
            throw std::invalid_argument("Help argument");
        }
    }
    
#ifdef HAVE_ICE
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    CICE::Instance()->GetParameter()->Load(set);
#endif
}

CServiceManager::~CServiceManager()
{
    qDebug(Service) << "CServiceManager::~CServiceManager";
}

void CServiceManager::start()
{
    qDebug(Service) << "Service", "Start ...";
    // Make sure the plugin is loaded only after the application is created
    if(!m_Plugins)
        m_Plugins = QSharedPointer<CManagePlugins>(new CManagePlugins());
    foreach(auto p, m_Plugins->m_Plugins)
        if(p) p->Start();
    
#ifdef HAVE_ICE
    CICE::Instance()->slotStart();
#endif
}

void CServiceManager::stop()
{
    qDebug(Service) << "Service", "Stop ...";
    if(!m_Plugins) return;
    foreach(auto p, m_Plugins->m_Plugins)
        if(p) p->Stop();
    
#ifdef HAVE_ICE
    CICE::Instance()->slotStop();
#endif
}
