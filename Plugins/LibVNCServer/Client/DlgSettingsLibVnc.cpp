// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsLibVnc.h"
#include "ui_DlgSettingsLibVnc.h"
#include <QDebug>
#include "ParameterProxyUI.h"

CDlgSettingsLibVnc::CDlgSettingsLibVnc(CParameterLibVNCServer *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsLibVnc),
    m_pPara(pPara),
    m_uiProxy(nullptr)
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
    
    m_uiProxy = new CParameterProxyUI(ui->tabWidget);
    m_uiProxy->SetParameter(&m_pPara->m_Proxy);
    ui->tabWidget->addTab(m_uiProxy, tr("Proxy"));
    
    
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
    qDebug() << "CDlgSettingsLibVnc::~CDlgSettingsLibVnc()";
    delete ui;
}

void CDlgSettingsLibVnc::on_pbOk_clicked()
{
    int nRet = 0;

    if(!m_pPara)
        reject();
    
    // Server
    m_pPara->SetName(ui->leName->text());
    
    nRet = ui->wNet->slotAccept(true);
    if(nRet) return;
   
    m_pPara->SetShowServerName(ui->cbShowServerName->isChecked());
    m_pPara->SetShared(ui->cbShared->isChecked());
    m_pPara->SetOnlyView(ui->cbOnlyView->isChecked());
    m_pPara->SetLocalCursor(ui->cbLocalCursor->isChecked());
    m_pPara->SetClipboard(ui->cbClipboard->isChecked());
    
    nRet = m_uiProxy->slotAccept();
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
