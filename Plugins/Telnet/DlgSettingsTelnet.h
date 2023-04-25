#ifndef DLGSETTINGSTELNET_H
#define DLGSETTINGSTELNET_H

#include <QDialog>
#include "ParameterTerminal.h"
#include "FrmParameterTerminalAppearanceSettings.h"
#include "FrmParameterTerminalBehavior.h"

namespace Ui {
class CDlgSettingsTelnet;
}

class CDlgSettingsTelnet : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsTelnet(CParameterTerminal *pPara, QWidget *parent = nullptr);
    ~CDlgSettingsTelnet();
    
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();

    void on_leHost_editingFinished();
    
private:
    Ui::CDlgSettingsTelnet *ui;
    
    CParameterTerminal* m_pPara;
    CFrmParameterTerminalAppearanceSettings* m_pFrmParaAppearance;
    CFrmParameterTerminalBehavior* m_pFrmParaBehavior;
};

#endif // DLGSETTINGSTELNET_H
