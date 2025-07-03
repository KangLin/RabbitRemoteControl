#include "DlgSettingsTelnet.h"
#include "ui_DlgSettingsTelnet.h"

CDlgSettingsTelnet::CDlgSettingsTelnet(CParameterTelnet *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsTelnet)
    , m_pPara(pPara)
{
    ui->setupUi(this);

    Q_ASSERT(m_pPara);

    ui->leLogin->setText(pPara->GetLogin());
    ui->lePassword->setText(pPara->GetPassword());

    ui->wNet->SetParameter(&m_pPara->m_Net);
    m_pFrmParaAppearnce =
        new CParameterTerminalUI(this);
    if(m_pFrmParaAppearnce) {
        m_pFrmParaAppearnce->SetParameter(&m_pPara->m_Terminal);
        ui->tabWidget->addTab(m_pFrmParaAppearnce,
                              m_pFrmParaAppearnce->windowTitle());
    }
}

CDlgSettingsTelnet::~CDlgSettingsTelnet()
{
    delete ui;
}

void CDlgSettingsTelnet::on_pbOK_clicked()
{
    if(!ui->wNet->CheckValidity(true)) {
        ui->tabWidget->setCurrentIndex(0);
        return;
    }

    ui->wNet->Accept();
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->Accept();

    m_pPara->SetLogin(ui->leLogin->text());
    m_pPara->SetPassword(ui->lePassword->text());

    accept();
}

void CDlgSettingsTelnet::on_pbCancel_clicked()
{
    reject();
}
