// Author: Kang Lin <kl222@126.com>

#include <QHBoxLayout>
#include "ParameterSftpServer.h"
#include "DlgSettingsSftpServer.h"
#include "ui_DlgSettingsSftpServer.h"

CDlgSettingsSftpServer::CDlgSettingsSftpServer(CParameterSftpServer *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsSftpServer)
    , m_pPara(pPara)
    , m_pServerUI(nullptr)
{
    ui->setupUi(this);
    m_pServerUI = new CParameterServerUI(ui->tabWidget);
    if(m_pServerUI) {
        m_pServerUI->SetParameter(pPara);
        ui->tabWidget->addTab(m_pServerUI, m_pServerUI->windowTitle());
    }
    m_pWhitelist = new CParameterFilterUI(ui->tabWidget);
    if(m_pWhitelist) {
        m_pWhitelist->SetParameter(&m_pPara->m_WhiteFilter);
        ui->tabWidget->addTab(m_pWhitelist, tr("Whitelist"));
    }
    m_pBlacklist = new CParameterFilterUI(ui->tabWidget);
    if(m_pBlacklist) {
        m_pBlacklist->SetParameter(&m_pPara->m_BlackFilter);
        ui->tabWidget->addTab(m_pBlacklist, tr("Blacklist"));
    }
}

CDlgSettingsSftpServer::~CDlgSettingsSftpServer()
{
    delete ui;
}

void CDlgSettingsSftpServer::accept()
{
    bool nRet = 0;
    nRet = m_pServerUI->CheckValidity(true);
    if(!nRet) return;
    if(m_pServerUI)
        m_pServerUI->Accept();
    if(m_pWhitelist)
        m_pWhitelist->Accept();
    if(m_pBlacklist)
        m_pBlacklist->Accept();
    QDialog::accept();
}
