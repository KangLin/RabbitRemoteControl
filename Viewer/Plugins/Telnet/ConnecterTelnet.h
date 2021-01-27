#ifndef CCONNECTERTELNET_H
#define CCONNECTERTELNET_H

#include "ConnecterPluginsTerminal.h"
#include "ParameterTelnet.h"

class CConnecterTelnet : public CConnecterPluginsTerminal
{
    Q_OBJECT
public:
    explicit CConnecterTelnet(CPluginFactory *parent);
    
    // CConnecterPluginsTerminal interface
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual CParameterTerminalAppearance *GetPara() override;
    
private:
    CParameterTelnet* m_pPara;
};

#endif // CCONNECTERTELNET_H
