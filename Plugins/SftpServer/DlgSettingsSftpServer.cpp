// Author: Kang Lin <kl222@126.com>

#include "ParameterSftpServer.h"
#include "DlgSettingsSftpServer.h"
#include "ui_DlgSettingsSftpServer.h"

CDlgSettingsSftpServer::CDlgSettingsSftpServer(CParameterSftpServer *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsSftpServer)
    , m_pPara(pPara)
{
    ui->setupUi(this);
}

CDlgSettingsSftpServer::~CDlgSettingsSftpServer()
{
    delete ui;
}

void CDlgSettingsSftpServer::accept()
{
    // TODO: Accept parameters

    QDialog::accept();
}