// Author: Kang Lin <kl222@126.com>

#include "DlgInputPassword.h"
#include "ui_DlgInputPassword.h"

CDlgInputPassword::CDlgInputPassword(QString szTitle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgInputPassword)
{
    ui->setupUi(this);
    setWindowTitle(tr("Input password"));
    ui->lbTitle->setText(tr("Input %1:").arg(szTitle));
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
