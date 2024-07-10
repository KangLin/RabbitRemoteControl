#include "ParameterUserUI.h"
#include "ui_ParameterUserUI.h"
#include "QFileDialog"

CParameterUserUI::CParameterUserUI(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::CParameterUserUI),
    m_pUser(nullptr)
{
    ui->setupUi(this);
    ui->leUser->setPlaceholderText(tr("Input user name"));
    ui->lePassword->setPlaceholderText(tr("Input password"));
    ui->lePublicFile->setPlaceholderText(tr("Please input the public key file"));
    ui->lePrivateFile->setPlaceholderText(tr("Please input the private key file"));
    ui->lePassphrase->setPlaceholderText(tr("Please input the passphrase of private key file"));
    // 调整窗口的初始大小。
    on_cbType_currentIndexChanged(0);
}

CParameterUserUI::~CParameterUserUI()
{
    delete ui;
}

int CParameterUserUI::SetParameter(CParameterUser *pParameter)
{
    m_pUser = pParameter;
    if(!m_pUser) return -1;
    
    ui->leUser->setText(m_pUser->GetUser());
    
    int nType = 0;
    if((int)CParameterUser::TYPE::None & (int)m_pUser->GetType()) {
        ui->cbType->addItem(tr("None"), (int)CParameterUser::TYPE::None);
        nType++;
    }
    if((int)CParameterUser::TYPE::Password & (int)m_pUser->GetType()) {
        ui->cbType->addItem(tr("Password"), (int)CParameterUser::TYPE::Password);
        nType++;
    }
    if((int)CParameterUser::TYPE::PublicKey & (int)m_pUser->GetType()) {
        ui->cbType->addItem(tr("PublicKey"), (int)CParameterUser::TYPE::PublicKey);
        nType++;
    }
    
    int index = ui->cbType->findData((int)m_pUser->GetUsedType());
    ui->cbType->setCurrentIndex(index);
    ui->cbType->setVisible(1 < nType);
    ui->lbType->setVisible(1 < nType);

    ui->lePassword->setText(m_pUser->GetPassword());
    ui->pbSave->setChecked(m_pUser->GetSavePassword());
    on_pbSave_clicked();
    ui->pbShow->setEnabled(m_pUser->GetParameterClient()->GetViewPassowrd());
    
    ui->lePublicFile->setText(m_pUser->GetPublicKeyFile());
    ui->lePrivateFile->setText(m_pUser->GetPrivateKeyFile());
    ui->lePassphrase->setText(m_pUser->GetPassphrase());
    ui->pbSavePassphrase->setChecked(m_pUser->GetSavePassphrase());
    on_pbSavePassphrase_clicked();
    ui->pbShowPassphrase->setEnabled(m_pUser->GetParameterClient()->GetViewPassowrd());

    return 0;
}

int CParameterUserUI::slotAccept()
{
    if(!m_pUser) return -1;
    
    m_pUser->SetUsedType((CParameterUser::TYPE)ui->cbType->currentData().toInt());
    
    m_pUser->SetUser(ui->leUser->text());
    m_pUser->SetPassword(ui->lePassword->text());
    m_pUser->SetSavePassword(ui->pbSave->isChecked());
    
    m_pUser->SetPublicKeyFile(ui->lePublicFile->text());
    m_pUser->SetPrivateKeyFile(ui->lePrivateFile->text());
    m_pUser->SetPassphrase(ui->lePassphrase->text());
    m_pUser->SetSavePassphrase(ui->pbSavePassphrase->isChecked());
    
    return 0;
}

void CParameterUserUI::on_pbShow_clicked()
{
    switch(ui->lePassword->echoMode())
    {
    case QLineEdit::Password:
        ui->lePassword->setEchoMode(QLineEdit::Normal);
        ui->pbShow->setIcon(QIcon::fromTheme("eye-off"));
        break;
    case QLineEdit::Normal:
        ui->lePassword->setEchoMode(QLineEdit::Password);
        ui->pbShow->setIcon(QIcon::fromTheme("eye-on"));
        break;
    default:
        ui->pbShow->setIcon(QIcon::fromTheme("eye-on"));
    }
}

void CParameterUserUI::on_pbSave_clicked()
{
    if(!parent()->inherits("CParameterNetUI")) return;
    if(ui->pbSave->isChecked())
    {
        ui->lePassword->setEnabled(true);
        ui->lePassword->setPlaceholderText(tr("Input password"));
    } else {
        ui->lePassword->setPlaceholderText(tr("Please checked save password to enable"));
        ui->lePassword->setEnabled(false);
    }
}

void CParameterUserUI::on_pbBrowsePublicFile_clicked()
{
    QString dir("~/.ssh");
    QString szFile = QFileDialog::getOpenFileName(this, tr("Open public key file"), dir);
    if(szFile.isEmpty())
        return;
    ui->lePublicFile->setText(szFile);
}

void CParameterUserUI::on_pbBrowsePrivateFile_clicked()
{
    QString dir("~/.ssh");
    QString szFile = QFileDialog::getOpenFileName(this, tr("Open public key file"), dir);
    if(szFile.isEmpty())
        return;
    ui->lePrivateFile->setText(szFile);
}

void CParameterUserUI::on_cbType_currentIndexChanged(int index)
{
    int type = ui->cbType->currentData().toInt();
    ui->lbPassword->setVisible((int)CParameterUser::TYPE::Password & type);
    ui->lePassword->setVisible((int)CParameterUser::TYPE::Password & type);
    ui->pbSave->setVisible((int)CParameterUser::TYPE::Password & type);
    ui->pbShow->setVisible((int)CParameterUser::TYPE::Password & type);
    
    ui->lbPublicFile->setVisible((int)CParameterUser::TYPE::PublicKey & type);
    ui->lePublicFile->setVisible((int)CParameterUser::TYPE::PublicKey & type);
    ui->pbBrowsePublicFile->setVisible((int)CParameterUser::TYPE::PublicKey & type);
    ui->lbPrivateFile->setVisible((int)CParameterUser::TYPE::PublicKey & type);
    ui->lePrivateFile->setVisible((int)CParameterUser::TYPE::PublicKey & type);
    ui->pbBrowsePrivateFile->setVisible((int)CParameterUser::TYPE::PublicKey & type);
    ui->lbPassphrase->setVisible((int)CParameterUser::TYPE::PublicKey & type);
    ui->lePassphrase->setVisible((int)CParameterUser::TYPE::PublicKey & type);
    ui->pbShowPassphrase->setVisible((int)CParameterUser::TYPE::PublicKey & type);
    ui->pbSavePassphrase->setVisible((int)CParameterUser::TYPE::PublicKey & type);
}

void CParameterUserUI::on_pbShowPassphrase_clicked()
{
    switch(ui->lePassphrase->echoMode())
    {
    case QLineEdit::Password:
        ui->lePassphrase->setEchoMode(QLineEdit::Normal);
        ui->pbShowPassphrase->setIcon(QIcon::fromTheme("eye-off"));
        break;
    case QLineEdit::Normal:
        ui->lePassphrase->setEchoMode(QLineEdit::Password);
        ui->pbShowPassphrase->setIcon(QIcon::fromTheme("eye-on"));
        break;
    default:
        ui->pbShowPassphrase->setIcon(QIcon::fromTheme("eye-on"));
    }
}

void CParameterUserUI::on_pbSavePassphrase_clicked()
{
    if(!parent()->inherits("CParameterNetUI")) return;
    if(ui->pbSavePassphrase->isChecked())
    {
        ui->lePassphrase->setEnabled(true);
        ui->lePassphrase->setPlaceholderText(tr("Input passphrase"));
    } else {
        ui->lePassphrase->setPlaceholderText(tr("Please checked save passphrase to enable"));
        ui->lePassphrase->setEnabled(false);
    }
}
