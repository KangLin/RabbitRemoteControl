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
    m_pServerUI = new CParameterServerUI();
    if(m_pServerUI) {
        m_pServerUI->SetParameter(m_Para);
        ui->wContain->addTab(m_pServerUI, m_pServerUI->windowTitle());
    }
    m_pWhitelist = new CParameterFilterUI();
    if(m_pWhitelist) {
        m_pWhitelist->SetParameter(&m_Para->m_WhiteFilter);
        ui->wContain->addTab(m_pWhitelist, tr("Whitelist"));
    }
    m_pBlackList = new CParameterFilterUI();
    if(m_pBlackList) {
        m_pBlackList->SetParameter(&m_Para->m_BlackFilter);
        ui->wContain->addTab(m_pBlackList, tr("Blacklist"));
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
    if(m_pServerUI)
        m_pServerUI->Accept();
    if(m_pWhitelist)
        m_pWhitelist->Accept();
    if(m_pBlackList)
        m_pBlackList->Accept();

    QDialog::accept();
}
