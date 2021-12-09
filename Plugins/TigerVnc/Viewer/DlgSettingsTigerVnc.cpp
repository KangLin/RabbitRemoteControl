// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsTigerVnc.h"
#include "ui_DlgSettingsTigerVnc.h"
#include "rfb/encodings.h"
#include <QDebug>
#include <QMessageBox>

#ifdef HAVE_QXMPP
    #include "QXmppUtils.h"
#endif

#undef SetPort

CDlgSettingsTigerVnc::CDlgSettingsTigerVnc(CParameterTigerVnc *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTigerVnc),
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

CDlgSettingsTigerVnc::~CDlgSettingsTigerVnc()
{
    qDebug() << "CDlgSettingsTigerVnc::~CDlgSettingsTigerVnc()";
    delete ui;
}

#ifdef HAVE_QXMPP
QString jidToDomain(const QString &jid)
{
    auto v = QXmppUtils::jidToBareJid(jid).split("@");
    if(v.size() < 2)
        return QString();
    return v.last();
}
#endif

void CDlgSettingsTigerVnc::on_pushButton_clicked()
{
    if(!m_pPara)
        reject();
    
    // Server
    if(ui->gpIce->isChecked())
    {
        m_pPara->SetIce(true);
        m_pPara->SetSignalServer(ui->leIceSignalServer->text());
        m_pPara->SetSignalPort(ui->spSignalPort->value());
        m_pPara->SetSignalUser(ui->leSignalUser->text());
        m_pPara->SetSignalPassword(ui->leSignalPassword->text());
        m_pPara->SetPeerUser(ui->lePeerUser->text());
#ifdef HAVE_QXMPP
        if(ui->leDomain->text().isEmpty()
                && jidToDomain(m_pPara->GetSignalUser()).isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Please set domain"));
            return;
        }
        if(jidToDomain(m_pPara->GetSignalUser()).isEmpty()
                && !ui->leDomain->text().isEmpty())
        {
            m_pPara->SetSignalUser(m_pPara->GetSignalUser()
                                   + "@" + ui->leDomain->text());
        }
        if(QXmppUtils::jidToResource(m_pPara->GetSignalUser()).isEmpty())
        {
            m_pPara->SetSignalUser(m_pPara->GetSignalUser() + "/"
                             + QXmppUtils::jidToUser(m_pPara->GetSignalUser()));
        }
        if(jidToDomain(m_pPara->GetPeerUser()).isEmpty()
                && !ui->leDomain->text().isEmpty())
        {
            m_pPara->SetPeerUser(m_pPara->GetPeerUser()
                                 + "@" + ui->leDomain->text());
        }
        if(QXmppUtils::jidToResource(m_pPara->GetPeerUser()).isEmpty())
        {
            m_pPara->SetPeerUser(m_pPara->GetPeerUser()
                         + "/" + QXmppUtils::jidToUser(m_pPara->GetPeerUser()));
        }
#endif
        m_pPara->SetStunServer(ui->leStunServer->text());
        m_pPara->SetStunPort(ui->spStunPort->value());
        m_pPara->SetTurnServer(ui->leTurnServer->text());
        m_pPara->SetTurnPort(ui->spTurnPort->value());
        m_pPara->SetTurnUser(ui->leTurnUser->text());
        m_pPara->SetTurnPassword(ui->leTurnPassword->text());
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
    
    // Compress
    m_pPara->SetAutoSelect(ui->cbCompressAutoSelect->isChecked());
    
    if(ui->rbTight->isChecked()) m_pPara->SetEncoding(rfb::encodingTight);
    if(ui->rbRaw->isChecked()) m_pPara->SetEncoding(rfb::encodingRaw);
    if(ui->rbRRE->isChecked()) m_pPara->SetEncoding(rfb::encodingRRE);
    if(ui->rbZRLE->isChecked()) m_pPara->SetEncoding(rfb::encodingZRLE);
    if(ui->rbCoRRE->isChecked()) m_pPara->SetEncoding(rfb::encodingCoRRE);
    if(ui->rbCopyRect->isChecked()) m_pPara->SetEncoding(rfb::encodingCopyRect);
    if(ui->rbHextile->isChecked()) m_pPara->SetEncoding(rfb::encodingHextile);
    
    if(ui->rbFull->isChecked()) m_pPara->SetColorLevel(CParameterTigerVnc::Full);
    if(ui->rbMeduim->isChecked()) m_pPara->SetColorLevel(CParameterTigerVnc::Medium);
    if(ui->rbLow->isChecked()) m_pPara->SetColorLevel(CParameterTigerVnc::Low);
    if(ui->rbVeryLow->isChecked()) m_pPara->SetColorLevel(CParameterTigerVnc::VeryLow);

    m_pPara->SetEnableCompressLevel(ui->cbCompress->isChecked());
    m_pPara->SetCompressLevel(ui->spCompressLevel->value());
    m_pPara->SetNoJpeg(!ui->cbJPEG->isChecked());
    m_pPara->SetQualityLevel(ui->spJPEGLevel->value());
    
    // Proxy
    if(ui->rbProxyNo->isChecked())
        m_pPara->SetProxyType(CParameter::emProxy::No);
    if(ui->rbProxySocks->isChecked())
        m_pPara->SetProxyType(CParameter::emProxy::SocksV5);
    if(ui->rbHttp->isChecked())
        m_pPara->SetProxyType(CParameter::emProxy::Http);
    m_pPara->SetProxyHost(ui->leProxyServer->text());
    m_pPara->SetProxyPort(ui->spProxyPort->value());
    m_pPara->SetProxyUser(ui->leProxyUser->text());
    m_pPara->SetProxyPassword(ui->leProxyPassword->text());
    
    accept();
}

void CDlgSettingsTigerVnc::on_pushButton_2_clicked()
{
    reject();
}

void CDlgSettingsTigerVnc::on_cbCompressAutoSelect_stateChanged(int arg1)
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

void CDlgSettingsTigerVnc::on_cbCompress_stateChanged(int arg1)
{
    m_pPara->SetCompressLevel(arg1);
    
    ui->spCompressLevel->setEnabled(m_pPara->GetEnableCompressLevel());
}

void CDlgSettingsTigerVnc::on_cbJPEG_stateChanged(int arg1)
{
    m_pPara->SetNoJpeg(!arg1);
    
    ui->spJPEGLevel->setEnabled(!m_pPara->GetNoJpeg());
}


void CDlgSettingsTigerVnc::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    
    // Server
    if(m_pPara->GetIce())
    {
        ui->gpIce->setChecked(true);
        ui->leServer->setEnabled(false);
        ui->spPort->setEnabled(false);
    } else {
        ui->gpIce->setChecked(false);
    }
    ui->leServer->setText(m_pPara->GetHost());
    ui->spPort->setValue(m_pPara->GetPort());
    ui->leIceSignalServer->setText(m_pPara->GetSignalServer());
    ui->spSignalPort->setValue(m_pPara->GetSignalPort());
    ui->leSignalUser->setText(m_pPara->GetSignalUser());
#ifdef HAVE_QXMPP
    ui->leDomain->setText(jidToDomain(m_pPara->GetSignalUser()));
#endif
    ui->leSignalPassword->setText(m_pPara->GetSignalPassword());
    ui->lePeerUser->setText(m_pPara->GetPeerUser());
    ui->leStunServer->setText(m_pPara->GetStunServer());
    ui->spStunPort->setValue(m_pPara->GetStunPort());
    ui->leTurnServer->setText(m_pPara->GetTurnServer());
    ui->spTurnPort->setValue(m_pPara->GetTurnPort());
    ui->leTurnUser->setText(m_pPara->GetTurnUser());
    ui->leTurnPassword->setText(m_pPara->GetTurnPassword());
    
    ui->leName->setText(m_pPara->GetName());
    ui->leUserName->setText(m_pPara->GetUser());
    ui->lePassword->setText(m_pPara->GetPassword());
    ui->cbSave->setChecked(m_pPara->GetSavePassword());
    ui->cbOnlyView->setChecked(m_pPara->GetOnlyView());
    
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
    case CParameterTigerVnc::Full:
        ui->rbFull->setChecked(true);
        break;
    case CParameterTigerVnc::Medium:
        ui->rbMeduim->setChecked(true);
        break;
    case CParameterTigerVnc::Low:
        ui->rbLow->setChecked(true);
        break;
    case CParameterTigerVnc::VeryLow:
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
    ui->leProxyServer->setText(m_pPara->GetProxyHost());
    ui->spProxyPort->setValue(m_pPara->GetProxyPort());
    ui->leProxyUser->setText(m_pPara->GetProxyUser());
    ui->leProxyPassword->setText(m_pPara->GetProxyPassword());
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

void CDlgSettingsTigerVnc::on_gpIce_clicked(bool checked)
{
    ui->leServer->setEnabled(!checked);
    ui->spPort->setEnabled(!checked);
}
