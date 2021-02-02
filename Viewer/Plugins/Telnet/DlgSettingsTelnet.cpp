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
    m_pPara->szHost = ui->leHost->text();
    m_pPara->nPort = ui->spPort->value();
    
    if(m_pFrmParaAppearance)
        m_pFrmParaAppearance->AcceptSettings();
    if(m_pFrmParaBehavior)
        m_pFrmParaBehavior->AcceptSettings();
    this->accept();
}

void CDlgSettingsTelnet::on_pbCancle_clicked()
{
    this->reject();
}
