#include "FrmParameterClient.h"
#include "ui_FrmParameterClient.h"
#include <RabbitCommonTools.h>

CFrmParameterClient::CFrmParameterClient(QWidget *parent) :
    CParameterUI(parent),
    ui(new Ui::CFrmParameterClient),
    m_pPara(nullptr)
{
    ui->setupUi(this);
    if(RabbitCommon::CTools::HasAdministratorPrivilege()) {
        ui->cbHookKeyboard->setText(
            tr("Hook: capture system key"));
        ui->cbPromptAdminPrivilege->setText("");
        ui->cbPromptAdminPrivilege->hide();
    } else {
        ui->cbHookKeyboard->setText(
            tr("Hook: capture system key(System shortcuts is need administrator privilege)"));
        ui->cbPromptAdminPrivilege->setText(
            tr("Prompt administrator privilege"));
        ui->cbPromptAdminPrivilege->show();
    }
}

CFrmParameterClient::~CFrmParameterClient()
{
    delete ui;
}

int CFrmParameterClient::Accept()
{
    if(!m_pPara)
        return -1;
    m_pPara->SetNativeWindowReceiveKeyboard(
        ui->cbNativeWindowReceiveKeyboard->isChecked());
    m_pPara->SetHookKeyboard(ui->cbHookKeyboard->isChecked());
    m_pPara->SetPromptAdministratorPrivilege(
        ui->cbPromptAdminPrivilege->isChecked());
    m_pPara->SetEnableSystemUserToUser(ui->cbEnableUserName->isChecked());
    m_pPara->SetAdaptWindows(
        (CFrmViewer::ADAPT_WINDOWS)ui->cbViewZoom->currentData().toInt());

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
    return 0;
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

int CFrmParameterClient::SetParameter(CParameter *pParameter)
{
    m_pPara = qobject_cast<CParameterClient*>(pParameter);
    if(!m_pPara)
        return -1;
    ui->cbNativeWindowReceiveKeyboard->setChecked(
        m_pPara->GetNativeWindowReceiveKeyboard());
    ui->cbHookKeyboard->setChecked(m_pPara->GetHookKeyboard());
    ui->cbPromptAdminPrivilege->setChecked(
        m_pPara->GetPromptAdministratorPrivilege());
    ui->cbEnableUserName->setChecked(m_pPara->GetEnableSystemUserToUser());

    ui->cbViewZoom->addItem(QIcon::fromTheme("zoom-original"),
                            tr("Original"),
                            (int)CFrmViewer::ADAPT_WINDOWS::Original);
    /*ui->cbViewZoom->addItem(tr("Original Center"),
                            (int)CFrmViewer::ADAPT_WINDOWS::OriginalCenter);*/
    ui->cbViewZoom->addItem(QIcon::fromTheme("zoom"), tr("Zoom"),
                            (int)CFrmViewer::ADAPT_WINDOWS::Zoom);
    ui->cbViewZoom->addItem(QIcon::fromTheme("zoom-fit-best"),
                            tr("Zoom to window"),
                            (int)CFrmViewer::ADAPT_WINDOWS::ZoomToWindow);
    ui->cbViewZoom->addItem(QIcon::fromTheme("zoom-aspect-ratio"),
                            tr("Keep desktop aspectration adapt to windows"),
                            (int)CFrmViewer::ADAPT_WINDOWS::KeepAspectRationToWindow);
    int nIndex = ui->cbViewZoom->findData((int)m_pPara->GetAdaptWindows());
    if(-1 != nIndex)
        ui->cbViewZoom->setCurrentIndex(nIndex);

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
    return 0;
}
