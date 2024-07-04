#include "ParameterUserUI.h"
#include "ui_ParameterUserUI.h"

CParameterUserUI::CParameterUserUI(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::CParameterUserUI),
    m_pUser(nullptr)
{
    ui->setupUi(this);
    ui->leUser->setPlaceholderText(tr("Input user name"));
    ui->lePassword->setPlaceholderText(tr("Input password"));
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
    ui->lePassword->setText(m_pUser->GetPassword());
    ui->pbSave->setChecked(m_pUser->GetSavePassword());
    on_pbSave_clicked();
    ui->pbShow->setEnabled(m_pUser->GetParameterClient()->GetViewPassowrd());
    return 0;
}

void CParameterUserUI::slotAccept()
{
    if(!m_pUser) return;
    m_pUser->SetUser(ui->leUser->text());
    m_pUser->SetPassword(ui->lePassword->text());
    m_pUser->SetSavePassword(ui->pbSave->isChecked());
    return;
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