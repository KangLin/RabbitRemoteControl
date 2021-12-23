#include "DlgSettings.h"
#include "ui_DlgSettings.h"

CDlgSettings::CDlgSettings(CParameterApp *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettings),
    m_pParameters(pPara)
{
    ui->setupUi(this);
    
    ui->cbShotRemoteDesktop->setChecked(m_pParameters->GetScreenShot());
    switch (m_pParameters->GetScreenShotEndAction()) {
    case CParameterApp::NoAction:
        ui->rbShotScreenNoAction->setChecked(true);
        break;
    case CParameterApp::OpenFile:
        ui->rbShotScreenOpenFile->setChecked(true);
        break;
    case CParameterApp::OpenFolder:
        ui->rbShotScreenOpenFolder->setChecked(true);
        break;    
    }
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::on_pbOk_clicked()
{
    if(!m_pParameters) return;
    
    m_pParameters->SetScreenShot(ui->cbShotRemoteDesktop->isChecked());
    
    if(ui->rbShotScreenNoAction->isChecked())
        m_pParameters->SetScreenShotEndAction(CParameterApp::NoAction);
    if(ui->rbShotScreenOpenFile->isChecked())
        m_pParameters->SetScreenShotEndAction(CParameterApp::OpenFile);
    if(ui->rbShotScreenOpenFolder->isChecked())
        m_pParameters->SetScreenShotEndAction(CParameterApp::OpenFolder);
    
    accept();
}

void CDlgSettings::on_pbNo_clicked()
{
    reject();
}
