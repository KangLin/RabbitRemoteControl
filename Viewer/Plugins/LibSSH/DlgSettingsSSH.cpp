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
    ui->leHost->setText(m_pPara->szHost);
    ui->sbPort->setValue(m_pPara->nPort);
    ui->cbSavePassword->setChecked(m_pPara->bSavePassword);
    ui->leUser->setText(m_pPara->szUser);
    ui->lePassword->setText(m_pPara->szPassword);
    
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
    m_pPara->szHost = ui->leHost->text();
    m_pPara->nPort = ui->sbPort->value();
    m_pPara->bSavePassword = ui->cbSavePassword->isChecked();
    m_pPara->szUser = ui->leUser->text();
    m_pPara->szPassword = ui->lePassword->text();
    
    m_pPara->captrueFile = ui->leCapFile->text();
    
    if(m_pFrmParaAppearance)
        m_pFrmParaAppearance->AcceptSettings();
    if(m_pFrmParaBehavior)
        m_pFrmParaBehavior->AcceptSettings();
    
    accept();
}

void CDlgSettingsSSH::on_pbCancle_clicked()
{
    reject();
}

void CDlgSettingsSSH::on_pbCapFileBrower_clicked()
{
    ui->leCapFile->setText(
            RabbitCommon::CDir::Instance()->GetSaveFileName(this,
                                                            tr("Capture file"),
               QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                                          + QDir::separator() + "capfile.dat"));
}
