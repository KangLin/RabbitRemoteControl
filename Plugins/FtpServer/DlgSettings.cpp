// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QHBoxLayout>
#include <QLoggingCategory>
#include "DlgSettings.h"
#include "ui_DlgSettings.h"

static Q_LOGGING_CATEGORY(log, "FtpServer.DlgSettings")
CDlgSettings::CDlgSettings(CParameterFtpServer* para, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettings)
    , m_Para(para)
    , m_pServerUI(nullptr)
{
    ui->setupUi(this);
    setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* layout = new QHBoxLayout(ui->wContain);
    if(layout) {
        ui->wContain->setLayout(layout);
        layout->setContentsMargins(0, 0, 0, 0);
    }
    m_pServerUI = new CParameterServerUI(ui->wContain);
    if(m_pServerUI) {
        m_pServerUI->SetParameter(m_Para);
        layout->addWidget(m_pServerUI);
    }
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::accept()
{
    bool nRet = 0;
    nRet = m_pServerUI->CheckValidity(true);
    if(!nRet) return;
    m_pServerUI->Accept();

    QDialog::accept();
}
