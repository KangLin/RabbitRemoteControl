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
    setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* layout = new QHBoxLayout(ui->wContain);
    if(layout) {
        ui->wContain->setLayout(layout);
        layout->setContentsMargins(0, 0, 0, 0);
    }
    m_pServerUI = new CParameterServerUI(ui->wContain);
    if(m_pServerUI) {
        m_pServerUI->SetParameter(pPara);
        layout->addWidget(m_pServerUI);
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
