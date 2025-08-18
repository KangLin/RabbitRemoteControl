// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>

#include "TerminalParameter.h"
#include "ParameterTerminalUI.h"

namespace Ui {
class CDlgSettingsTerminal;
}

class CDlgSettingsTerminal : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsTerminal(CTerminalParameter *pPara, QWidget *parent = nullptr);
    ~CDlgSettingsTerminal();

private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
    void on_pbShellBrowse_clicked();
    
    void on_cbShell_currentIndexChanged(int index);
    
    void on_pbAddCommand_clicked();
    
    void on_leCommand_editingFinished();
    
    void on_pbDeleteCommands_clicked();
    
    void on_pbHelp_clicked();

private:
    int AddShell(QString szShell, const QString& name = QString());

private:
    Ui::CDlgSettingsTerminal *ui;
    CTerminalParameter* m_pPara;
    CParameterTerminalUI* m_pFrmParaAppearnce;
};

