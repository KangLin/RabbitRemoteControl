// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINSERVICE_H
#define CPLUGINSERVICE_H

#include <QObject>
#include <QTranslator>
#include <QSharedPointer>

#include "Service.h"
#include "Screen.h"

class CServiceThread;

/*!
 *  \~chinese 服务插件接口。此接口仅由插件实现。
 *  
 *  \~english
 *  \brief The service plugin interface.
 *         The interface only is implemented and used by plugin.
 *  
 *  \~
 *  \ingroup LIBAPI_SERVICE
 *  \see CService CServiceManager
 */
class SERVICE_EXPORT CPluginService : public QObject
{
    Q_OBJECT

public:
    explicit CPluginService(QObject *parent = nullptr);
    virtual ~CPluginService();
    
    /// ID. Default: Protocol() + "_" + Name()
    virtual const QString Id() const;
    /// Plugin Protocol
    virtual const QString Protocol() const = 0;
    /*!
     * \~chinese 插件名，这个名一定要与工程名(${PROJECT_NAME})相同。
     *           翻译文件(${PROJECT_NAME}_*.ts)）名与其相关。
     * \~english This name must be the same as the project name (${PROJECT_NAME}).
     *           The translation file (${PROJECT_NAME}_*.ts)) name is associated with it. 
     */
    virtual const QString Name() const = 0;
    /// The plugin display name
    virtual const QString DisplayName() const;
    /// \~chinese 插件描述
    /// \~english Plugin description
    virtual const QString Description() const = 0;
    virtual const QIcon Icon() const;
    
    /*!
     * \brief Start a Service
     * \see CService
     */
    virtual void Start();
    virtual void Stop();

    /// New service
    virtual CService* NewService() = 0;

    int InitTranslator();
    
private:
    QSharedPointer<CService> m_Service;
    QTranslator m_Translator;
};

QT_BEGIN_NAMESPACE
#define CPluginService_iid "KangLinStudio.Rabbit.RemoteControl.CPluginService"
Q_DECLARE_INTERFACE(CPluginService, CPluginService_iid)
QT_END_NAMESPACE

#endif // CPLUGINSERVICE_H
