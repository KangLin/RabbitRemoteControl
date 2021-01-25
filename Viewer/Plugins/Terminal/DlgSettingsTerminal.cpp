#include "DlgSettingsTerminal.h"
#include "ui_DlgSettingsTerminal.h"
#include "RabbitCommonLog.h"
#include "FrmTerminalAppearanceSettings.h"

#include <QDebug>
#include <qtermwidget.h>

CDlgSettingsTerminal::CDlgSettingsTerminal(CParameterTerminalAppearance *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTerminal),
    m_pPara(pPara)
{
    ui->setupUi(this);
        
    ui->tabWidget->addTab(new CFrmTerminalAppearanceSettings(m_pPara, this), tr("Appearance"));
}

CDlgSettingsTerminal::~CDlgSettingsTerminal()
{
    qDebug() << "CDlgSettingsTerminal::~CDlgSettingsTerminal()";
    delete ui;
}

void CDlgSettingsTerminal::on_pbOk_clicked()
{
    this->accept();
}

void CDlgSettingsTerminal::on_pbCancle_clicked()
{
    this->reject();    
}
