// Author: Kang Lin <kl222@126.com>

#pragma once
#include "OperateTerminal.h"

class CTerminal : public COperateTerminal
{
    Q_OBJECT

public:
    explicit CTerminal(CPlugin *parent);
    virtual ~CTerminal() override;

public:
    [[nodiscard]] virtual CBackend *InstanceBackend() override;

    virtual int Start() override;
    virtual int Stop() override;

private:
   [[nodiscard]] virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
   CParameterTerminalBase m_Parameters;
    
    // COperate interface
protected:
    virtual int Initial() override;

    // COperate interface
public:
    virtual const QString Name() override;
};
