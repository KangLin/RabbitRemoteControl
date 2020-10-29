#ifndef CMANAGECONNECTTIGERVNC_H
#define CMANAGECONNECTTIGERVNC_H

#include "ManageConnecter.h"

class CManageConnectTigerVnc : public CManageConnecter
{
    Q_OBJECT
    Q_INTERFACES(CManageConnecter)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID ManageConnecter_iid)
#endif
    
public:
    explicit CManageConnectTigerVnc(QObject *parent = nullptr);
    virtual ~CManageConnectTigerVnc();
    
    virtual QString Name();
    virtual QString Description();
    
    virtual CConnecter* CreateConnecter(const QString& szName);
};

#endif // CMANAGECONNECTTIGERVNC_H
