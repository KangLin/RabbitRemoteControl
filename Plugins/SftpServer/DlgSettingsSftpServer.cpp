// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
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
    m_pServerUI = new CParameterServerUI(ui->wContain);
    if(m_pServerUI) {
        m_pServerUI->SetParameter(pPara);
        ui->wContain->insertTab(0, m_pServerUI, m_pServerUI->windowIcon(), m_pServerUI->windowTitle());
    }
    ui->wContain->setCurrentIndex(0);

    ui->cbAnonymousLogin->setChecked(m_pPara->GetAnonymousLogin());
    ui->cbReadOnly->setChecked(m_pPara->GetReadOnly());
    ui->leRoot->setText(m_pPara->GetRoot());
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

    m_pPara->SetAnonymousLogin(ui->cbAnonymousLogin->isChecked());
    m_pPara->SetReadOnly(ui->cbReadOnly->isChecked());
    m_pPara->SetRoot(ui->leRoot->text());

    QDialog::accept();
}

void CDlgSettingsSftpServer::on_pbRoot_clicked()
{
    QString szDir = QFileDialog::getExistingDirectory(this, QString(), ui->leRoot->text());
    if(szDir.isEmpty())
        return;
    ui->leRoot->setText(szDir);
}