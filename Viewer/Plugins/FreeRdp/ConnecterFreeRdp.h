//! @author: Kang Lin(kl222@126.com)

#ifndef CCONNECTERFREERDP_H
#define CCONNECTERFREERDP_H

#include "Connecter.h"
#include "ConnecterPlugins.h"
#include "freerdp/freerdp.h"

class CConnecterFreeRdp : public CConnecterPlugins
{
    Q_OBJECT
public:
    explicit CConnecterFreeRdp(CPluginFactory *parent = nullptr);
    virtual ~CConnecterFreeRdp() override;
    
    rdpSettings* m_pSettings;
    
    // CConnecter interface
public:
    virtual QString GetServerName() override;
    virtual qint16 Version() override;
    
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;

    virtual CConnect *InstanceConnect() override;
};

#endif // CCONNECTERFREERDP_H
