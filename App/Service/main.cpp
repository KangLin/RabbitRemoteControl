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
 */

#include "ServiceManager.h"

int main(int argc, char *argv[])
{
    int nRet = 0;
    

    CServiceManager s(argc, argv, "RabbitRemoteControlService");
    s.exec();
    return nRet;
}
