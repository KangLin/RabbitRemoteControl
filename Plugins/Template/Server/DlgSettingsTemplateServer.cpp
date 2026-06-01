// Author: Kang Lin <kl222@126.com>

#include "ParameterTemplateServer.h"
#include "DlgSettingsTemplateServer.h"
#include "ui_DlgSettingsTemplateServer.h"

CDlgSettingsTemplateServer::CDlgSettingsTemplateServer(CParameterTemplateServer *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsTemplateServer)
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

CDlgSettingsTemplateServer::~CDlgSettingsTemplateServer()
{
    delete ui;
}

void CDlgSettingsTemplateServer::accept()
{
    bool nRet = 0;
    nRet = m_pServerUI->CheckValidity(true);
    if(!nRet) return;
    if(m_pServerUI)
        m_pServerUI->Accept();
    // TODO: Accept parameters

    QDialog::accept();
}