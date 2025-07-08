// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "DlgFileTransfer.h"
#include "ui_DlgFileTransfer.h"

static Q_LOGGING_CATEGORY(log, "Plugin.FileTransfer.Dialog")
CDlgFileTransfer::CDlgFileTransfer(CParameterFileTransfer *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgFileTransfer)
    , m_pPara(pPara)
{
    ui->setupUi(this);
    ui->cbProtocol->addItem("FTP", (int)CParameterFileTransfer::Protocol::FTP);
    ui->cbProtocol->addItem("SFTP", (int)CParameterFileTransfer::Protocol::SFTP);
    int nIdx = ui->cbProtocol->findData((int)m_pPara->GetProtocol());
    if(-1 != nIdx)
        ui->cbProtocol->setCurrentIndex(nIdx);
    ui->wNet->SetParameter(&m_pPara->m_Net);
}

CDlgFileTransfer::~CDlgFileTransfer()
{
    qDebug(log) << Q_FUNC_INFO;
    delete ui;
}

void CDlgFileTransfer::accept()
{
    if(!ui->wNet->CheckValidity(true))
    {
        return;
    }
    ui->wNet->Accept();
    m_pPara->SetProtocol((CParameterFileTransfer::Protocol)ui->cbProtocol->currentData().toInt());
    QDialog::accept();
}
