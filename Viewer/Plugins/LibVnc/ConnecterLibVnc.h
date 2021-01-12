#ifndef CCONNECTERLIBVNC_H
#define CCONNECTERLIBVNC_H

//#include "ConnectThreadLibVnc.h"
#include "ConnecterPlugins.h"
#include "ConnectLibVnc.h"

class CDlgSettingsLibVnc;
class CConnecterLibVnc : public CConnecterPlugins
{
public:
    CConnecterLibVnc(CPluginFactory *parent);
    virtual ~CConnecterLibVnc() override;
    
    // CConnecter interface
public:
    virtual QString GetServerName() override;
    virtual qint16 Version() override;
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    virtual CConnect* InstanceConnect() override;
    
private:   
    CConnectLibVnc::strPara m_Para;
    friend CConnectLibVnc;
    friend CDlgSettingsLibVnc;
};

#endif // CCONNECTERLIBVNC_H
