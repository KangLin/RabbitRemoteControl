#include "DlgSettingsTelnet.h"
#include "ui_DlgSettingsTelnet.h"

CDlgSettingsTelnet::CDlgSettingsTelnet(CParameterTerminal *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTelnet),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    ui->leName->setText(m_pPara->GetName());
    ui->leHost->setText(m_pPara->GetHost());
    ui->spPort->setValue(m_pPara->GetPort());
    
    m_pFrmParaAppearance =
            new CFrmParameterTerminalAppearanceSettings(m_pPara, this);
    if(m_pFrmParaAppearance)
        ui->tabWidget->addTab(m_pFrmParaAppearance,
                              m_pFrmParaAppearance->windowTitle());

    m_pFrmParaBehavior = new CFrmParameterTerminalBehavior(m_pPara, this);
    if(m_pFrmParaBehavior)
        ui->tabWidget->addTab(m_pFrmParaBehavior, m_pFrmParaBehavior->windowTitle());
}

CDlgSettingsTelnet::~CDlgSettingsTelnet()
{
    delete ui;
}

void CDlgSettingsTelnet::on_pbOK_clicked()
{
    m_pPara->SetName(ui->leHost->text());
    m_pPara->SetHost(ui->leHost->text());
    m_pPara->SetPort(ui->spPort->value());
    
    if(m_pFrmParaAppearance)
        m_pFrmParaAppearance->AcceptSettings();
    if(m_pFrmParaBehavior)
        m_pFrmParaBehavior->AcceptSettings();
    this->accept();
}

void CDlgSettingsTelnet::on_pbCancel_clicked()
{
    this->reject();
}

void CDlgSettingsTelnet::on_leHost_editingFinished()
{
    auto s = ui->leHost->text().split(":");
    if(s.size() == 2)
    {
        ui->spPort->setValue(s[1].toUInt());
        ui->leHost->setText(s[0]);
    }
}
