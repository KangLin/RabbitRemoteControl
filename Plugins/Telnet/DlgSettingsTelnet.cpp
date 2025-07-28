// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsTelnet.h"
#include "ui_DlgSettingsTelnet.h"

CDlgSettingsTelnet::CDlgSettingsTelnet(CParameterTelnet *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsTelnet)
    , m_pPara(pPara)
{
    ui->setupUi(this);

    Q_ASSERT(m_pPara);

    ui->leLogin->setText(pPara->GetLogin());
    ui->lePassword->setText(pPara->GetPassword());

    ui->wNet->SetParameter(&m_pPara->m_Net);
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

CDlgSettingsTelnet::~CDlgSettingsTelnet()
{
    delete ui;
}

void CDlgSettingsTelnet::on_pbOK_clicked()
{
    if(!ui->wNet->CheckValidity(true)) {
        ui->tabWidget->setCurrentIndex(0);
        return;
    }

    ui->wNet->Accept();
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->Accept();

    m_pPara->SetLogin(ui->leLogin->text());
    m_pPara->SetPassword(ui->lePassword->text());

    QStringList cmds;
    for(int i = 0; i < ui->lvCommands->count(); i++) {
        auto c = ui->lvCommands->item(i)->data(Qt::DisplayRole).toString();
        cmds << c;
    }
    if(!cmds.isEmpty())
        m_pPara->SetCommands(cmds);

    accept();
}

void CDlgSettingsTelnet::on_pbCancel_clicked()
{
    reject();
}

void CDlgSettingsTelnet::on_pbAddCommand_clicked()
{
    QString szCmd = ui->leCommand->text();
    if(szCmd.isEmpty())
        return;
    if(ui->lvCommands->findItems(szCmd, Qt::MatchCaseSensitive).isEmpty())
        ui->lvCommands->addItem(szCmd);
    ui->leCommand->setText(QString());
}

void CDlgSettingsTelnet::on_leCommand_editingFinished()
{
    on_pbAddCommand_clicked();
}
