#include "ParameterUserUI.h"
#include "ui_ParameterUserUI.h"
#include "QFileDialog"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Parameter.User.UI")

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
    on_cbSystemFile_stateChanged(ui->cbSystemFile->isChecked());
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
    if((int)CParameterUser::TYPE::UserPassword & (int)m_pUser->GetType()) {
        ui->cbType->addItem(tr("User and Password"), (int)CParameterUser::TYPE::UserPassword);
        nType++;
    }
    if((int)CParameterUser::TYPE::PublicKey & (int)m_pUser->GetType()) {
        ui->cbType->addItem(tr("Public Key"), (int)CParameterUser::TYPE::PublicKey);
        nType++;
    }
    if((int)CParameterUser::TYPE::OnlyPassword & (int)m_pUser->GetType()) {
        ui->cbType->addItem(tr("Password"), (int)CParameterUser::TYPE::OnlyPassword);
        nType++;
    }
    
    int index = ui->cbType->findData((int)m_pUser->GetUsedType());
    if(-1 == index) {
        qDebug(log) << "Forget set use type?";
    } else
        ui->cbType->setCurrentIndex(index);
    ui->cbType->setVisible(1 < nType);
    ui->lbType->setVisible(1 < nType);
    
    qDebug(log) << "CParameterUserUI::SetParameter:" << index;
    
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
    Q_UNUSED(index);
    qDebug(log) << "CParameterUserUI::on_cbType_currentIndexChanged:" << index;
    
    int type = ui->cbType->currentData().toInt();
    
    ui->lbUser->setVisible(!((int)CParameterUser::TYPE::OnlyPassword & type));
    ui->leUser->setVisible(!((int)CParameterUser::TYPE::OnlyPassword & type));
    
    bool bVisible = false;
    bVisible = ((int)CParameterUser::TYPE::UserPassword & type)
                    | ((int)CParameterUser::TYPE::OnlyPassword & type);
    ui->lbPassword->setVisible(bVisible);
    ui->lePassword->setVisible(bVisible);
    ui->pbSave->setVisible(bVisible);
    ui->pbShow->setVisible(bVisible);
    
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
    ui->cbSystemFile->setVisible((int)CParameterUser::TYPE::PublicKey & type);
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

void CParameterUserUI::on_cbSystemFile_stateChanged(int arg1)
{
    ui->lePublicFile->setEnabled(!ui->cbSystemFile->isChecked());
    ui->pbBrowsePublicFile->setEnabled(!ui->cbSystemFile->isChecked());
    ui->lePrivateFile->setEnabled(!ui->cbSystemFile->isChecked());
    ui->pbBrowsePrivateFile->setEnabled(!ui->cbSystemFile->isChecked());
}
