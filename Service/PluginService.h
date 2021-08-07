#ifndef CPLUGINSERVICE_H
#define CPLUGINSERVICE_H

#include <QObject>
#include "Service.h"

class CServiceThread;
class CServiceManager;

///
/// \~chinese 服务插件接口。默认为每个插件启动一个线程进行处理。
/// 
/// \~english
/// \brief The service plugin interface. The default start a thread.
/// 
/// \~
/// \ingroup LIBAPI_SERVICE
///
class SERVICE_EXPORT CPluginService : public QObject
{
    Q_OBJECT
public:
    explicit CPluginService(QObject *parent = nullptr);
    virtual ~CPluginService();
    
    /// \~english ID. Default: Protol() + "_" + Name()
    virtual const QString Id() const;
    /// \~english Plugin protol
    virtual const QString Protol() const = 0;
    /// \~english Plugin name
    virtual const QString Name() const = 0;
    /// \~english Plugin description
    virtual const QString Description() const = 0;
    virtual const QIcon Icon() const;
    
    virtual void Start();
    virtual void Stop();

protected:
    /// New service
    virtual CService* NewService() = 0;

private:
    CServiceThread* m_pThread;
    friend CServiceThread;
    friend CServiceManager;
};

QT_BEGIN_NAMESPACE
#define CPluginService_iid "KangLinStudio.Rabbit.RemoteControl.CPluginService"
Q_DECLARE_INTERFACE(CPluginService, CPluginService_iid)
QT_END_NAMESPACE

#endif // CPLUGINSERVICE_H
