#include "DlgSettingsTerminal.h"
#include "ui_DlgSettingsTerminal.h"
#include "RabbitCommonLog.h"

#include <QDebug>
#include <qtermwidget.h>

CDlgSettingsTerminal::CDlgSettingsTerminal(CParameterTerminalAppearance *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTerminal),
    m_pPara(pPara)
{
    ui->setupUi(this);

    m_pFrmTerminalAppearanceSettings = new CFrmTerminalAppearanceSettings(m_pPara, this);
    ui->tabWidget->addTab(m_pFrmTerminalAppearanceSettings, tr("Appearance"));
}

CDlgSettingsTerminal::~CDlgSettingsTerminal()
{
    qDebug() << "CDlgSettingsTerminal::~CDlgSettingsTerminal()";
    delete ui;
}

void CDlgSettingsTerminal::on_pbOk_clicked()
{
    if(m_pFrmTerminalAppearanceSettings)
        m_pFrmTerminalAppearanceSettings->AcceptSettings();
    this->accept();
}

void CDlgSettingsTerminal::on_pbCancle_clicked()
{
    this->reject();    
}
