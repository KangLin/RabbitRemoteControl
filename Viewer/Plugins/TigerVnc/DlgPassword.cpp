#include "DlgPassword.h"
#include "ui_DlgPassword.h"

CDlgPassword::CDlgPassword(CConnect *pConnect, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgPassword),
    m_pConnect(pConnect)
{
    ui->leName->setEnabled(false);
    ui->setupUi(this);
}

CDlgPassword::~CDlgPassword()
{
    delete ui;
}

void CDlgPassword::on_pbOk_clicked()
{
    m_pConnect->SetUser(ui->leName->text(), ui->lePassword->text());
    this->accept();
}

void CDlgPassword::on_pbCancle_clicked()
{
    this->reject();
}
