#ifndef CMANAGECONNECTER_H
#define CMANAGECONNECTER_H

#include <QObject>
#include <QDir>
#include <QMap>
#include <QList>

#include "Connecter.h"

class RABBITREMOTECONTROL_EXPORT CManageConnecter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString m_szName READ Name)
    Q_PROPERTY(QString m_szDescription READ Description)
    
public:
    explicit CManageConnecter(QObject *parent = nullptr);
    virtual ~CManageConnecter();
    
    // Plugin name
    virtual QString Name();
    virtual QString Description();
    
    virtual QString Protol();
        
    // Return CConnecter pointer, the owner is caller
    virtual CConnecter* CreateConnecter(const QString& szProtol);
    
    virtual CConnecter* LoadConnecter(const QString& szFile);
    virtual int SaveConnecter(const QString& szFile, CConnecter* pConnecter);
    
    int LoadPlugins();
    int FindPlugins(QDir dir, QStringList filters);
    
    QList<CManageConnecter*> GetManageConnecter();
    
private:
    QMap<QString, CManageConnecter*> m_Plugins;
    
    qint8 m_FileVersion;
};

QT_BEGIN_NAMESPACE
#define ManageConnecter_iid "KangLinStudio.Rabbit.RemoteControl.ManageConnecter"
Q_DECLARE_INTERFACE(CManageConnecter, ManageConnecter_iid)
QT_END_NAMESPACE

#endif // CMANAGECONNECTER_H
