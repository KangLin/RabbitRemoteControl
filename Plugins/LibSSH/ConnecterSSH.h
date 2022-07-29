#ifndef CCONNECTERSSH_H
#define CCONNECTERSSH_H

#include "ConnecterTerminal.h"
#include "ParameterSSH.h"

class CConnectSSH;
class CConnecterSSH : public CConnecterTerminal
{
    Q_OBJECT

public:
    explicit CConnecterSSH(CPluginClient *parent);
    virtual ~CConnecterSSH();

    virtual CConnect *InstanceConnect() override;
    
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    
private:
    friend CConnectSSH;
};

#endif // CCONNECTERSSH_H
