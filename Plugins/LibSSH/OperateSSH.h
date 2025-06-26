#ifndef CCONNECTERSSH_H
#define CCONNECTERSSH_H

#include "OperateTerminal.h"
#include "ParameterTerminalBase.h"

class COperateSSH : public COperateTerminal
{
    Q_OBJECT

public:
    explicit COperateSSH(CPlugin *parent);
    virtual ~COperateSSH();

    virtual CBackend *InstanceBackend() override;
    
private:
    CParameterTerminalBase m_Parameters;
    
    // COperate interface
protected:
    virtual int Initial() override;
    virtual int Clean() override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
};

#endif // CCONNECTERSSH_H
