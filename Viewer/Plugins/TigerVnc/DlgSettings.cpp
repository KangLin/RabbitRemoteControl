//! @author: Kang Lin(kl222@126.com)

#include "DlgSettings.h"
#include "ui_DlgSettings.h"
#include "rfb/encodings.h"
#include <QDebug>

CDlgSettings::CDlgSettings(CConnectTigerVnc::strPara *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettings),
    m_pPara(pPara)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    
    // Server
    ui->leServer->setText(m_pPara->szServerName);
    ui->leName->setText(m_pPara->szUser);
    ui->lePassword->setText(m_pPara->szPassword);
    ui->cbSave->setChecked(m_pPara->bSave);
    
    ui->cbShared->setChecked(m_pPara->bShared);
    ui->cbRealTimeUpdate->setChecked(!m_pPara->bBufferEndRefresh);
    ui->cbLocalCursor->setChecked(m_pPara->bLocalCursor);
    ui->cbResizeWindows->setChecked(m_pPara->bSupportsDesktopResize);
    ui->cbClipboard->setChecked(m_pPara->bClipboard);
    
    // Compress
    ui->cbCompressAutoSelect->setChecked(m_pPara->bAutoSelect);

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
    case CConnectTigerVnc::Full:
        ui->rbFull->setChecked(true);
        break;
    case CConnectTigerVnc::Medium:
        ui->rbMeduim->setChecked(true);
        break;
    case CConnectTigerVnc::Low:
        ui->rbLow->setChecked(true);
        break;
    case CConnectTigerVnc::VeryLow:
        ui->rbVeryLow->setChecked(true);
        break;
    }
    ui->cbCompress->setChecked(m_pPara->bCompressLevel);
    ui->spCompressLevel->setValue(m_pPara->nCompressLevel);
    ui->cbJPEG->setChecked(!m_pPara->bNoJpeg);
    ui->spJPEGLevel->setValue(m_pPara->nQualityLevel);
}

CDlgSettings::~CDlgSettings()
{
    qDebug() << "CDlgSettings::~CDlgSettings()";
    delete ui;
}

void CDlgSettings::on_pushButton_clicked()
{
    if(!m_pPara)
        reject();
    
    // Server
    m_pPara->szServerName = ui->leServer->text();
    m_pPara->szUser = ui->leName->text();
    m_pPara->szPassword = ui->lePassword->text();
    
    m_pPara->bSave = ui->cbSave->isChecked();
    m_pPara->bShared = ui->cbShared->isChecked();
    m_pPara->bBufferEndRefresh = !ui->cbRealTimeUpdate->isChecked();
    m_pPara->bLocalCursor = ui->cbLocalCursor->isChecked();
    m_pPara->bSupportsDesktopResize = ui->cbResizeWindows->isChecked();
    m_pPara->bClipboard = ui->cbClipboard->isChecked();
    
    // Compress
    m_pPara->bAutoSelect = ui->cbCompressAutoSelect->isChecked();
    
    if(ui->rbTight->isChecked()) m_pPara->nEncoding = rfb::encodingTight;
    if(ui->rbRaw->isChecked()) m_pPara->nEncoding = rfb::encodingRaw;
    if(ui->rbRRE->isChecked()) m_pPara->nEncoding = rfb::encodingRRE;
    if(ui->rbZRLE->isChecked()) m_pPara->nEncoding = rfb::encodingZRLE;
    if(ui->rbCoRRE->isChecked()) m_pPara->nEncoding = rfb::encodingCoRRE;
    if(ui->rbCopyRect->isChecked()) m_pPara->nEncoding = rfb::encodingCopyRect;
    if(ui->rbHextile->isChecked()) m_pPara->nEncoding = rfb::encodingHextile;
    
    if(ui->rbFull->isChecked()) m_pPara->nColorLevel = CConnectTigerVnc::Full;
    if(ui->rbMeduim->isChecked()) m_pPara->nColorLevel = CConnectTigerVnc::Medium;
    if(ui->rbLow->isChecked()) m_pPara->nColorLevel = CConnectTigerVnc::Low;
    if(ui->rbVeryLow->isChecked()) m_pPara->nColorLevel = CConnectTigerVnc::VeryLow;

    m_pPara->bCompressLevel = ui->cbCompress->isChecked();
    m_pPara->nCompressLevel = ui->spCompressLevel->value();
    m_pPara->bNoJpeg = !ui->cbJPEG->isChecked();
    m_pPara->nQualityLevel = ui->spJPEGLevel->value();
    
    accept();
}

void CDlgSettings::on_pushButton_2_clicked()
{
    reject();
}

void CDlgSettings::on_cbCompressAutoSelect_stateChanged(int arg1)
{
    m_pPara->bAutoSelect = arg1;
    if(m_pPara->bAutoSelect)
    {
        ui->gpEncodeing->setEnabled(false);
        ui->gpColorLevel->setEnabled(false);
        ui->cbJPEG->setEnabled(false);
        ui->spJPEGLevel->setEnabled(false);
        ui->cbCompress->setEnabled(false);
        ui->spCompressLevel->setEnabled(false);
    } else {
        ui->gpEncodeing->setEnabled(true);
        ui->gpColorLevel->setEnabled(true);
        ui->cbJPEG->setEnabled(true);
        if(ui->cbJPEG->isChecked())
            ui->spJPEGLevel->setEnabled(true);
        ui->cbCompress->setEnabled(true);
        if(ui->cbCompress->isChecked())
            ui->spCompressLevel->setEnabled(true);
    }
}

void CDlgSettings::on_cbCompress_stateChanged(int arg1)
{
    m_pPara->bCompressLevel = arg1;
    if(m_pPara->bCompressLevel)
    {
        ui->spCompressLevel->setEnabled(true);
    } else {
        ui->spCompressLevel->setEnabled(false);
    }
}

void CDlgSettings::on_cbJPEG_stateChanged(int arg1)
{
    m_pPara->bNoJpeg = !arg1;
    if(m_pPara->bNoJpeg)
    {
        ui->spJPEGLevel->setEnabled(false);
    } else {
        ui->spJPEGLevel->setEnabled(true);
    }
}
