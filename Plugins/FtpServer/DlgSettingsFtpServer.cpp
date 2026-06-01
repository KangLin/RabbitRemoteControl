// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
#include "ParameterFtpServer.h"
#include "DlgSettingsFtpServer.h"
#include "ui_DlgSettingsFtpServer.h"

CDlgSettingsFtpServer::CDlgSettingsFtpServer(CParameterFtpServer *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsFtpServer)
    , m_pPara(pPara)
    , m_pServerUI(nullptr)
{
    ui->setupUi(this);
    m_pServerUI = new CParameterServerUI();
    if(m_pServerUI) {
        m_pServerUI->SetParameter(pPara);
        ui->tabWidget->insertTab(0, m_pServerUI, m_pServerUI->windowIcon(), m_pServerUI->windowTitle());
    }
    ui->tabWidget->setCurrentIndex(0);
    ui->cbAnonymousLogin->setChecked(m_pPara->GetAnonymousLogin());
    ui->cbReadOnly->setChecked(m_pPara->GetReadOnly());
    ui->leRoot->setText(m_pPara->GetRoot());
}

CDlgSettingsFtpServer::~CDlgSettingsFtpServer()
{
    delete ui;
}

void CDlgSettingsFtpServer::accept()
{
    bool nRet = 0;
    nRet = m_pServerUI->CheckValidity(true);
    if(!nRet) return;
    if(m_pServerUI)
        m_pServerUI->Accept();
    m_pPara->SetAnonymousLogin(ui->cbAnonymousLogin->isChecked());
    m_pPara->SetReadOnly(ui->cbReadOnly->isChecked());
    m_pPara->SetRoot(ui->leRoot->text());


    QDialog::accept();
}

void CDlgSettingsFtpServer::on_pbRoot_clicked()
{
    QString szDir = QFileDialog::getExistingDirectory(this, QString(), ui->leRoot->text());
    if(szDir.isEmpty())
        return;
    ui->leRoot->setText(szDir);
}