// Author: Kang Lin <kl222@126.com>

#ifndef CMANAGECONNECTER_H
#define CMANAGECONNECTER_H

#pragma once

#include <QObject>
#include <QDir>
#include <QMap>
#include <QIcon>
#include <QTranslator>
#include "PluginFactory.h"

/**
 * \~english \brief manage plugins and connecter
 * \~
 * \see   CPluginFactory CConnecter
 * \ingroup VIEWER_API
 */
class RABBITREMOTECONTROL_EXPORT CManageConnecter : public QObject
{
    Q_OBJECT
    
public:
    explicit CManageConnecter(QObject *parent = nullptr);
    virtual ~CManageConnecter();
    
    /// \~english New CConnecter pointer, the owner is caller.
    ///           <b>The caller must delete it, when isn't need it</b>.
    virtual CConnecter* CreateConnecter(const QString &id);
    /// \~english New CConnecter pointer from file, the owner is caller.
    ///           <b>The caller must delete it, when isn't need it</b>.
    virtual CConnecter* LoadConnecter(const QString& szFile);
    /// \~english Save parameters to file
    virtual int SaveConnecter(const QString& szFile, CConnecter* pConnecter);

    class Handle{
    public:
        Handle(): m_bIgnoreReturn(false){}
        virtual int onProcess(const QString& id, CPluginFactory* pFactory) = 0;
        int m_bIgnoreReturn;
    };
    virtual int EnumPlugins(Handle* handle);

private:    
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);

private:
    QMap<QString, CPluginFactory*> m_Plugins;
    
    qint8 m_FileVersion;

    QTranslator m_Translator;
};

#endif // CMANAGECONNECTER_H
