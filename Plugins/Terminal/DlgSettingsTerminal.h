#ifndef DLGSETTINGSTERMINAL_H
#define DLGSETTINGSTERMINAL_H

#include <QDialog>
#include "ParameterTerminal.h"
#include "FrmParameterTerminalAppearanceSettings.h"
#include "FrmParameterTerminalBehavior.h"

namespace Ui {
class CDlgSettingsTerminal;
}

class CDlgSettingsTerminal : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsTerminal(CParameterTerminal *pPara, QWidget *parent = nullptr);
    ~CDlgSettingsTerminal();

private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgSettingsTerminal *ui;
    CParameterTerminal* m_pPara;
    CFrmParameterTerminalAppearanceSettings* m_pFrmParaAppearnce;
    CFrmParameterTerminalBehavior* m_pFrmParaBehavior;
};

#endif // DLGSETTINGSTERMINAL_H
