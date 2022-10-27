// Author: Kang Lin <kl222@126.com>

#include "DlgInputPassword.h"
#include "ui_DlgInputPassword.h"

CDlgInputPassword::CDlgInputPassword(bool bShow, QString szTitle, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::CDlgInputPassword)
{
    ui->setupUi(this);
    ui->pbShow->setEnabled(bShow);
    setWindowTitle(tr("Input encrypt key"));

    QString szDescript = tr("The encryption key is used to encrypt the password that is saved to the file.");
    if(!szTitle.isEmpty())
        szDescript += tr("If you forget the encryption key, please use input %1.").arg(szTitle);
    ui->lbDescript->setText(szDescript);
    if(szTitle.isEmpty())
        ui->rbPassword->setVisible(false);

    ui->rbPassword->setText(tr("Input %1").arg(szTitle));
}

CDlgInputPassword::~CDlgInputPassword()
{
    delete ui;
}

void CDlgInputPassword::on_pbNo_clicked()
{
    reject();
}

void CDlgInputPassword::on_pbYes_clicked()
{
    accept();
}

int CDlgInputPassword::GetValue(InputType &t, QString &password)
{
    if(ui->rbKey->isChecked()) t = Encrypt;
    if(ui->rbPassword->isChecked()) t = Password;
    password = ui->lePassword->text();
    return 0;
}

void CDlgInputPassword::on_pbShow_clicked()
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
    }
}
