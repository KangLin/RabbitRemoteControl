//! @author: Kang Lin(kl222@126.com)

#ifndef CPLUGINFACTORY_H
#define CPLUGINFACTORY_H

#include <QIcon>
#include <QObject>
#include "Connecter.h"

/**
 * @brief The CPluginFactory interface, Create connecter instance
 *        be implemented by the protol plugin
 * @addtogroup API
 */
class RABBITREMOTECONTROL_EXPORT CPluginFactory : public QObject
{
    Q_OBJECT

public:
    explicit CPluginFactory(QObject *parent = nullptr);

    virtual QString Name() = 0;
    virtual QString Description() = 0;
    virtual QString Protol() = 0;
    virtual QIcon Icon();
    
    // Return CConnecter pointer, the owner is caller
    virtual CConnecter* CreateConnecter(const QString& szProtol) = 0;
};

QT_BEGIN_NAMESPACE
#define CPluginFactory_iid "KangLinStudio.Rabbit.RemoteControl.CPluginFactory"
Q_DECLARE_INTERFACE(CPluginFactory, CPluginFactory_iid)
QT_END_NAMESPACE

#endif // CPLUGINFACTORY_H
