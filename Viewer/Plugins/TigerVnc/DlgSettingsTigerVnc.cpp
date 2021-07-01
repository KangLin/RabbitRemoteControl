// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsTigerVnc.h"
#include "ui_DlgSettingsTigerVnc.h"
#include "rfb/encodings.h"
#include <QDebug>

CDlgSettingsTigerVnc::CDlgSettingsTigerVnc(CConnectTigerVnc::strPara *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTigerVnc),
    m_pPara(pPara)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
}

CDlgSettingsTigerVnc::~CDlgSettingsTigerVnc()
{
    qDebug() << "CDlgSettingsTigerVnc::~CDlgSettingsTigerVnc()";
    delete ui;
}

void CDlgSettingsTigerVnc::on_pushButton_clicked()
{
    if(!m_pPara)
        reject();
    
    // Server
    m_pPara->szName = ui->leName->text();
    m_pPara->szHost = ui->leServer->text();
    m_pPara->nPort = ui->spPort->value();
    m_pPara->szUser = ui->leUserName->text();
    m_pPara->szPassword = ui->lePassword->text();
    
    m_pPara->bOnlyView = ui->cbOnlyView->isChecked();
    m_pPara->bSavePassword = ui->cbSave->isChecked();
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
    
    // Proxy
    if(ui->rbProxyNo->isChecked())
        m_pPara->eProxyType = CParameter::emProxy::No;
    if(ui->rbProxySocks->isChecked())
        m_pPara->eProxyType = CParameter::emProxy::SocksV5;
    if(ui->rbHttp->isChecked())
        m_pPara->eProxyType = CParameter::emProxy::Http;
    m_pPara->szProxyHost = ui->leProxyServer->text();
    m_pPara->nProxyPort = ui->spProxyPort->value();
    m_pPara->szProxyUser = ui->leProxyUser->text();
    m_pPara->szProxyPassword = ui->leProxyPassword->text();
    
    accept();
}

void CDlgSettingsTigerVnc::on_pushButton_2_clicked()
{
    reject();
}

void CDlgSettingsTigerVnc::on_cbCompressAutoSelect_stateChanged(int arg1)
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

void CDlgSettingsTigerVnc::on_cbCompress_stateChanged(int arg1)
{
    m_pPara->bCompressLevel = arg1;
    
    ui->spCompressLevel->setEnabled(m_pPara->bCompressLevel);
}

void CDlgSettingsTigerVnc::on_cbJPEG_stateChanged(int arg1)
{
    m_pPara->bNoJpeg = !arg1;
    
    ui->spJPEGLevel->setEnabled(!m_pPara->bNoJpeg);
}


void CDlgSettingsTigerVnc::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    
    // Server
    ui->leName->setText(m_pPara->szName);
    ui->leServer->setText(m_pPara->szHost);
    ui->spPort->setValue(m_pPara->nPort);
    ui->leUserName->setText(m_pPara->szUser);
    ui->lePassword->setText(m_pPara->szPassword);
    ui->cbSave->setChecked(m_pPara->bSavePassword);
    ui->cbOnlyView->setChecked(m_pPara->bOnlyView);
    
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
    
    if(m_pPara->bAutoSelect)
    {
        ui->spCompressLevel->setEnabled(false);
        ui->spJPEGLevel->setEnabled(false);
    } else {    
        ui->spCompressLevel->setEnabled(m_pPara->bCompressLevel);
        ui->spJPEGLevel->setEnabled(!m_pPara->bNoJpeg);
    }
    
    // Proxy
    switch(m_pPara->eProxyType)
    {
    case CParameter::emProxy::No:
        ui->rbProxyNo->setChecked(true);
        break;
    case CParameter::emProxy::SocksV4:
    case CParameter::emProxy::SocksV5:
        ui->rbProxySocks->setChecked(true);
        break;
    case CParameter::emProxy::Http:
        ui->rbHttp->setChecked(true);
        break;
    default:
        break;
    }
    ui->leProxyServer->setText(m_pPara->szProxyHost);
    ui->spProxyPort->setValue(m_pPara->nProxyPort);
    ui->leProxyUser->setText(m_pPara->szProxyUser);
    ui->leProxyPassword->setText(m_pPara->szProxyPassword);
}

void CDlgSettingsTigerVnc::on_pbShow_clicked()
{
    switch(ui->lePassword->echoMode())
    {
    case QLineEdit::Password:
        ui->lePassword->setEchoMode(QLineEdit::Normal);
        ui->pbShow->setIcon(QIcon(":/image/EyeOff"));
        break;
    case QLineEdit::Normal:
        ui->lePassword->setEchoMode(QLineEdit::Password);
        ui->pbShow->setIcon(QIcon(":/image/EyeOn"));
        break;
    }
}
