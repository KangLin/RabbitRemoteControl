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
    
    for(auto t: m_pUser->GetType()) {
        if(CParameterUser::TYPE::None == t) {
            ui->cbType->addItem(
                m_pUser->ConvertTypeToName(CParameterUser::TYPE::None),
                (int)CParameterUser::TYPE::None);
        }
        if(CParameterUser::TYPE::OnlyPassword == t) {
            ui->cbType->addItem(
                m_pUser->ConvertTypeToName(CParameterUser::TYPE::OnlyPassword),
                (int)CParameterUser::TYPE::OnlyPassword);
        }
        if(CParameterUser::TYPE::UserPassword == t) {
            ui->cbType->addItem(
                m_pUser->ConvertTypeToName(CParameterUser::TYPE::UserPassword),
                (int)CParameterUser::TYPE::UserPassword);
        }
        if(CParameterUser::TYPE::PublicKey == t) {
            ui->cbType->addItem(
                m_pUser->ConvertTypeToName(CParameterUser::TYPE::PublicKey),
                (int)CParameterUser::TYPE::PublicKey);
        }
    }
    
    int index = ui->cbType->findData((int)m_pUser->GetUsedType());
    if(-1 == index) {
        qDebug(log) << "Forget set use type?";
    } else
        ui->cbType->setCurrentIndex(index);
    ui->cbType->setVisible(1 < m_pUser->GetType().length());
    ui->lbType->setVisible(1 < m_pUser->GetType().length());
    
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
    
    CParameterUser::TYPE type = static_cast<CParameterUser::TYPE>(ui->cbType->currentData().toInt());
    bool bUser = false;
    bUser = !((CParameterUser::TYPE::OnlyPassword == type)
              ||(CParameterUser::TYPE::None == type));
    ui->lbUser->setVisible(bUser);
    ui->leUser->setVisible(bUser);
    
    bool bPassword = false;
    bPassword = (CParameterUser::TYPE::UserPassword == type)
                    || (CParameterUser::TYPE::OnlyPassword == type);
    ui->lbPassword->setVisible(bPassword);
    ui->lePassword->setVisible(bPassword);
    ui->pbSave->setVisible(bPassword);
    ui->pbShow->setVisible(bPassword);
    
    bool bpublicKey = false;
    bpublicKey = CParameterUser::TYPE::PublicKey == type;
    ui->lbPublicFile->setVisible(bpublicKey);
    ui->lePublicFile->setVisible(bpublicKey);
    ui->pbBrowsePublicFile->setVisible(bpublicKey);
    ui->lbPrivateFile->setVisible(bpublicKey);
    ui->lePrivateFile->setVisible(bpublicKey);
    ui->pbBrowsePrivateFile->setVisible(bpublicKey);
    ui->lbPassphrase->setVisible(bpublicKey);
    ui->lePassphrase->setVisible(bpublicKey);
    ui->pbShowPassphrase->setVisible(bpublicKey);
    ui->pbSavePassphrase->setVisible(bpublicKey);
    ui->cbSystemFile->setVisible(bpublicKey);
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
