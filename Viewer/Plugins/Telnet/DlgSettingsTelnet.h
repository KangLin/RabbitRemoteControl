#ifndef DLGSETTINGSTELNET_H
#define DLGSETTINGSTELNET_H

#include <QDialog>
#include "ParameterTerminalAppearance.h"
#include "FrmTerminalAppearanceSettings.h"

namespace Ui {
class CDlgSettingsTelnet;
}

class CDlgSettingsTelnet : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsTelnet(CParameterTerminalAppearance *pPara, QWidget *parent = nullptr);
    ~CDlgSettingsTelnet();
    
private slots:
    void on_pbOK_clicked();
    void on_pbCancle_clicked();
    
private:
    Ui::CDlgSettingsTelnet *ui;
    
    CParameterTerminalAppearance* m_pPara;
    CFrmTerminalAppearanceSettings* m_pFrmTerminalAppearanceSettings;
};

#endif // DLGSETTINGSTELNET_H
