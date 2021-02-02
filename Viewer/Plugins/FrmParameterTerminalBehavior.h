#ifndef FRMPARAMETERTERMINALBEHAVIOR_H
#define FRMPARAMETERTERMINALBEHAVIOR_H

#include <QWidget>
#include "ParameterTerminal.h"
#include "rabbitremotecontrolterminal_export.h"

namespace Ui {
class CFrmParameterTerminalBehavior;
}

class RABBITREMOTECONTROLTERMINAL_EXPORT CFrmParameterTerminalBehavior : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmParameterTerminalBehavior(CParameterTerminal* pPara, QWidget *parent = nullptr);
    virtual ~CFrmParameterTerminalBehavior();
    int AcceptSettings();
    
private:
    Ui::CFrmParameterTerminalBehavior *ui;
    CParameterTerminal *m_pPara;
};

#endif // FRMPARAMETERTERMINALBEHAVIOR_H
