#ifndef CMANAGECONNECTER_H
#define CMANAGECONNECTER_H

#include <QObject>
#include <QDir>
#include <QMap>
#include <QList>
#include <QIcon>

#include "PluginFactory.h"

/**
 * @group API
 */

/**
 * @brief The CManageConnecter class
 * @addtogroup API
 */
class RABBITREMOTECONTROL_EXPORT CManageConnecter : public QObject
{
    Q_OBJECT
    
public:
    explicit CManageConnecter(QObject *parent = nullptr);
    virtual ~CManageConnecter();
    
    // Return CConnecter pointer, the owner is caller
    virtual CConnecter* CreateConnecter(const QString& szProtol);
    
    virtual CConnecter* LoadConnecter(const QString& szFile);
    virtual int SaveConnecter(const QString& szFile, CConnecter* pConnecter);
    
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);
    
    QList<CPluginFactory*> GetManageConnecter();
    
private:
    QMap<QString, CPluginFactory*> m_Plugins;
    
    qint8 m_FileVersion;
};

#endif // CMANAGECONNECTER_H
