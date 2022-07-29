#include "FrmParameterViewer.h"
#include "ui_FrmParameterViewer.h"

CFrmParameterViewer::CFrmParameterViewer(CParameterViewer *pParameter, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterViewer),
    m_pPara(pParameter)
{
    ui->setupUi(this);

    ui->cbHookKeyboard->setChecked(m_pPara->GetHookKeyboard());
    
    ui->leEncryptKey->setText(m_pPara->GetEncryptKey());
    ui->cbSavePassword->setChecked(m_pPara->GetSavePassword());
    ui->cbEnableViewPassword->setChecked(m_pPara->GetViewPassowrd());
    ui->pbEncryptKey->setEnabled(ui->cbEnableViewPassword->isChecked());
    switch (m_pPara->GetPromptType()) {
    case CParameterViewer::PromptType::Always:
        ui->rbPromptAlways->setChecked(true);
        break;
    case CParameterViewer::PromptType::First:
        ui->rbPromptFirst->setChecked(true);
        break;
    case CParameterViewer::PromptType::No:
        ui->rbPromptNo->setChecked(true);
        break;
    }
}

CFrmParameterViewer::~CFrmParameterViewer()
{
    delete ui;
}

void CFrmParameterViewer::slotAccept()
{
    m_pPara->SetHookKeyboard(ui->cbHookKeyboard->isChecked());
    
    m_pPara->SetEncryptKey(ui->leEncryptKey->text());
    m_pPara->SetSavePassword(ui->cbSavePassword->isChecked());
    m_pPara->SetViewPassowrd(ui->cbEnableViewPassword->isChecked());
    if(ui->rbPromptAlways->isChecked())
        m_pPara->SetPromptType(CParameterViewer::PromptType::Always);
    if(ui->rbPromptFirst->isChecked())
        m_pPara->SetPromptType(CParameterViewer::PromptType::First);
    if(ui->rbPromptNo->isChecked())
        m_pPara->SetPromptType(CParameterViewer::PromptType::No);
}

void CFrmParameterViewer::on_cbEnableViewPassword_clicked(bool checked)
{
    ui->pbEncryptKey->setEnabled(checked);
}

void CFrmParameterViewer::on_pbEncryptKey_clicked()
{
    switch(ui->leEncryptKey->echoMode())
    {
    case QLineEdit::Password:
        ui->leEncryptKey->setEchoMode(QLineEdit::Normal);
        ui->pbEncryptKey->setIcon(QIcon(":/image/EyeOff"));
        break;
    case QLineEdit::Normal:
        ui->leEncryptKey->setEchoMode(QLineEdit::Password);
        ui->pbEncryptKey->setIcon(QIcon(":/image/EyeOn"));
        break;
    default:
        break;
    }
}
