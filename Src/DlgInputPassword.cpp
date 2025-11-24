// Author: Kang Lin <kl222@126.com>

#include "DlgInputPassword.h"
#include "ui_DlgInputPassword.h"

CDlgInputPassword::CDlgInputPassword(bool bShow, bool bStore, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::CDlgInputPassword)
{
    ui->setupUi(this);
    ui->pbShow->setEnabled(bShow);
    if(bStore)
        setWindowTitle(tr("Store password"));
    else
        setWindowTitle(tr("Restore password"));
    QString szDescript = tr("The encryption key is used to encrypt the password that is saved to the file.");
    ui->lbDescript->setText(szDescript);
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

QString CDlgInputPassword::GetPassword()
{
    return ui->lePassword->text();
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
    default:
        break;
    }
}
