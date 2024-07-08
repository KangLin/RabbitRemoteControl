// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsTigerVnc.h"
#include "ui_DlgSettingsTigerVnc.h"
#include "rfb/encodings.h"
#include <QDebug>
#include <QMessageBox>

#ifdef HAVE_ICE
    #include "Ice.h"
#endif

#undef SetPort

static Q_LOGGING_CATEGORY(log, "VNC.Tiger")

CDlgSettingsTigerVnc::CDlgSettingsTigerVnc(CParameterTigerVnc *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTigerVnc),
    m_pPara(pPara)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    
    ui->wNet->SetParameter(&m_pPara->m_Net);
    
    m_pProxy = new CParameterProxyUI(ui->tabWidget);
    m_pProxy->SetParameter(&m_pPara->m_Proxy);
    ui->tabWidget->insertTab(1, m_pProxy, "Proxy");

#ifdef HAVE_ICE
    ui->gpIce->show();
#else
    ui->gpIce->hide();
#endif
    
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
            ui->wNet->setEnabled(false);
        } else {
            ui->gpIce->setChecked(false);
        }
    } else {
        ui->gpIce->setEnabled(false);
    }
    
    ui->lePeerUser->setText(m_pPara->GetPeerUser());
    
    ui->leName->setText(m_pPara->GetName());
    
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
}

CDlgSettingsTigerVnc::~CDlgSettingsTigerVnc()
{
    qDebug(log) << "CDlgSettingsTigerVnc::~CDlgSettingsTigerVnc()";
    delete ui;
}

void CDlgSettingsTigerVnc::on_pbOK_clicked()
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
    
    ui->wNet->slotAccept(true);
    m_pProxy->slotAccept();
    
    m_pPara->SetName(ui->leName->text());
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
    
    if(ui->rbFull->isChecked()) m_pPara->SetColorLevel(CParameterTigerVnc::Full);
    if(ui->rbMeduim->isChecked()) m_pPara->SetColorLevel(CParameterTigerVnc::Medium);
    if(ui->rbLow->isChecked()) m_pPara->SetColorLevel(CParameterTigerVnc::Low);
    if(ui->rbVeryLow->isChecked()) m_pPara->SetColorLevel(CParameterTigerVnc::VeryLow);

    m_pPara->SetEnableCompressLevel(ui->cbCompress->isChecked());
    m_pPara->SetCompressLevel(ui->spCompressLevel->value());
    m_pPara->SetNoJpeg(!ui->cbJPEG->isChecked());
    m_pPara->SetQualityLevel(ui->spJPEGLevel->value());
        
    accept();
}

void CDlgSettingsTigerVnc::on_pbCancel_clicked()
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

void CDlgSettingsTigerVnc::on_gpIce_clicked(bool checked)
{
    ui->wNet->setEnabled(!checked);
}
