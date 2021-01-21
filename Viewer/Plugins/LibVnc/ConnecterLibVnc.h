#ifndef CCONNECTERLIBVNC_H
#define CCONNECTERLIBVNC_H

#include "ConnecterPlugins.h"
#include "ConnectLibVnc.h"

class CDlgSettingsLibVnc;
class CConnecterLibVnc : public CConnecterPlugins
{
public:
    explicit CConnecterLibVnc(CPluginFactory *parent);
    virtual ~CConnecterLibVnc() override;

public:
    virtual qint16 Version() override;

protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual int OnLoad(QDataStream& d) override;
    virtual int OnSave(QDataStream& d) override;
    virtual CConnect* InstanceConnect() override;
    
private:
    CConnectLibVnc::strPara m_Para;
    
    friend CConnectLibVnc;
    friend CDlgSettingsLibVnc;
};

#endif // CCONNECTERLIBVNC_H
