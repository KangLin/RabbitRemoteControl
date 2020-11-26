#ifndef MANAGECONNECTFREERDP_H
#define MANAGECONNECTFREERDP_H

#include "ManageConnecter.h"
#include <QTranslator>

class CManageConnectFreeRdp : public CManageConnecter
{
    Q_OBJECT
    Q_INTERFACES(CManageConnecter)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID ManageConnecter_iid)
#endif
    
public:
    explicit CManageConnectFreeRdp(QObject *parent = nullptr);
    virtual ~CManageConnectFreeRdp() override;
    
    virtual QString Name() override;
    virtual QString Description() override;
    
    virtual QString Protol() override;
    virtual CConnecter* CreateConnecter(const QString& szProtol) override;
    
private:
    QTranslator m_Translator;
};

#endif // MANAGECONNECTFREERDP_H
