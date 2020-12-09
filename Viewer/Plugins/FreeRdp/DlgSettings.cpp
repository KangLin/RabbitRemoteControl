//! @author: Kang Lin(kl222@126.com)

#include "DlgSettings.h"
#include "ui_DlgSettings.h"

CDlgSettings::CDlgSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettings)
{
    ui->setupUi(this);
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::on_pbOk_clicked()
{
    this->accept();
}

void CDlgSettings::on_pbCancle_clicked()
{
    this->reject();
}
