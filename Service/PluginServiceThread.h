#ifndef CPLUGINSERVICTHREAD_H
#define CPLUGINSERVICTHREAD_H

#include "PluginService.h"

/*!
 *  \~chinese 服务插件接口。默认为每个插件启动一个线程进行处理。此接口仅由插件实现。
 *  
 *  \~english
 *  \brief The service plugin interface. The default start a thread.
 *         The interface only is implemented and used by plugin.
 *  
 *  \~
 *  \ingroup LIBAPI_SERVICE
 *  \see CService CServiceThread CServiceManager CPluginService
 */
class SERVICE_EXPORT CPluginServiceThread : public CPluginService
{
    Q_OBJECT

public:
    explicit CPluginServiceThread(QObject *parent = nullptr);
    virtual ~CPluginServiceThread() override;
    
    /*!
     * \brief Default start a new thread to process Service
     * \see CService CServiceThread
     */
    virtual void Start() override;
    virtual void Stop() override;
    
private:
    QSharedPointer<CServiceThread> m_Thread;
};

#endif // CPLUGINSERVICTHREAD_H
