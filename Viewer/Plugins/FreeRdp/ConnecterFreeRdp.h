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
    
    class CParamterFreeRdp: public CParamter
    {
    public:
        rdpSettings* pSettings;
    };
    CParamterFreeRdp m_ParamterFreeRdp;

public:
    virtual qint16 Version() override;
    
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual int OnLoad(QDataStream &d) override;
    virtual int OnSave(QDataStream &d) override;

    virtual CConnect *InstanceConnect() override;
};

#endif // CCONNECTERFREERDP_H
