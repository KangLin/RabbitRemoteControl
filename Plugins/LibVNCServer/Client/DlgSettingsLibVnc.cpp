// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsLibVnc.h"
#include "ui_DlgSettingsLibVnc.h"

#include <QLoggingCategory>
static Q_LOGGING_CATEGORY(log, "LibVNCServer.Setting.Dialog")

CDlgSettingsLibVnc::CDlgSettingsLibVnc(CParameterLibVNCServer *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsLibVnc),
    m_pPara(pPara),
    m_pProxyUI(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    // Server
    ui->leName->setText(m_pPara->GetName());
    ui->wNet->SetParameter(&m_pPara->m_Net);
    
    ui->cbShowServerName->setChecked(m_pPara->GetShowServerName());
    
    ui->cbShared->setChecked(m_pPara->GetShared());
    ui->cbOnlyView->setChecked(m_pPara->GetOnlyView());
    ui->cbLocalCursor->setChecked(m_pPara->GetLocalCursor());
    ui->cbClipboard->setChecked(m_pPara->GetClipboard());
    
    m_pProxyUI = new CParameterProxyUI(ui->tabWidget);
    m_pProxyUI->SetParameter(&m_pPara->m_Proxy);
    ui->tabWidget->insertTab(1, m_pProxyUI, tr("Proxy"));
        
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
    
}

CDlgSettingsLibVnc::~CDlgSettingsLibVnc()
{
    qDebug(log) << "CDlgSettingsLibVnc::~CDlgSettingsLibVnc()";
    delete ui;
}

void CDlgSettingsLibVnc::on_pbOk_clicked()
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

    // Server
    m_pPara->SetName(ui->leName->text());
    
    nRet = ui->wNet->Accept();
    if(nRet) return;
   
    m_pPara->SetShowServerName(ui->cbShowServerName->isChecked());
    m_pPara->SetShared(ui->cbShared->isChecked());
    m_pPara->SetOnlyView(ui->cbOnlyView->isChecked());
    m_pPara->SetLocalCursor(ui->cbLocalCursor->isChecked());
    m_pPara->SetClipboard(ui->cbClipboard->isChecked());
    
    nRet = m_pProxyUI->Accept();
    if(nRet)
        return;
    
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
    
    accept();
}

void CDlgSettingsLibVnc::on_pbCancel_clicked()
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
