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

public:
    virtual qint16 Version() override;
    
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;

    virtual CConnect *InstanceConnect() override;
    
private:
    CParameterFreeRdp m_ParameterFreeRdp;
};

#endif // CCONNECTERFREERDP_H
