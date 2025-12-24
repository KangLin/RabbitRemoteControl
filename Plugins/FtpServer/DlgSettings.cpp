// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
#include <QLoggingCategory>
#include "DlgSettings.h"
#include "ui_DlgSettings.h"
static Q_LOGGING_CATEGORY(log, "FtpServer.DlgSettings")
CDlgSettings::CDlgSettings(QSharedPointer<CParameterFtpServer> para, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettings)
    , m_Para(para)
{
    ui->setupUi(this);
    Q_ASSERT(m_Para);
    
    ui->sbPort->setValue(m_Para->GetPort());
    ui->leUser->setText(m_Para->GetUser());
    ui->lePassword->setText(m_Para->GetPassword());
    ui->leRoot->setText(m_Para->GetRoot());
    ui->cbAnonmousLogin->setChecked(m_Para->GetAnonymousLogin());
    ui->cbReadOnly->setChecked(m_Para->GetReadOnly());
    ui->sbConnectCount->setValue(m_Para->GetConnectCount());
    ui->sbConnectCount->setToolTip(tr("-1: Enable all\n 0: Disable all\n>0: Connect count"));
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::on_pbRoot_clicked()
{
    QString szDir = QFileDialog::getExistingDirectory(this, QString(), ui->leRoot->text());
    if(szDir.isEmpty())
        return;
    ui->leRoot->setText(szDir);
}

void CDlgSettings::accept()
{
    m_Para->SetPort(ui->sbPort->value());
    m_Para->SetUser(ui->leUser->text());
    m_Para->SetPassword(ui->lePassword->text());
    m_Para->SetRoot(ui->leRoot->text());
    m_Para->SetAnonymousLogin(ui->cbAnonmousLogin->isChecked());
    m_Para->SetReadOnly(ui->cbReadOnly->isChecked());
    m_Para->SetConnectCount(ui->sbConnectCount->value());
    QDialog::accept();
}

void CDlgSettings::on_cbAnonmousLogin_checkStateChanged(const Qt::CheckState &arg1)
{
    bool bEnable = (Qt::Checked != arg1);
    ui->lePassword->setEnabled(bEnable);
    ui->leUser->setEnabled(bEnable);
}

