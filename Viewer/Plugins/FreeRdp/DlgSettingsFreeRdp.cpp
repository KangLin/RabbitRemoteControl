//! @author: Kang Lin(kl222@126.com)

#include "DlgSettingsFreeRdp.h"
#include "ui_DlgSettingsFreeRdp.h"

CDlgSettingsFreeRdp::CDlgSettingsFreeRdp(rdpSettings *pSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsFreeRdp),
    m_pSettings(pSettings)
{
    ui->setupUi(this);
    
}

CDlgSettingsFreeRdp::~CDlgSettingsFreeRdp()
{
    delete ui;
}

void CDlgSettingsFreeRdp::on_pbOk_clicked()
{

    this->accept();
}

void CDlgSettingsFreeRdp::on_pbCancle_clicked()
{
    this->reject();
}
