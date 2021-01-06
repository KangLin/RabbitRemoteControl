//! @author: Kang Lin(kl222@126.com)

#ifndef CMANAGECONNECTER_H
#define CMANAGECONNECTER_H

#include <QObject>
#include <QDir>
#include <QMap>
#include <QIcon>

#include "PluginFactory.h"

/**
 * @group API
 * @brief Rabbit remote control API
 */

/**
 * @brief The CManageConnecter class, manage plugins
 * @see   CPluginFactory CConnecter
 * @addtogroup API
 */
class RABBITREMOTECONTROL_EXPORT CManageConnecter : public QObject
{
    Q_OBJECT
    
public:
    explicit CManageConnecter(QObject *parent = nullptr);
    virtual ~CManageConnecter();
    
    // Return CConnecter pointer, the owner is caller
    virtual CConnecter* CreateConnecter(const QString &id);
    
    virtual CConnecter* LoadConnecter(const QString& szFile);
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
};

#endif // CMANAGECONNECTER_H
