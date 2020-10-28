#ifndef CMANAGECONNECTER_H
#define CMANAGECONNECTER_H

#include <QObject>
#include <QDir>
#include <QMap>

#include "Connecter.h"

class RABBITREMOTECONTROL_EXPORT CManageConnecter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString m_szName READ Name)
    Q_PROPERTY(QString m_szDescription READ Description)
    
public:
    explicit CManageConnecter(QObject *parent = nullptr);
    
    virtual QString Name();
    virtual QString Description();
    
    // Return CConnecter pointer, the owner is caller
    virtual CConnecter* CreateConnecter(const QString& szName);
    
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);
    
private:
    QMap<QString, CManageConnecter*> m_Plugins;

};

QT_BEGIN_NAMESPACE
#define ManageConnecter_iid "KangLinStudio.Rabbit.RemoteControl.ManageConnecter"
Q_DECLARE_INTERFACE(CManageConnecter, ManageConnecter_iid)
QT_END_NAMESPACE

#endif // CMANAGECONNECTER_H
