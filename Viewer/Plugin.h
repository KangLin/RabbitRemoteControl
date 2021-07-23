// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGIN_H_KL_2021_07_23
#define CPLUGIN_H_KL_2021_07_23

#pragma once

#include <QIcon>
#include <QObject>
#include "Connecter.h"

/**
 * \~english \brief The plugin interface. Create CConnecter instance
 *                  be implemented by the protol plugin.
 * \~
 * \ingroup VIEWER_PLUGIN_API
 * \see CConnecter
 */
class VIEWER_EXPORT CPlugin : public QObject
{
    Q_OBJECT

public:
    /**
     * \~english
     * \brief the resources are initialized and
     *        the translation resources are loaded here. eg:
     * \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Initialize resorce
     * \note When the derived class is implemented.
     * 
     * \~
     * \param parent
     */
    explicit CPlugin(QObject *parent = nullptr);
    /**
     * \~english
     * \brief When the derived class is implemented,
     *        the resources are clean and
     *        the translation resources are unloaded here. eg:
     * 
     * \~
     * \snippet Viewer/Plugins/TigerVnc/PluginFactoryTigerVnc.cpp Clean resource
     */
    virtual ~CPlugin();

    /// \~english ID. Default: Protol() + ":" + Name()
    virtual const QString Id() const;
    /// \~english Plugin protol
    virtual const QString Protol() const = 0;
    /// \~english Plugin name
    virtual const QString Name() const = 0;
    /// \~english Plugin description
    virtual const QString Description() const = 0;
    virtual const QIcon Icon() const;
    
    /**
     * \~english
     * \brief New CConnecter instance. Only is called by CManageConnecter 
     * \return Return CConnecter pointer, the owner is caller
     * \note The function new object pointer,
     *       <b>the caller must delete it when don't use it</b>.
     * \param szProtol
     * 
     * \~ \see CManageConnecter::CreateConnecter CManageConnecter::LoadConnecter
     * 
     */
    virtual CConnecter* CreateConnecter(const QString& szProtol) = 0;
};

QT_BEGIN_NAMESPACE
#define CPlugin_iid "KangLinStudio.Rabbit.RemoteControl.CPlugin"
Q_DECLARE_INTERFACE(CPlugin, CPlugin_iid)
QT_END_NAMESPACE

#endif // CPLUGIN_H_KL_2021_07_23
