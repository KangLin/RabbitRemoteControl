// Author: Kang Lin <kl222@126.com>
#pragma once

#include <QDialog>
#include "ParameterTelnet.h"
#include "ParameterTerminalUI.h"

namespace Ui {
class CDlgSettingsTelnet;
}

class CDlgSettingsTelnet : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsTelnet(CParameterTelnet *pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettingsTelnet();
    
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
    void on_pbAddCommand_clicked();

private:
    Ui::CDlgSettingsTelnet *ui;

    CParameterTelnet* m_pPara;
    CParameterTerminalUI* m_pFrmParaAppearnce;
};
