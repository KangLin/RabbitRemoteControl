// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERTIGERVNC_H
#define CCONNECTERTIGERVNC_H

#include "ConnecterDesktop.h"
#include "DlgSettingsTigerVnc.h"

class CConnecterTigerVnc : public CConnecterDesktop
{
    Q_OBJECT

public:
    explicit CConnecterTigerVnc(CPluginViewer *parent);
    virtual ~CConnecterTigerVnc() override;

    virtual qint16 Version() override;

protected:
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) override;
    
    virtual int OnLoad(QDataStream& d) override;
    virtual int OnSave(QDataStream& d) override;

    virtual CConnect* InstanceConnect() override;
    
public:
    CConnectTigerVnc::strPara m_Para;
    CConnect* m_pConnect;
};

#endif // CCONNECTERTIGERVNC_H
