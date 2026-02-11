// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
#include "ParameterPluginUI.h"
#include "ui_ParameterPluginUI.h"
#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"

CParameterPluginUI::CParameterPluginUI(QWidget *parent) :
    CParameterUI(parent),
    ui(new Ui::CParameterPluginUI),
    m_pPara(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(tr("Plugin"));
    if(RabbitCommon::CTools::HasAdministratorPrivilege()) {
        ui->cbPromptAdminPrivilege->setText("");
        ui->cbPromptAdminPrivilege->hide();
    } else {
        ui->cbPromptAdminPrivilege->setText(
            tr("Prompt administrator privilege"));
        ui->cbPromptAdminPrivilege->show();
    }
    ui->pbEncryptKey->setToolTip(tr("Enable view password"));
}

CParameterPluginUI::~CParameterPluginUI()
{
    delete ui;
}

int CParameterPluginUI::Accept()
{
    if(!m_pPara)
        return -1;
    m_pPara->SetCaptureAllKeyboard(ui->gbCaptureAllKeyboard->isChecked());
    m_pPara->SetDesktopShortcutsScript(ui->gpDesktopShortcutsSctipt->isChecked());
    m_pPara->SetDisableDesktopShortcutsScript(ui->leDesktopShortcutsDisableScript->text());
    m_pPara->SetRestoreDesktopShortcutsScript(ui->leDesktopShortcutsRestoreScript->text());
    m_pPara->SetEnableLocalInputMethod(ui->cbEnableLocalInputMethod->isChecked());
    m_pPara->SetPromptAdministratorPrivilege(
        ui->cbPromptAdminPrivilege->isChecked());
    m_pPara->SetEnableSystemUserToUser(ui->cbEnableUserName->isChecked());
    if(ui->rbSaveSettingsToFile->isChecked())
        m_pPara->GetGlobalParameters()->SetSaveSettingsType(CParameterGlobal::SaveSettingsType::File);
    else if(ui->rbSaveSettingsToDatabase->isChecked())
        m_pPara->GetGlobalParameters()->SetSaveSettingsType(CParameterGlobal::SaveSettingsType::Database);
    m_pPara->SetAdaptWindows(
        (CFrmViewer::ADAPT_WINDOWS)ui->cbViewZoom->currentData().toInt());

    m_pPara->SetUseSystemCredential(ui->cbSystemCredential->isChecked());
    m_pPara->SetEncryptKey(ui->leEncryptKey->text());
    m_pPara->SetSavePassword(ui->cbSavePassword->isChecked());
    m_pPara->SetViewPassowrd(ui->cbEnableViewPassword->isChecked());
    if(ui->rbPromptFirst->isChecked())
        m_pPara->SetPromptType(CParameterPlugin::PromptType::First);
    if(ui->rbPromptNo->isChecked())
        m_pPara->SetPromptType(CParameterPlugin::PromptType::No);
    m_pPara->SetShowProtocolPrefix(ui->cbShowPrefix->isChecked());
    m_pPara->SetShowIpPortInName(ui->cbShowIPPort->isChecked());
    return 0;
}

void CParameterPluginUI::on_cbEnableViewPassword_clicked(bool checked)
{
    ui->pbEncryptKey->setEnabled(checked);
}

void CParameterPluginUI::on_pbEncryptKey_clicked()
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

int CParameterPluginUI::SetParameter(CParameter *pParameter)
{
    m_pPara = qobject_cast<CParameterPlugin*>(pParameter);
    if(!m_pPara)
        return -1;
    ui->gbCaptureAllKeyboard->setChecked(m_pPara->GetCaptureAllKeyboard());
    ui->gpDesktopShortcutsSctipt->setEnabled(ui->gbCaptureAllKeyboard->isChecked());
    ui->gpDesktopShortcutsSctipt->setChecked(m_pPara->GetDesktopShortcutsScript());
    ui->leDesktopShortcutsDisableScript->setText(m_pPara->GetDisableDesktopShortcutsScript());
    ui->leDesktopShortcutsRestoreScript->setText(m_pPara->GetRestoreDesktopShortcutsScript());
    ui->cbEnableLocalInputMethod->setChecked(m_pPara->GetEnableLocalInputMethod());
    ui->cbPromptAdminPrivilege->setChecked(
        m_pPara->GetPromptAdministratorPrivilege());
    ui->cbEnableUserName->setChecked(m_pPara->GetEnableSystemUserToUser());
    switch(m_pPara->GetGlobalParameters()->GetSaveSettingsType()) {
    case CParameterGlobal::SaveSettingsType::File:
        ui->rbSaveSettingsToFile->setChecked(true);
        break;
    case CParameterGlobal::SaveSettingsType::Database:
        ui->rbSaveSettingsToDatabase->setChecked(true);
        break;
    }
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
                            tr("Keep aspect ratio to window"),
                            (int)CFrmViewer::ADAPT_WINDOWS::KeepAspectRationToWindow);
    int nIndex = ui->cbViewZoom->findData((int)m_pPara->GetAdaptWindows());
    if(-1 != nIndex)
        ui->cbViewZoom->setCurrentIndex(nIndex);

#if HAVE_QTKEYCHAIN
    ui->cbSystemCredential->setChecked(m_pPara->GetUseSystemCredential());
#else
    ui->cbSystemCredential->hide();
#endif
    ui->leEncryptKey->setText(m_pPara->GetEncryptKey());
    ui->cbSavePassword->setChecked(m_pPara->GetSavePassword());
    ui->cbEnableViewPassword->setChecked(m_pPara->GetViewPassowrd());
    ui->pbEncryptKey->setEnabled(ui->cbEnableViewPassword->isChecked());
    switch (m_pPara->GetPromptType()) {
    case CParameterPlugin::PromptType::First:
        ui->rbPromptFirst->setChecked(true);
        break;
    case CParameterPlugin::PromptType::No:
        ui->rbPromptNo->setChecked(true);
        break;
    }

    ui->cbShowPrefix->setChecked(m_pPara->GetShowProtocolPrefix());
    ui->cbShowIPPort->setChecked(m_pPara->GetShowIpPortInName());
    return 0;
}

void CParameterPluginUI::on_cbSystemCredential_checkStateChanged(const Qt::CheckState &state)
{
    ui->gpEncryptKey->setEnabled(Qt::Unchecked == state);
}

void CParameterPluginUI::on_pbDesktopShortcutsDisable_clicked()
{
    QString szDir = ui->leDesktopShortcutsDisableScript->text();
    if(szDir.isEmpty())
        szDir = RabbitCommon::CDir::Instance()->GetDirData() + QDir::separator() + "Script";
    QString szFile = QFileDialog::getOpenFileName(
        nullptr, tr("Open disable desktop shortcuts script"), szDir);
    if(szFile.isEmpty()) return;
    ui->leDesktopShortcutsDisableScript->setText(szFile);
}

void CParameterPluginUI::on_pbDesktopShortcutsRestore_clicked()
{
    QString szDir = ui->leDesktopShortcutsRestoreScript->text();
    if(szDir.isEmpty())
        szDir = RabbitCommon::CDir::Instance()->GetDirData() + QDir::separator() + "Script";
    QString szFile = QFileDialog::getOpenFileName(
        nullptr, tr("Open restore desktop shortcuts script"), szDir);
    if(szFile.isEmpty()) return;
    ui->leDesktopShortcutsRestoreScript->setText(szFile);
}

void CParameterPluginUI::on_cbCaptureAllKeyboard_checkStateChanged(const Qt::CheckState &arg1)
{
    ui->gpDesktopShortcutsSctipt->setEnabled(Qt::CheckState::Checked == arg1);
}
