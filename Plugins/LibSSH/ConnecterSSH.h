#ifndef CCONNECTERSSH_H
#define CCONNECTERSSH_H

#include "ConnecterPluginsTerminal.h"
#include "ParameterSSH.h"

class CConnectSSH;
class CConnecterSSH : public CConnecterPluginsTerminal
{
    Q_OBJECT

public:
    explicit CConnecterSSH(CPluginViewer *parent);
    virtual ~CConnecterSSH();

    virtual CConnect *InstanceConnect() override;
    
protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual CParameterTerminal *GetPara() override;
    
private:
    CParameterSSH* m_pPara;
    friend CConnectSSH;
};

#endif // CCONNECTERSSH_H
