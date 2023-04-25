// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsRabbitVNC.h"
#include "ui_DlgSettingsRabbitVNC.h"
#include "rfb/encodings.h"
#include <QDebug>
#include <QMessageBox>

#ifdef HAVE_ICE
    #include "Ice.h"
#endif

#undef SetPort

CDlgSettingsRabbitVNC::CDlgSettingsRabbitVNC(CParameterRabbitVNC *pPara, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::CDlgSettingsRabbitVNC),
    m_pPara(pPara)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    
#ifdef HAVE_ICE
    ui->gpIce->show();
#else
    ui->gpIce->hide();
#endif
}

CDlgSettingsRabbitVNC::~CDlgSettingsRabbitVNC()
{
    qDebug() << "CDlgSettingsRabbitVnc::~CDlgSettingsRabbitVnc()";
    delete ui;
}

void CDlgSettingsRabbitVNC::on_pbOk_clicked()
{
    if(!m_pPara)
        reject();
    
    // Server
    bool ice = false;
#ifdef HAVE_ICE
    ice = CICE::Instance()->GetParameter()->getIce();
#endif
    if(ui->gpIce->isChecked() && ice)
    {
        m_pPara->SetIce(true);
        m_pPara->SetPeerUser(ui->lePeerUser->text());
    } else {
        m_pPara->SetIce(false);
    }

    m_pPara->SetHost(ui->leServer->text());
    m_pPara->SetPort(ui->spPort->value());
    m_pPara->SetName(ui->leName->text());
    m_pPara->SetUser(ui->leUserName->text());
    m_pPara->SetPassword(ui->lePassword->text());
    
    m_pPara->SetOnlyView(ui->cbOnlyView->isChecked());
    m_pPara->SetSavePassword(ui->cbSave->isChecked());
    m_pPara->SetShared(ui->cbShared->isChecked());
    m_pPara->SetBufferEndRefresh(!ui->cbRealTimeUpdate->isChecked());
    m_pPara->SetLocalCursor(ui->cbLocalCursor->isChecked());
    m_pPara->SetSupportsDesktopResize(ui->cbResizeWindows->isChecked());
    m_pPara->SetClipboard(ui->cbClipboard->isChecked());
    m_pPara->SetShowServerName(ui->cbShowServerName->isChecked());
    
    // Compress
    m_pPara->SetAutoSelect(ui->cbCompressAutoSelect->isChecked());
    
    if(ui->rbTight->isChecked()) m_pPara->SetEncoding(rfb::encodingTight);
    if(ui->rbRaw->isChecked()) m_pPara->SetEncoding(rfb::encodingRaw);
    if(ui->rbRRE->isChecked()) m_pPara->SetEncoding(rfb::encodingRRE);
    if(ui->rbZRLE->isChecked()) m_pPara->SetEncoding(rfb::encodingZRLE);
    if(ui->rbCoRRE->isChecked()) m_pPara->SetEncoding(rfb::encodingCoRRE);
    if(ui->rbCopyRect->isChecked()) m_pPara->SetEncoding(rfb::encodingCopyRect);
    if(ui->rbHextile->isChecked()) m_pPara->SetEncoding(rfb::encodingHextile);
    
    if(ui->rbFull->isChecked()) m_pPara->SetColorLevel(CParameterRabbitVNC::Full);
    if(ui->rbMeduim->isChecked()) m_pPara->SetColorLevel(CParameterRabbitVNC::Medium);
    if(ui->rbLow->isChecked()) m_pPara->SetColorLevel(CParameterRabbitVNC::Low);
    if(ui->rbVeryLow->isChecked()) m_pPara->SetColorLevel(CParameterRabbitVNC::VeryLow);

    m_pPara->SetEnableCompressLevel(ui->cbCompress->isChecked());
    m_pPara->SetCompressLevel(ui->spCompressLevel->value());
    m_pPara->SetNoJpeg(!ui->cbJPEG->isChecked());
    m_pPara->SetQualityLevel(ui->spJPEGLevel->value());
    
    // Proxy
    if(ui->rbProxyNo->isChecked())
        m_pPara->SetProxyType(CParameterConnecter::emProxy::No);
    if(ui->rbProxySocks->isChecked())
        m_pPara->SetProxyType(CParameterConnecter::emProxy::SocksV5);
    if(ui->rbHttp->isChecked())
        m_pPara->SetProxyType(CParameterConnecter::emProxy::Http);
    m_pPara->SetProxyHost(ui->leProxyServer->text());
    m_pPara->SetProxyPort(ui->spProxyPort->value());
    m_pPara->SetProxyUser(ui->leProxyUser->text());
    m_pPara->SetProxyPassword(ui->leProxyPassword->text());
    
    accept();
}

void CDlgSettingsRabbitVNC::on_pbCancel_clicked()
{
    reject();
}

void CDlgSettingsRabbitVNC::on_cbCompressAutoSelect_stateChanged(int arg1)
{
    m_pPara->SetAutoSelect(arg1);
    if(m_pPara->GetAutoSelect())
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

void CDlgSettingsRabbitVNC::on_cbCompress_stateChanged(int arg1)
{
    m_pPara->SetCompressLevel(arg1);
    
    ui->spCompressLevel->setEnabled(m_pPara->GetEnableCompressLevel());
}

void CDlgSettingsRabbitVNC::on_cbJPEG_stateChanged(int arg1)
{
    m_pPara->SetNoJpeg(!arg1);
    
    ui->spJPEGLevel->setEnabled(!m_pPara->GetNoJpeg());
}


void CDlgSettingsRabbitVNC::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    // Server
    bool ice = false;
#ifdef HAVE_ICE
    ice = CICE::Instance()->GetParameter()->getIce();
#endif
    if(ice)
    {
        if(m_pPara->GetIce())
        {
            ui->gpIce->setChecked(true);
            ui->leServer->setEnabled(false);
            ui->spPort->setEnabled(false);
        } else {
            ui->gpIce->setChecked(false);
        }
    } else {
        ui->gpIce->setEnabled(false);
    }

    ui->leServer->setText(m_pPara->GetHost());
    ui->spPort->setValue(m_pPara->GetPort());
    ui->lePeerUser->setText(m_pPara->GetPeerUser());
    
    ui->leName->setText(m_pPara->GetName());
    ui->leUserName->setText(m_pPara->GetUser());
    ui->lePassword->setText(m_pPara->GetPassword());
    ui->pbShow->setEnabled(m_pPara->GetParameterClient()->GetViewPassowrd());
    ui->cbSave->setChecked(m_pPara->GetSavePassword());
    ui->cbOnlyView->setChecked(m_pPara->GetOnlyView());
    ui->cbShowServerName->setChecked(m_pPara->GetShowServerName());

    ui->cbShared->setChecked(m_pPara->GetShared());
    ui->cbRealTimeUpdate->setChecked(!m_pPara->GetBufferEndRefresh());
    ui->cbLocalCursor->setChecked(m_pPara->GetLocalCursor());
    ui->cbResizeWindows->setChecked(m_pPara->GetSupportsDesktopResize());
    ui->cbClipboard->setChecked(m_pPara->GetClipboard());
    
    // Compress
    ui->cbCompressAutoSelect->setChecked(m_pPara->GetAutoSelect());
        
    switch(m_pPara->GetEncoding())
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
    
    switch(m_pPara->GetColorLevel())
    {
    case CParameterRabbitVNC::Full:
        ui->rbFull->setChecked(true);
        break;
    case CParameterRabbitVNC::Medium:
        ui->rbMeduim->setChecked(true);
        break;
    case CParameterRabbitVNC::Low:
        ui->rbLow->setChecked(true);
        break;
    case CParameterRabbitVNC::VeryLow:
        ui->rbVeryLow->setChecked(true);
        break;
    }
    
    ui->cbCompress->setChecked(m_pPara->GetEnableCompressLevel());
    ui->spCompressLevel->setValue(m_pPara->GetCompressLevel());
    ui->cbJPEG->setChecked(!m_pPara->GetNoJpeg());
    ui->spJPEGLevel->setValue(m_pPara->GetQualityLevel());
    
    if(m_pPara->GetAutoSelect())
    {
        ui->spCompressLevel->setEnabled(false);
        ui->spJPEGLevel->setEnabled(false);
    } else {    
        ui->spCompressLevel->setEnabled(m_pPara->GetEnableCompressLevel());
        ui->spJPEGLevel->setEnabled(!m_pPara->GetNoJpeg());
    }
    
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
    case CParameterConnecter::emProxy::Http:
        ui->rbHttp->setChecked(true);
        break;
    default:
        break;
    }
    ui->leProxyServer->setText(m_pPara->GetProxyHost());
    ui->spProxyPort->setValue(m_pPara->GetProxyPort());
    ui->leProxyUser->setText(m_pPara->GetProxyUser());
    ui->leProxyPassword->setText(m_pPara->GetProxyPassword());
}

void CDlgSettingsRabbitVNC::on_pbShow_clicked()
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

void CDlgSettingsRabbitVNC::on_gpIce_clicked(bool checked)
{
    ui->leServer->setEnabled(!checked);
    ui->spPort->setEnabled(!checked);
}

void CDlgSettingsRabbitVNC::on_leServer_editingFinished()
{
    auto s = ui->leServer->text().split(":");
    if(s.size() == 2)
    {
        ui->spPort->setValue(s[1].toUInt());
        ui->leServer->setText(s[0]);
    }
}
