// Author: Kang Lin <kl222@126.com>

#include "ParameterServerUI.h"
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
    m_pServerUI = new CParameterServerUI();
    if(m_pServerUI) {
        m_pServerUI->SetParameter(pPara);
        ui->tabWidget->addTab(m_pServerUI, m_pServerUI->windowTitle());
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
    m_pServerUI->Accept();

    QDialog::accept();
}
