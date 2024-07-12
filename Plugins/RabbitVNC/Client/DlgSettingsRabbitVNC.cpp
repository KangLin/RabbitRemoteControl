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

Q_DECLARE_LOGGING_CATEGORY(RabbitVNC)

CDlgSettingsRabbitVNC::CDlgSettingsRabbitVNC(CParameterRabbitVNC *pPara, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::CDlgSettingsRabbitVNC),
    m_pPara(pPara),
    m_pParameterProxyUI(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    m_pParameterProxyUI = new CParameterProxyUI(ui->tabWidget);
    ui->tabWidget->insertTab(1, m_pParameterProxyUI, tr("Proxy"));

#ifdef HAVE_ICE
    ui->gpIce->show();
#else
    ui->gpIce->hide();
#endif
}

CDlgSettingsRabbitVNC::~CDlgSettingsRabbitVNC()
{
    qDebug(RabbitVNC) << "CDlgSettingsRabbitVnc::~CDlgSettingsRabbitVnc()";
    delete ui;
}

void CDlgSettingsRabbitVNC::on_pbOk_clicked()
{
    int nRet = 0;
    if(!m_pPara)
        return;
    
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

    m_pPara->SetName(ui->leName->text());
    
    nRet = ui->wNet->slotAccept(true);
    if(nRet) return;
    nRet = m_pParameterProxyUI->slotAccept();
    if(nRet) return;
    
    m_pPara->SetOnlyView(ui->cbOnlyView->isChecked());
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
            ui->wNet->setEnabled(false);
        } else {
            ui->gpIce->setChecked(false);
        }
    } else {
        ui->gpIce->setEnabled(false);
    }

    ui->lePeerUser->setText(m_pPara->GetPeerUser());
    
    ui->leName->setText(m_pPara->GetName());
    ui->wNet->SetParameter(&m_pPara->m_Net);
    
    ui->cbOnlyView->setChecked(m_pPara->GetOnlyView());
    ui->cbShowServerName->setChecked(m_pPara->GetShowServerName());

    ui->cbShared->setChecked(m_pPara->GetShared());
    ui->cbRealTimeUpdate->setChecked(!m_pPara->GetBufferEndRefresh());
    ui->cbLocalCursor->setChecked(m_pPara->GetLocalCursor());
    ui->cbResizeWindows->setChecked(m_pPara->GetSupportsDesktopResize());
    ui->cbClipboard->setChecked(m_pPara->GetClipboard());
    
    m_pParameterProxyUI->SetParameter(&m_pPara->m_Proxy);

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
}

void CDlgSettingsRabbitVNC::on_gpIce_clicked(bool checked)
{
    ui->wNet->setEnabled(!checked);
}
