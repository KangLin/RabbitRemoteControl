#include "DlgSettingsTelnet.h"
#include "ui_DlgSettingsTelnet.h"

CDlgSettingsTelnet::CDlgSettingsTelnet(CParameterTerminal *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTelnet),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    ui->leHost->setText(m_pPara->szHost);
    ui->spPort->setValue(m_pPara->nPort);
    
    m_pFrmTerminalAppearanceSettings = new CFrmTerminalAppearanceSettings(m_pPara, this);
    ui->tabWidget->addTab(m_pFrmTerminalAppearanceSettings, tr("Appearance"));
}

CDlgSettingsTelnet::~CDlgSettingsTelnet()
{
    delete ui;
}

void CDlgSettingsTelnet::on_pbOK_clicked()
{
    m_pPara->szHost = ui->leHost->text();
    m_pPara->nPort = ui->spPort->value();
    
    if(m_pFrmTerminalAppearanceSettings)
        m_pFrmTerminalAppearanceSettings->AcceptSettings();
    this->accept();
}

void CDlgSettingsTelnet::on_pbCancle_clicked()
{
    this->reject();
}
