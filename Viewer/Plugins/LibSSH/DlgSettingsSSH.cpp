#include "DlgSettingsSSH.h"
#include "ui_DlgSettingsSSH.h"

CDlgSettingsSSH::CDlgSettingsSSH(CParameterSSH *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsSSH),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    Q_ASSERT(m_pPara);
    ui->leHost->setText(m_pPara->szHost);
    ui->sbPort->setValue(m_pPara->nPort);
    ui->cbSavePassword->setChecked(m_pPara->bSavePassword);
    ui->leUser->setText(m_pPara->szUser);
    ui->lePassword->setText(m_pPara->szPassword);
    
    m_pFrmTerminalAppearanceSettings = new CFrmTerminalAppearanceSettings(m_pPara, this);
    ui->tabWidget->addTab(m_pFrmTerminalAppearanceSettings, tr("Appearance"));
}

CDlgSettingsSSH::~CDlgSettingsSSH()
{
    delete ui;
}

void CDlgSettingsSSH::on_pbOK_clicked()
{
    m_pPara->szHost = ui->leHost->text();
    m_pPara->nPort = ui->sbPort->value();
    m_pPara->bSavePassword = ui->cbSavePassword->isChecked();
    m_pPara->szUser = ui->leUser->text();
    m_pPara->szPassword = ui->lePassword->text();
    
    if(m_pFrmTerminalAppearanceSettings)
        m_pFrmTerminalAppearanceSettings->AcceptSettings();
    accept();
}

void CDlgSettingsSSH::on_pbCancle_clicked()
{
    reject();
}
