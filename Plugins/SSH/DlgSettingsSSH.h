// Author: Kang Lin <kl222@126.com>
#pragma once

#include <QDialog>
#include "OperateSSH.h"
#include "ParameterTerminalBase.h"
#include "ParameterTerminalUI.h"

namespace Ui {
class CDlgSettingsSSH;
}

class CDlgSettingsSSH : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsSSH(CParameterTerminalSSH* pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettingsSSH();
    
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgSettingsSSH *ui;
    CParameterTerminalSSH* m_pPara;
    CParameterTerminalUI* m_pFrmParaAppearnce;
};
