// Author: Kang Lin <kl222@126.com>

/*!
 *  \~english
 *  \defgroup Service Rabbit remote control service programe
 *  \brief Rabbit remote control service programe.
 *
 *  The command line arguments, see CServiceManager::CServiceManager
 *  
 *  \~chinese \defgroup Service 玉兔远程控制服务程序
 *  \brief 玉兔远程控制服务程序。
 *  
 *  命令行参数，请参见： CServiceManager::CServiceManager
 *  
 *  \~
 *  \ingroup APP
 */

#include "ServiceManager.h"
#include "RabbitCommonTools.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(App, "App")

int main(int argc, char *argv[])
{
    int nRet = 0;

    try{
        CServiceManager s(argc, argv, "RabbitRemoteControlService");
        s.exec();
    } catch (std::exception& e) {
        qCritical(App) << "Exception:" << e.what();
    }
    
    return nRet;
}
