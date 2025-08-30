// Author: Kang Lin <kl222@126.com>

#include <QMessageBox>
#include <QLoggingCategory>
#include "DlgSettingsVnc.h"
#include "ui_DlgSettingsVnc.h"
#include "rfb/encodings.h"

#ifdef HAVE_ICE
    #include "Ice.h"
#endif

#undef SetPort

static Q_LOGGING_CATEGORY(log, "VNC.Settings")

CDlgSettingsVnc::CDlgSettingsVnc(CParameterVnc *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsVnc),
    m_pPara(pPara)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    ui->wNet->SetParameter(&m_pPara->m_Net);

    m_pProxyUI = new CParameterProxyUI(ui->tabWidget);
    m_pProxyUI->SetParameter(&m_pPara->m_Proxy);
    ui->tabWidget->insertTab(1, m_pProxyUI, tr("Proxy"));

    m_pRecordUI = new CParameterRecordUI(ui->tabWidget);
    m_pRecordUI->SetParameter(&m_pPara->m_Record);
    ui->tabWidget->addTab(m_pRecordUI, m_pRecordUI->windowIcon(), tr("Record"));

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
    ui->cbCursorPosition->setChecked(m_pPara->GetCursorPosition());
    ui->cbResizeWindows->setChecked(m_pPara->GetSupportsDesktopResize());
    ui->cbLedState->setChecked(m_pPara->GetLedState());
    ui->cbClipboard->setChecked(m_pPara->GetClipboard());
    
    // Compress
    ui->cbCompressAutoSelect->setChecked(m_pPara->GetAutoSelect());
#ifdef HAVE_H264
    ui->rbH264->setVisible(true);
#else
    ui->rbH264->setVisible(false);
#endif
    switch(m_pPara->GetPreferredEncoding())
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
#ifdef HAVE_H264
    case rfb::encodingH264:
        ui->rbH264->setChecked(true);
        break;
#endif
    }
    
    switch(m_pPara->GetColorLevel())
    {
    case CParameterVnc::Full:
        ui->rbFull->setChecked(true);
        break;
    case CParameterVnc::Medium:
        ui->rbMedium->setChecked(true);
        break;
    case CParameterVnc::Low:
        ui->rbLow->setChecked(true);
        break;
    case CParameterVnc::VeryLow:
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

CDlgSettingsVnc::~CDlgSettingsVnc()
{
    qDebug(log) << "CDlgSettingsVnc::~CDlgSettingsVnc()";
    delete ui;
}

void CDlgSettingsVnc::on_pbOK_clicked()
{
    int nRet = 0;
    if(!m_pPara)
        return;

    if(!ui->wNet->CheckValidity(true)) {
        ui->tabWidget->setCurrentIndex(0);
        return;
    }

    if(!m_pProxyUI->CheckValidity(true)) {
        ui->tabWidget->setCurrentWidget(m_pProxyUI);
        return;
    }

    if(!m_pRecordUI->CheckValidity(true)) {
        ui->tabWidget->setCurrentWidget(m_pRecordUI);
        return;
    }

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

    nRet = ui->wNet->Accept();
    if(nRet) return;
    nRet = m_pProxyUI->Accept();
    if(nRet) return;
    nRet = m_pRecordUI->Accept();
    if(nRet) return;

    m_pPara->SetName(ui->leName->text());
    m_pPara->SetShared(ui->cbShared->isChecked());
    m_pPara->SetBufferEndRefresh(!ui->cbRealTimeUpdate->isChecked());
    m_pPara->SetLocalCursor(ui->cbLocalCursor->isChecked());
    m_pPara->SetCursorPosition(ui->cbCursorPosition->isChecked());
    m_pPara->SetSupportsDesktopResize(ui->cbResizeWindows->isChecked());
    m_pPara->SetLedState(ui->cbLedState->isChecked());
    m_pPara->SetClipboard(ui->cbClipboard->isChecked());
    m_pPara->SetShowServerName(ui->cbShowServerName->isChecked());
    
    // Compress
    m_pPara->SetAutoSelect(ui->cbCompressAutoSelect->isChecked());
    if(m_pPara->GetAutoSelect()) {
        m_pPara->SetPreferredEncoding(rfb::encodingTight);
        m_pPara->SetColorLevel(CParameterVnc::Full);
        m_pPara->SetCompressLevel(2);
        m_pPara->SetQualityLevel(8);
    } else {
        if(ui->rbTight->isChecked()) m_pPara->SetPreferredEncoding(rfb::encodingTight);
        if(ui->rbRaw->isChecked()) m_pPara->SetPreferredEncoding(rfb::encodingRaw);
        if(ui->rbRRE->isChecked()) m_pPara->SetPreferredEncoding(rfb::encodingRRE);
        if(ui->rbZRLE->isChecked()) m_pPara->SetPreferredEncoding(rfb::encodingZRLE);
        if(ui->rbCoRRE->isChecked()) m_pPara->SetPreferredEncoding(rfb::encodingCoRRE);
        if(ui->rbCopyRect->isChecked()) m_pPara->SetPreferredEncoding(rfb::encodingCopyRect);
        if(ui->rbHextile->isChecked()) m_pPara->SetPreferredEncoding(rfb::encodingHextile);
#ifdef HAVE_H264
        if(ui->rbH264->isChecked()) m_pPara->SetPreferredEncoding(rfb::encodingH264);
#endif
        
        if(ui->rbFull->isChecked()) m_pPara->SetColorLevel(CParameterVnc::Full);
        if(ui->rbMedium->isChecked()) m_pPara->SetColorLevel(CParameterVnc::Medium);
        if(ui->rbLow->isChecked()) m_pPara->SetColorLevel(CParameterVnc::Low);
        if(ui->rbVeryLow->isChecked()) m_pPara->SetColorLevel(CParameterVnc::VeryLow);
        
        m_pPara->SetEnableCompressLevel(ui->cbCompress->isChecked());
        m_pPara->SetCompressLevel(ui->spCompressLevel->value());
        m_pPara->SetNoJpeg(!ui->cbJPEG->isChecked());
        m_pPara->SetQualityLevel(ui->spJPEGLevel->value());
    }
    
    accept();
}

void CDlgSettingsVnc::on_pbCancel_clicked()
{
    reject();
}

void CDlgSettingsVnc::on_cbCompressAutoSelect_stateChanged(int arg1)
{
    m_pPara->SetAutoSelect(arg1);
    if(m_pPara->GetAutoSelect())
    {
        ui->gpPreferredEncodeing->setEnabled(false);
        ui->gpColorLevel->setEnabled(false);
        ui->cbJPEG->setEnabled(false);
        ui->spJPEGLevel->setEnabled(false);
        ui->cbCompress->setEnabled(false);
        ui->spCompressLevel->setEnabled(false);
    } else {
        ui->gpPreferredEncodeing->setEnabled(true);
        ui->gpColorLevel->setEnabled(true);
        ui->cbJPEG->setEnabled(true);
        if(ui->cbJPEG->isChecked())
        ui->spJPEGLevel->setEnabled(true);
        ui->cbCompress->setEnabled(true);
        if(ui->cbCompress->isChecked())
        ui->spCompressLevel->setEnabled(true);
    }
}

void CDlgSettingsVnc::on_cbCompress_stateChanged(int arg1)
{
    m_pPara->SetCompressLevel(arg1);
    
    ui->spCompressLevel->setEnabled(m_pPara->GetEnableCompressLevel());
}

void CDlgSettingsVnc::on_cbJPEG_stateChanged(int arg1)
{
    m_pPara->SetNoJpeg(!arg1);
    
    ui->spJPEGLevel->setEnabled(!m_pPara->GetNoJpeg());
}

void CDlgSettingsVnc::on_gpIce_clicked(bool checked)
{
    ui->wNet->setEnabled(!checked);
}
