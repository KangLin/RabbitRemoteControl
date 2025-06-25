#ifndef DLGSETTINGSTERMINAL_H
#define DLGSETTINGSTERMINAL_H

#include <QDialog>
#include "ParameterTerminalBase.h"
#include "ParameterTerminalUI.h"

namespace Ui {
class CDlgSettingsTerminal;
}

class CDlgSettingsTerminal : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsTerminal(CParameterTerminalBase *pPara, QWidget *parent = nullptr);
    ~CDlgSettingsTerminal();

private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgSettingsTerminal *ui;
    CParameterTerminalBase* m_pPara;
    CParameterTerminalUI* m_pFrmParaAppearnce;
};

#endif // DLGSETTINGSTERMINAL_H
