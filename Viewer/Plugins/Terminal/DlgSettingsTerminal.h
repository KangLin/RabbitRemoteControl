#ifndef DLGSETTINGSTERMINAL_H
#define DLGSETTINGSTERMINAL_H

#include <QDialog>
#include "ParameterTerminalAppearance.h"
#include "FrmTerminalAppearanceSettings.h"

namespace Ui {
class CDlgSettingsTerminal;
}

class CDlgSettingsTerminal : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsTerminal(CParameterTerminalAppearance *pPara, QWidget *parent = nullptr);
    ~CDlgSettingsTerminal();

private slots:
    void on_pbOk_clicked();
    void on_pbCancle_clicked();
    
private:
    Ui::CDlgSettingsTerminal *ui;
    CParameterTerminalAppearance* m_pPara;
    CFrmTerminalAppearanceSettings* m_pFrmTerminalAppearanceSettings;
};

#endif // DLGSETTINGSTERMINAL_H
