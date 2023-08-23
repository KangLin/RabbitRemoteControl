#include "FrmParameterClient.h"
#include "ui_FrmParameterClient.h"

CFrmParameterClient::CFrmParameterClient(CParameterClient *pParameter, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterClient),
    m_pPara(pParameter)
{
    ui->setupUi(this);

    ui->cbHookKeyboard->setChecked(m_pPara->GetHookKeyboard());
    
    ui->leEncryptKey->setText(m_pPara->GetEncryptKey());
    ui->cbSavePassword->setChecked(m_pPara->GetSavePassword());
    ui->cbEnableViewPassword->setChecked(m_pPara->GetViewPassowrd());
    ui->pbEncryptKey->setEnabled(ui->cbEnableViewPassword->isChecked());
    switch (m_pPara->GetPromptType()) {
    case CParameterClient::PromptType::Always:
        ui->rbPromptAlways->setChecked(true);
        break;
    case CParameterClient::PromptType::First:
        ui->rbPromptFirst->setChecked(true);
        break;
    case CParameterClient::PromptType::No:
        ui->rbPromptNo->setChecked(true);
        break;
    }
    ui->cbShowPrefix->setChecked(m_pPara->GetShowProtocolPrefix());
    ui->cbShowIPPort->setChecked(m_pPara->GetShowIpPortInName());
}

CFrmParameterClient::~CFrmParameterClient()
{
    delete ui;
}

void CFrmParameterClient::slotAccept()
{
    m_pPara->SetHookKeyboard(ui->cbHookKeyboard->isChecked());
    
    m_pPara->SetEncryptKey(ui->leEncryptKey->text());
    m_pPara->SetSavePassword(ui->cbSavePassword->isChecked());
    m_pPara->SetViewPassowrd(ui->cbEnableViewPassword->isChecked());
    if(ui->rbPromptAlways->isChecked())
        m_pPara->SetPromptType(CParameterClient::PromptType::Always);
    if(ui->rbPromptFirst->isChecked())
        m_pPara->SetPromptType(CParameterClient::PromptType::First);
    if(ui->rbPromptNo->isChecked())
        m_pPara->SetPromptType(CParameterClient::PromptType::No);
    m_pPara->SetShowProtocolPrefix(ui->cbShowPrefix->isChecked());
    m_pPara->SetShowIpPortInName(ui->cbShowIPPort->isChecked());
}

void CFrmParameterClient::on_cbEnableViewPassword_clicked(bool checked)
{
    ui->pbEncryptKey->setEnabled(checked);
}

void CFrmParameterClient::on_pbEncryptKey_clicked()
{
    switch(ui->leEncryptKey->echoMode())
    {
    case QLineEdit::Password:
        ui->leEncryptKey->setEchoMode(QLineEdit::Normal);
        ui->pbEncryptKey->setIcon(QIcon::fromTheme("eye-off"));
        break;
    case QLineEdit::Normal:
        ui->leEncryptKey->setEchoMode(QLineEdit::Password);
        ui->pbEncryptKey->setIcon(QIcon::fromTheme("eye-on"));
        break;
    default:
        break;
    }
}
