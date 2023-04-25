#include "DlgSettingsSSH.h"
#include "ui_DlgSettingsSSH.h"
#include "RabbitCommonDir.h"
#include <QStandardPaths>

CDlgSettingsSSH::CDlgSettingsSSH(CParameterSSH *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsSSH),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    Q_ASSERT(m_pPara);
    ui->leName->setText(m_pPara->GetName());
    ui->leHost->setText(m_pPara->GetHost());
    ui->spPort->setValue(m_pPara->GetPort());
    ui->cbSavePassword->setChecked(m_pPara->GetSavePassword());
    ui->leUser->setText(m_pPara->GetUser());
    ui->lePassword->setText(m_pPara->GetPassword());
    
    ui->leCapFile->setText(m_pPara->captrueFile);
    
    m_pFrmParaAppearance =
            new CFrmParameterTerminalAppearanceSettings(m_pPara, this);
    if(m_pFrmParaAppearance)
        ui->tabWidget->addTab(m_pFrmParaAppearance,
                              m_pFrmParaAppearance->windowTitle());
    m_pFrmParaBehavior = new CFrmParameterTerminalBehavior(m_pPara, this);
    if(m_pFrmParaBehavior)
        ui->tabWidget->addTab(m_pFrmParaBehavior, m_pFrmParaBehavior->windowTitle());
}

CDlgSettingsSSH::~CDlgSettingsSSH()
{
    delete ui;
}

void CDlgSettingsSSH::on_pbOK_clicked()
{
    m_pPara->SetName(ui->leName->text());
    m_pPara->SetHost(ui->leHost->text());
    m_pPara->SetPort(ui->spPort->value());
    m_pPara->SetSavePassword(ui->cbSavePassword->isChecked());
    m_pPara->SetUser(ui->leUser->text());
    m_pPara->SetPassword(ui->lePassword->text());
    
    m_pPara->captrueFile = ui->leCapFile->text();
    
    if(m_pFrmParaAppearance)
        m_pFrmParaAppearance->AcceptSettings();
    if(m_pFrmParaBehavior)
        m_pFrmParaBehavior->AcceptSettings();
    
    accept();
}

void CDlgSettingsSSH::on_pbCancel_clicked()
{
    reject();
}

void CDlgSettingsSSH::on_pbCapFileBrowser_clicked()
{
    ui->leCapFile->setText(
            RabbitCommon::CDir::Instance()->GetSaveFileName(this,
                                                            tr("Capture file"),
               QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                                          + QDir::separator() + "capfile.dat"));
}

void CDlgSettingsSSH::on_leHost_editingFinished()
{
    auto s = ui->leHost->text().split(":");
    if(s.size() == 2)
    {
        ui->spPort->setValue(s[1].toUInt());
        ui->leHost->setText(s[0]);
    }
}
