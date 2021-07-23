#ifndef CCONNECTERTERMINAL_H
#define CCONNECTERTERMINAL_H

#include "ConnecterPluginsTerminal.h"

class CConnecterTerminal : public CConnecterPluginsTerminal
{
    Q_OBJECT
public:
    explicit CConnecterTerminal(CPlugin *parent);
    virtual ~CConnecterTerminal();
    
    // CConnecterPluginsTerminal interface
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    CParameterTerminal* GetPara() override;
    virtual int OnConnect() override;
    virtual int OnDisConnect() override;
    
private:
    CParameterTerminal* m_pPara;
};

#endif // CCONNECTERTERMINAL_H
