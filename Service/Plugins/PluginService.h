#ifndef CPLUGINSERVICE_H
#define CPLUGINSERVICE_H

#include <QObject>
#include "Service.h"

class CServiceThread;

class CPluginService : public QObject
{
    Q_OBJECT
public:
    explicit CPluginService(QObject *parent = nullptr);
    virtual ~CPluginService();
    
    /// \~english ID. Default: Protol() + ":" + Name()
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
    virtual CService* NewService() = 0;

private:
    CService* m_pService;
    CServiceThread* m_pThread;
    friend CServiceThread;
};

QT_BEGIN_NAMESPACE
#define CPluginService_iid "KangLinStudio.Rabbit.RemoteControl.CPluginService"
Q_DECLARE_INTERFACE(CPluginService, CPluginService_iid)
QT_END_NAMESPACE

#endif // CPLUGINSERVICE_H
