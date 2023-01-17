// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsLibVnc.h"
#include "ui_DlgSettingsLibVnc.h"
#include <QDebug>

CDlgSettingsLibVnc::CDlgSettingsLibVnc(CConnecterLibVNCServer *pConnecter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsLibVnc),
    m_pPara(&pConnecter->m_Para)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    
    // Server
    ui->leName->setText(m_pPara->GetName());
    ui->leHost->setText(m_pPara->GetHost());
    ui->spPort->setValue(m_pPara->GetPort());
    ui->leUserName->setText(m_pPara->GetUser());
    ui->lePassword->setText(m_pPara->GetPassword());
    ui->pbShow->setEnabled(m_pPara->GetParameterClient()->GetViewPassowrd());
    ui->cbSavePassword->setChecked(m_pPara->GetSavePassword());
    ui->cbShowServerName->setChecked(m_pPara->GetShowServerName());
    
    ui->cbShared->setChecked(m_pPara->GetShared());
    ui->cbOnlyView->setChecked(m_pPara->GetOnlyView());
//    ui->cbRealTimeUpdate->setChecked(!m_pPara->GetBufferEndRefresh());
    ui->cbLocalCursor->setChecked(m_pPara->GetLocalCursor());
//    ui->cbResizeWindows->setChecked(m_pPara->GetSupportsDesktopResize());
    ui->cbClipboard->setChecked(m_pPara->GetClipboard());
    
    // Compress
//    ui->cbCompressAutoSelect->setChecked(m_pPara->bAutoSelect);

//    switch(m_pPara->nEncoding)
//    {
//    case rfb::encodingTight:
//        ui->rbTight->setChecked(true);
//        break;
//    case rfb::encodingRaw:
//        ui->rbRaw->setChecked(true);
//        break;
//    case rfb::encodingRRE:
//        ui->rbRRE->setChecked(true);
//        break;
//    case rfb::encodingZRLE:
//        ui->rbZRLE->setChecked(true);
//        break;
//    case rfb::encodingCoRRE:
//        ui->rbCoRRE->setChecked(true);
//        break;
//    case rfb::encodingCopyRect:
//        ui->rbCopyRect->setChecked(true);
//        break;
//    case rfb::encodingHextile:
//        ui->rbHextile->setChecked(true);
//        break;
//    }

    ui->cbCompress->setChecked(m_pPara->GetEnableCompressLevel());
    ui->spCompressLevel->setEnabled(m_pPara->GetEnableCompressLevel());
    ui->spCompressLevel->setValue(m_pPara->GetCompressLevel());
    ui->spJPEGLevel->setEnabled(m_pPara->GetJpeg());
    ui->cbJPEG->setChecked(m_pPara->GetJpeg());
    ui->spJPEGLevel->setValue(m_pPara->GetQualityLevel());
    
    // Proxy
    switch(m_pPara->GetProxyType())
    {
    case CParameterConnecter::emProxy::No:
        ui->rbProxyNo->setChecked(true);
        break;
    case CParameterConnecter::emProxy::SocksV4:
    case CParameterConnecter::emProxy::SocksV5:
        ui->rbProxySocks->setChecked(true);
        break;
    case (CParameterConnecter::emProxy) CParameterLibVNCServer::emVncProxy::UltraVncRepeater:
        ui->rbProxyUltraVncRepeater->setChecked(true);
        break;
    default:
        break;
    }
    ui->leProxyServer->setText(m_pPara->GetProxyHost());
    ui->spProxyPort->setValue(m_pPara->GetProxyPort());
    ui->leProxyUser->setText(m_pPara->GetProxyUser());
    ui->leProxyPassword->setText(m_pPara->GetProxyPassword());
}

CDlgSettingsLibVnc::~CDlgSettingsLibVnc()
{
    qDebug() << "CDlgSettingsLibVnc::~CDlgSettingsLibVnc()";
    delete ui;
}

void CDlgSettingsLibVnc::on_pushButton_clicked()
{
    if(!m_pPara)
        reject();
    
    // Server
    m_pPara->SetName(ui->leName->text());
    m_pPara->SetHost(ui->leHost->text());
    m_pPara->SetPort(ui->spPort->value());
    m_pPara->SetUser(ui->leUserName->text());
    m_pPara->SetPassword(ui->lePassword->text());
    m_pPara->SetSavePassword(ui->cbSavePassword->isChecked());
    m_pPara->SetShowServerName(ui->cbShowServerName->isChecked());
    
//    m_pPara->bSave = ui->cbSave->isChecked();
    m_pPara->SetShared(ui->cbShared->isChecked());
    m_pPara->SetOnlyView(ui->cbOnlyView->isChecked());
//    m_pPara->bBufferEndRefresh = !ui->cbRealTimeUpdate->isChecked();
    m_pPara->SetLocalCursor(ui->cbLocalCursor->isChecked());
//    m_pPara->bSupportsDesktopResize = ui->cbResizeWindows->isChecked();
    m_pPara->SetClipboard(ui->cbClipboard->isChecked());
    
//    // Compress
//    m_pPara->bAutoSelect = ui->cbCompressAutoSelect->isChecked();
    
//    if(ui->rbTight->isChecked()) m_pPara->nEncoding = rfb::encodingTight;
//    if(ui->rbRaw->isChecked()) m_pPara->nEncoding = rfb::encodingRaw;
//    if(ui->rbRRE->isChecked()) m_pPara->nEncoding = rfb::encodingRRE;
//    if(ui->rbZRLE->isChecked()) m_pPara->nEncoding = rfb::encodingZRLE;
//    if(ui->rbCoRRE->isChecked()) m_pPara->nEncoding = rfb::encodingCoRRE;
//    if(ui->rbCopyRect->isChecked()) m_pPara->nEncoding = rfb::encodingCopyRect;
//    if(ui->rbHextile->isChecked()) m_pPara->nEncoding = rfb::encodingHextile;
    

    m_pPara->SetEnableCompressLevel(ui->cbCompress->isChecked());
    m_pPara->SetCompressLevel(ui->spCompressLevel->value());
    m_pPara->SetJpeg(ui->cbJPEG->isChecked());
    m_pPara->SetQualityLevel(ui->spJPEGLevel->value());
    
    // Proxy
    if(ui->rbProxyNo->isChecked())
        m_pPara->SetProxyType(CParameterConnecter::emProxy::No);
    if(ui->rbProxySocks->isChecked())
        m_pPara->SetProxyType(CParameterConnecter::emProxy::SocksV5);
    if(ui->rbProxyUltraVncRepeater->isChecked())
        m_pPara->SetProxyType( (CParameterConnecter::emProxy)CParameterLibVNCServer::emVncProxy::UltraVncRepeater);
    m_pPara->SetProxyHost(ui->leProxyServer->text());
    m_pPara->SetProxyPort(ui->spProxyPort->value());
    m_pPara->SetProxyUser(ui->leProxyUser->text());
    m_pPara->SetProxyPassword(ui->leProxyPassword->text());
    
    accept();
}

void CDlgSettingsLibVnc::on_pushButton_2_clicked()
{
    reject();
}

void CDlgSettingsLibVnc::on_cbCompress_stateChanged(int arg1)
{
    m_pPara->SetEnableCompressLevel(arg1);
    ui->spCompressLevel->setEnabled(arg1);
}

void CDlgSettingsLibVnc::on_cbJPEG_stateChanged(int arg1)
{
    m_pPara->SetJpeg(arg1);
    ui->spJPEGLevel->setEnabled(m_pPara->GetJpeg());
}

void CDlgSettingsLibVnc::on_pbShow_clicked()
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

void CDlgSettingsLibVnc::on_leHost_editingFinished()
{
    auto s = ui->leHost->text().split(":");
    if(s.size() == 2)
    {
        ui->spPort->setValue(s[1].toUInt());
        ui->leHost->setText(s[0]);
    }
}
