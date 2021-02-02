#ifndef DLGSETTINGSTELNET_H
#define DLGSETTINGSTELNET_H

#include <QDialog>
#include "ParameterTerminal.h"
#include "FrmTerminalAppearanceSettings.h"

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
    void on_pbCancle_clicked();
    
private:
    Ui::CDlgSettingsTelnet *ui;
    
    CParameterTerminal* m_pPara;
    CFrmTerminalAppearanceSettings* m_pFrmTerminalAppearanceSettings;
};

#endif // DLGSETTINGSTELNET_H
