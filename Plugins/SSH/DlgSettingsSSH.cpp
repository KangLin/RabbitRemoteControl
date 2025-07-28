// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsSSH.h"
#include "ui_DlgSettingsSSH.h"
#include "RabbitCommonDir.h"
#include <QStandardPaths>

CDlgSettingsSSH::CDlgSettingsSSH(CParameterTerminalSSH *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsSSH)
    , m_pPara(pPara)
{
    ui->setupUi(this);
    
    Q_ASSERT(m_pPara);
    
    ui->wNet->SetParameter(&m_pPara->m_SSH.m_Net);
    m_pFrmParaAppearnce =
        new CParameterTerminalUI(this);
    if(m_pFrmParaAppearnce) {
        m_pFrmParaAppearnce->SetParameter(&m_pPara->m_Terminal);
        ui->tabWidget->addTab(m_pFrmParaAppearnce,
                              m_pFrmParaAppearnce->windowTitle());
    }
    foreach(auto c, m_pPara->GetCommands()) {
        ui->lvCommands->addItem(c);
    }
}

CDlgSettingsSSH::~CDlgSettingsSSH()
{
    delete ui;
}

void CDlgSettingsSSH::on_pbOK_clicked()
{
    if(!ui->wNet->CheckValidity(true)) {
        ui->tabWidget->setCurrentIndex(0);
        return;
    }

    ui->wNet->Accept();
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->Accept();
    QStringList cmds;
    for(int i = 0; i < ui->lvCommands->count(); i++) {
        auto c = ui->lvCommands->item(i)->data(Qt::DisplayRole).toString();
        cmds << c;
    }
    if(!cmds.isEmpty())
        m_pPara->SetCommands(cmds);
    accept();
}

void CDlgSettingsSSH::on_pbCancel_clicked()
{
    reject();
}

void CDlgSettingsSSH::on_pbAddCommand_clicked()
{
    QString szCmd = ui->leCommand->text();
    if(szCmd.isEmpty())
        return;
    if(ui->lvCommands->findItems(szCmd, Qt::MatchCaseSensitive).isEmpty())
        ui->lvCommands->addItem(szCmd);
    ui->leCommand->setText(QString());
}

void CDlgSettingsSSH::on_leCommand_editingFinished()
{
    on_pbAddCommand_clicked();
}

void CDlgSettingsSSH::on_pbDeleteCommands_clicked()
{
    auto item = ui->lvCommands->currentItem();
    delete item;
}

