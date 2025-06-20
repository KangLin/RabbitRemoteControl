// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterUI.h"
#include "ParameterTerminal.h"

namespace Ui {
class CFrmParameterTerminalBehavior;
}

class PLUGIN_EXPORT CFrmParameterTerminalBehavior : public CParameterUI
{
    Q_OBJECT
    
public:
    explicit CFrmParameterTerminalBehavior(QWidget *parent = nullptr);
    virtual ~CFrmParameterTerminalBehavior();

    virtual int SetParameter(CParameter *pParameter) override;
    virtual int Accept() override;

private:
    Ui::CFrmParameterTerminalBehavior *ui;
    CParameterTerminal *m_pPara;
};
