// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsSSH.h"
#include "ui_DlgSettingsSSH.h"
#include "RabbitCommonDir.h"
#include <QStandardPaths>

CDlgSettingsSSH::CDlgSettingsSSH(CParameterTerminalSSH *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsSSH)
    , m_pPara(pPara)
{
    ui->setupUi(this);
    
    Q_ASSERT(m_pPara);
    
    ui->wNet->SetParameter(&m_pPara->m_SSH.m_Net);
    m_pFrmParaAppearnce =
        new CParameterTerminalUI(this);
    if(m_pFrmParaAppearnce) {
        m_pFrmParaAppearnce->SetParameter(&m_pPara->m_Terminal);
        ui->tabWidget->addTab(m_pFrmParaAppearnce,
                              m_pFrmParaAppearnce->windowTitle());
    }
}

CDlgSettingsSSH::~CDlgSettingsSSH()
{
    delete ui;
}

void CDlgSettingsSSH::on_pbOK_clicked()
{
    if(!ui->wNet->CheckValidity(true)) {
        ui->tabWidget->setCurrentIndex(0);
        return;
    }

    ui->wNet->Accept();
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->Accept();
    
    accept();
}

void CDlgSettingsSSH::on_pbCancel_clicked()
{
    reject();
}
