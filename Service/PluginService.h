// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINSERVICE_H
#define CPLUGINSERVICE_H

#include <QObject>
#include <QTranslator>

#include "Service.h"
#include "Screen.h"

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
    /*!
     * \~chinese 插件名，这个名一定要与工程名(${PROJECT_NAME})相同。
     *           翻译文件(${PROJECT_NAME}_*.ts)）名与其相关。
     * \~english This name must be the same as the project name (${PROJECT_NAME}).
     *           The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
     */
    virtual const QString Name() const = 0;
    /// \~english Plugin description
    virtual const QString Description() const = 0;
    virtual const QIcon Icon() const;
    
    /*!
     * \brief Default start a new thread to process Service
     * \see CService CServiceThread
     */
    virtual void Start();
    virtual void Stop();

protected:
    /// New service
    virtual CService* NewService() = 0;

private:
    CServiceThread* m_pThread;
    friend CServiceThread;
    friend CServiceManager;

    QTranslator m_Translator;
    int InitTranslator();
};

QT_BEGIN_NAMESPACE
#define CPluginService_iid "KangLinStudio.Rabbit.RemoteControl.CPluginService"
Q_DECLARE_INTERFACE(CPluginService, CPluginService_iid)
QT_END_NAMESPACE

#endif // CPLUGINSERVICE_H
