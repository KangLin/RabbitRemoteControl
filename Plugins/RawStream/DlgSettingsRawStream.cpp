// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsRawStream.h"
#include "ui_DlgSettingsRawStream.h"

CDlgSettingsRawStream::CDlgSettingsRawStream(CParameterRawStream *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsRawStream)
    , m_pPara(pPara)
{
    ui->setupUi(this);
    m_pFrmParaAppearnce =
        new CParameterTerminalUI(this);
    if(m_pFrmParaAppearnce) {
        m_pFrmParaAppearnce->SetParameter(&m_pPara->m_Terminal);
        ui->tabWidget->addTab(m_pFrmParaAppearnce,
                              m_pFrmParaAppearnce->windowTitle());
    }

    ui->cbType->addItem(CParameterRawStream::GetTypeName(CParameterRawStream::TYPE::TCP), (int)CParameterRawStream::TYPE::TCP);
    ui->cbType->addItem(CParameterRawStream::GetTypeName(CParameterRawStream::TYPE::NamePipe), (int)CParameterRawStream::TYPE::NamePipe);
    
    CParameterRawStream::TYPE type = m_pPara->GetType();
    int index = ui->cbType->findData((int)type);
    if(-1 != index)
        ui->cbType->setCurrentIndex(index);

    ui->gpTcp->hide();
    ui->gpNamePipe->hide();
    if(CParameterRawStream::TYPE::TCP == type)
        ui->gpTcp->show();
    if(CParameterRawStream::TYPE::NamePipe == type)
        ui->gpNamePipe->show();

    ui->wTcpNet->SetParameter(&m_pPara->m_Net);
}

CDlgSettingsRawStream::~CDlgSettingsRawStream()
{
    delete ui;
}

void CDlgSettingsRawStream::accept()
{
    if(!ui->wTcpNet->CheckValidity(true)) {
        ui->tabWidget->setCurrentIndex(0);
        return;
    }

    m_pPara->SetType((CParameterRawStream::TYPE)ui->cbType->currentData().toInt());
    ui->wTcpNet->Accept();

    QDialog::accept();
}
