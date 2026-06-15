// Author: Kang Lin <kl222@126.com>

#include "ParameterFreeRDPServer.h"
#include "DlgSettingsFreeRDPServer.h"
#include "ui_DlgSettingsFreeRDPServer.h"

CDlgSettingsFreeRDPServer::CDlgSettingsFreeRDPServer(
    CParameterFreeRDPServer *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsFreeRDPServer)
    , m_pPara(pPara)
    , m_pServerUI(nullptr)
{
    ui->setupUi(this);
    m_pServerUI = new CParameterServerUI();
    if(m_pServerUI) {
        m_pServerUI->SetParameter(pPara);
        ui->tabWidget->insertTab(0, m_pServerUI, m_pServerUI->windowIcon(), m_pServerUI->windowTitle());
        ui->tabWidget->setCurrentIndex(0);
    }
}

CDlgSettingsFreeRDPServer::~CDlgSettingsFreeRDPServer()
{
    delete ui;
}

void CDlgSettingsFreeRDPServer::accept()
{
    bool nRet = 0;
    nRet = m_pServerUI->CheckValidity(true);
    if(!nRet) return;
    if(m_pServerUI)
        m_pServerUI->Accept();
    // TODO: Accept parameters

    QDialog::accept();
}