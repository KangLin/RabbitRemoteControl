// Author: Kang Lin <kl222@126.com>

#ifndef CSERVICEMANAGER_H
#define CSERVICEMANAGER_H

#pragma once

#include "QtService/QtService"
#include "service_export.h"
#include <QCoreApplication>
#include <QSharedPointer>
#include "ManagerPlugins.h"

class CPluginService;

/*!
 *  \~chinese
 *   \brief 管理服务
 *   \details 管理系统服务
 *   
 *   使用：
 *   
 *        
 *  \~english
 *  \brief Manage service
 *  \details Manage system service
 *  
 *  Usge:
 * 
 *  \~
 *        \code
 *        CServiceManager s(argc, argv, "RabbitRemoteControlService");
 *        s.exec();
 *        \endcode
 *        
 *  \ingroup LIBAPI_SERVICE
 *  \see CManagerPlugins CPluginService
 */
class SERVICE_EXPORT CServiceManager : public QtService<QCoreApplication>
{
public:
    /*!
     *  \~chinese
     *   
     *   命令行参数：
     *    <table>
           <tr><th> 短参数 </th><th> 长参数      </th><th> 解释 </th></tr>
           <tr><td> -i    </td><td> -install   </td><td> 安装服务 </td></tr>
           <tr><td> -u    </td><td> -uninstall </td><td> 卸载服务 </td></tr>
           <tr><td> -e    </td><td> -exec      </td>
                 <td> 做为一个标准应用程序执行（通常用于调试目的）。
                 这是一个阻塞调用，服务象标准程序一样执行。在此模式下，你不能用控制器（controller)与服务通信。
                 </td>
           </tr>
           <tr><td> -t </td><td> -terminate </td><td> 停止服务 </td></tr>
           <tr><td> -p </td><td> -pause </td><td> 暂停服务 </td></tr>
           <tr><td> -r </td><td> -resume </td><td> 继续暂停的服务 </td></tr>
           <tr><td> -c \e{cmd} </td><td> -command \e{cmd}</td>
                <td> 发送用户定义的命令代码到服务程序</td>
           </tr>
           <tr><td>-s </td><td> -save    </td><td> 产生默认配置文件到 [目录] 中</td></tr>
           <tr><td>-v </td><td> -version </td><td> 显示版本和状态信息 </td></tr>
           <tr><td colspan=2>无参数</td><td> 开始服务 </td></tr>
        </table>
        
     *  \~english
     *  The following command line arguments are recognized as service specific:
     *  
        <table>
           <tr><th> Short </th><th> Long       </th><th> Explanation </th></tr>
           <tr><td> -i    </td><td> -install   </td><td> Install the service. </td></tr>
           <tr><td> -u    </td><td> -uninstall </td><td> Uninstall the service.</td></tr>
           <tr><td> -e    </td><td> -exec      </td>
                 <td> Execute the service as a standalone application (useful for debug purposes).
                   This is a blocking call, the service will be executed like a normal application.
                   In this mode you will not be able to communicate with the service from the controller.
                 </td>
           </tr>
           <tr><td> -t </td><td> -terminate </td><td> Stop the service.</td></tr>
           <tr><td> -p </td><td> -pause </td><td> Pause the service.</td></tr>
           <tr><td> -r </td><td> -resume </td><td> Resume a paused service.</td></tr>
           <tr><td> -c \e{cmd} </td><td> -command \e{cmd}</td>
                <td> Send the user defined command code \e{cmd} to the service application.</td>
           </tr>
           <tr><td>-s </td><td> -save    </td><td> Generate default configuration file in [Directory]</td></tr>
           <tr><td>-v </td><td> -version </td><td> Display version and status information.</td></tr>
           <tr><td colspan=2>No arguments</td><td>Start the service</td></tr>
        </table>
         
     */
    explicit CServiceManager(int argc, char **argv, const QString& appName, const QString &name = "Rabbit Remote control service");
    virtual ~CServiceManager();
    
    // QtServiceBase interface
protected:
    virtual void start() override;
    virtual void stop() override;
    
    QSharedPointer<CManagePlugins> m_Plugins;
};

#endif // CSERVICEMANAGER_H
