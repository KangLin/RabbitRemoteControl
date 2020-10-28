#include "DlgSettings.h"
#include "ui_DlgSettings.h"
#include "rfb/encodings.h"

CDlgSettings::CDlgSettings(strPara *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettings),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    ui->leServer->setText(m_pPara->szServerName);
    ui->leName->setText(m_pPara->szUser);
    ui->lePassword->setText(m_pPara->szPassword);
    
    switch(m_pPara->nEncoding)
    {
    case rfb::encodingTight:
        ui->rbTight->setChecked(true);
        break;
    case rfb::encodingRaw:
        ui->rbRaw->setChecked(true);
        break;
    case rfb::encodingRRE:
        ui->rbRRE->setChecked(true);
        break;
    case rfb::encodingZRLE:
        ui->rbZRLE->setChecked(true);
        break;
    case rfb::encodingCoRRE:
        ui->rbCoRRE->setChecked(true);
        break;
    case rfb::encodingCopyRect:
        ui->rbCopyRect->setChecked(true);
        break;
    case rfb::encodingHextile:
        ui->rbHextile->setChecked(true);
        break;
    }
    
    switch(m_pPara->nColorLevel)
    {
    case 0:
        ui->rbFull->setChecked(true);
        break;
    case 1:
        ui->rbMeduim->setChecked(true);
        break;
    case 2:
        ui->rbLow->setChecked(true);
        break;
    case 3:
        ui->rbVeryLow->setChecked(true);
        break;
    }
    
    ui->spCompressLevel->setValue(m_pPara->nCompressLevel);
    ui->spJPEGLevel->setValue(m_pPara->nQualityLevel);
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::on_pushButton_clicked()
{
    if(!m_pPara)
        reject();
    
    m_pPara->szServerName = ui->leServer->text();
    m_pPara->szUser = ui->leName->text();
    m_pPara->szPassword = ui->lePassword->text();
    
    if(ui->rbTight->isChecked()) m_pPara->nEncoding = rfb::encodingTight;
    if(ui->rbRaw->isChecked()) m_pPara->nEncoding = rfb::encodingRaw;
    if(ui->rbRRE->isChecked()) m_pPara->nEncoding = rfb::encodingRRE;
    if(ui->rbZRLE->isChecked()) m_pPara->nEncoding = rfb::encodingZRLE;
    if(ui->rbCoRRE->isChecked()) m_pPara->nEncoding = rfb::encodingCoRRE;
    if(ui->rbCopyRect->isChecked()) m_pPara->nEncoding = rfb::encodingCopyRect;
    if(ui->rbHextile->isChecked()) m_pPara->nEncoding = rfb::encodingHextile;
    
    if(ui->rbFull->isChecked()) m_pPara->nColorLevel = 0;
    if(ui->rbMeduim->isChecked()) m_pPara->nColorLevel = 1;
    if(ui->rbLow->isChecked()) m_pPara->nColorLevel = 2;
    if(ui->rbVeryLow->isChecked()) m_pPara->nColorLevel = 3;

    m_pPara->nCompressLevel = ui->spCompressLevel->value();
    m_pPara->nQualityLevel = ui->spJPEGLevel->value();
    
    accept();
}

void CDlgSettings::on_pushButton_2_clicked()
{
    reject();
}
