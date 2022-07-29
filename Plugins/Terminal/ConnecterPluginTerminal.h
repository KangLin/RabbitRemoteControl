#ifndef CCONNECTERTERMINAL_H
#define CCONNECTERTERMINAL_H

#include "ConnecterTerminal.h"

class CConnecterPluginTerminal : public CConnecterTerminal
{
    Q_OBJECT
public:
    explicit CConnecterPluginTerminal(CPluginClient *parent);
    virtual ~CConnecterPluginTerminal() override;
    
    // CConnecterTerminal interface
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual int OnConnect() override;
    virtual int OnDisConnect() override;
};

#endif // CCONNECTERTERMINAL_H
