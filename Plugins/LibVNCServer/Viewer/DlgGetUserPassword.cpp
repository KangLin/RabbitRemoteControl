#include "DlgGetUserPassword.h"
#include "ui_DlgGetUserPassword.h"

CDlgGetUserPassword::CDlgGetUserPassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgGetUserPassword)
{
    ui->setupUi(this);
}

CDlgGetUserPassword::~CDlgGetUserPassword()
{
    delete ui;
}

const QString &CDlgGetUserPassword::GetUser() const
{
    return m_User;
}

void CDlgGetUserPassword::SetUser(const QString &NewUser)
{
    m_User = NewUser;
}

const QString &CDlgGetUserPassword::GetPassword() const
{
    return m_Password;
}

void CDlgGetUserPassword::SetPassword(const QString &NewPassword)
{
    m_Password = NewPassword;
}

bool CDlgGetUserPassword::GetSavePassword() const
{
    return m_SavePassword;
}

void CDlgGetUserPassword::SetSavePassword(bool NewSave)
{
    m_SavePassword = NewSave;
}

void CDlgGetUserPassword::showEvent(QShowEvent *event)
{
    ui->leUser->setText(GetUser());
    ui->lePassword->setText(GetPassword());
    ui->cbSavePassword->setChecked(GetSavePassword());
    ui->lbText->setText(GetText());
}

void CDlgGetUserPassword::on_pbOK_clicked()
{
    SetUser(ui->leUser->text());
    SetPassword(ui->lePassword->text());
    SetSavePassword(ui->cbSavePassword->isChecked());            
    accept();
}

void CDlgGetUserPassword::on_pbCancel_clicked()
{
    reject();
}

const QString &CDlgGetUserPassword::GetText() const
{
    return m_Text;
}

void CDlgGetUserPassword::SetText(const QString &newText)
{
    m_Text = newText;
}
