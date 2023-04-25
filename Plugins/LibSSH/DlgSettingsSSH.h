#ifndef DLGSETTINGSSSH_H
#define DLGSETTINGSSSH_H

#include <QDialog>
#include "ConnecterSSH.h"
#include "FrmParameterTerminalAppearanceSettings.h"
#include "FrmParameterTerminalBehavior.h"
#include "ParameterSSH.h"

namespace Ui {
class CDlgSettingsSSH;
}

class CDlgSettingsSSH : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsSSH(CParameterSSH* pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettingsSSH();
    
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    void on_pbCapFileBrowser_clicked();
    
    void on_leHost_editingFinished();
    
private:
    Ui::CDlgSettingsSSH *ui;
    CConnecterSSH* m_pConnecter;
    CParameterSSH* m_pPara;
    CFrmParameterTerminalAppearanceSettings* m_pFrmParaAppearance;
    CFrmParameterTerminalBehavior* m_pFrmParaBehavior;
};

#endif // DLGSETTINGSSSH_H
