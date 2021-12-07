// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERFREERDP_H
#define CCONNECTERFREERDP_H

#include "Connecter.h"
#include "ConnecterDesktop.h"
#include "freerdp/freerdp.h"
#include "ParameterFreeRdp.h"

class CConnecterFreeRdp : public CConnecterDesktop
{
    Q_OBJECT
public:
    explicit CConnecterFreeRdp(CPluginViewer *parent = nullptr);
    virtual ~CConnecterFreeRdp() override;
    
    CParameterFreeRdp m_ParameterFreeRdp;

public:
    virtual qint16 Version() override;
    
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

    virtual CConnect *InstanceConnect() override;
};

#endif // CCONNECTERFREERDP_H
