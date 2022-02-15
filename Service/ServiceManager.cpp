// Author: Kang Lin <kl222@126.com>

#include "ServiceManager.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"
#include "Service.h"

#include "PluginService.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>

/*!
 * \~Chinese
 * \details
 *  命令行参数：
 *    - s: 产生默认参数文件到目录中
 *    - e: 以用户进程方式启动程序
 *    
 * \~English
 * \details
 * Command line parameters:
 *   - s: Save default parameters to files in <directory>
 *   - e: Start a program as a user process
 */
CServiceManager::CServiceManager(int argc, char **argv, const QString& appName, const QString &name)
    : QtService<QCoreApplication>(argc, argv, name)
{
    application()->setApplicationName(appName);

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
            foreach(auto p, m_Plugins.m_Plugins)
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
    foreach(auto p, m_Plugins.m_Plugins)
        if(p) p->Start();
}

void CServiceManager::stop()
{
    LOG_MODEL_DEBUG("Service", "Stop ...");
    foreach(auto p, m_Plugins.m_Plugins)
        if(p) p->Stop();
}
