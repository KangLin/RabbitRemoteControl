//! @author: Kang Lin (kl222@126.com)

#ifndef CPLUGINFACTORY_H
#define CPLUGINFACTORY_H

#pragma once

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
    /**
     * @brief CPluginFactory
     *        When the derived class is implemented,
     *        the resources are initialized and
     *        the translation resources are loaded here 
     * @param parent
     */
    explicit CPluginFactory(QObject *parent = nullptr);
    /**
     * @brief ~CPluginFactory
     *        When the derived class is implemented,
     *        the resources are clean and
     *        the translation resources are unloaded here 
     */
    virtual ~CPluginFactory();
    
    virtual const QString Id() const;
    virtual const QString Protol() const = 0;
    virtual const QString Name() const = 0;
    virtual const QString Description() const = 0;
    virtual const QIcon Icon() const;
    
    /**
     * @brief CreateConnecter
     * @param szProtol
     * @return Return CConnecter pointer, the owner is caller
     * @note The function new object pointer,
     *       the caller must delete it when don't use it.
     */
    virtual CConnecter* CreateConnecter(const QString& szProtol) = 0;
};

QT_BEGIN_NAMESPACE
#define CPluginFactory_iid "KangLinStudio.Rabbit.RemoteControl.CPluginFactory"
Q_DECLARE_INTERFACE(CPluginFactory, CPluginFactory_iid)
QT_END_NAMESPACE

#endif // CPLUGINFACTORY_H
