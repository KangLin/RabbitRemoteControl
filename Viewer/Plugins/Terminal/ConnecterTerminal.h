#ifndef CCONNECTERTERMINAL_H
#define CCONNECTERTERMINAL_H

#include "ConnecterPluginsTerminal.h"

class CConnecterTerminal : public CConnecterPluginsTerminal
{
    Q_OBJECT
public:
    explicit CConnecterTerminal(CPluginFactory *parent);
    virtual ~CConnecterTerminal();
    
    // CConnecterPluginsTerminal interface
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    CParameterTerminalAppearance* GetPara() override;
    virtual int OnConnect() override;
    virtual int OnDisConnect() override;
    
private:
    CParameterTerminalAppearance* m_pPara;
};

#endif // CCONNECTERTERMINAL_H
