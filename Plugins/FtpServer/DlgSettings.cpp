// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLoggingCategory>
#include "DlgSettings.h"
#include "ui_DlgSettings.h"

static Q_LOGGING_CATEGORY(log, "FtpServer.DlgSettings")
CDlgSettings::CDlgSettings(CParameterFtpServer* para, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettings)
    , m_pPara(para)
    , m_pServerUI(nullptr)
{
    ui->setupUi(this);
    m_pServerUI = new CParameterServerUI();
    if(m_pServerUI) {
        m_pServerUI->SetParameter(m_pPara);
        ui->wContain->insertTab(0, m_pServerUI, m_pServerUI->windowIcon(), m_pServerUI->windowTitle());
    }
    ui->wContain->setCurrentIndex(0);
    ui->cbAnonymousLogin->setChecked(m_pPara->GetAnonymousLogin());
    ui->cbReadOnly->setChecked(m_pPara->GetReadOnly());
    ui->leRoot->setText(m_pPara->GetRoot());
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::accept()
{
    bool nRet = 0;
    if(!m_pPara) return;

    nRet = m_pServerUI->CheckValidity(true);
    if(!nRet) return;
    if(m_pServerUI)
        m_pServerUI->Accept();
    m_pPara->SetAnonymousLogin(ui->cbAnonymousLogin->isChecked());
    m_pPara->SetReadOnly(ui->cbReadOnly->isChecked());
    m_pPara->SetRoot(ui->leRoot->text());

    QDialog::accept();
}

void CDlgSettings::on_pbRoot_clicked()
{
    QString szDir = QFileDialog::getExistingDirectory(this, QString(), ui->leRoot->text());
    if(szDir.isEmpty())
        return;
    ui->leRoot->setText(szDir);
}