#include "DlgSettingsTerminal.h"
#include "ui_DlgSettingsTerminal.h"

#include <QLoggingCategory>
#include <qtermwidget.h>

Q_DECLARE_LOGGING_CATEGORY(Terminal)

CDlgSettingsTerminal::CDlgSettingsTerminal(CParameterTerminal *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTerminal),
    m_pPara(pPara)
{
    ui->setupUi(this);

    m_pFrmParaAppearnce =
            new CFrmParameterTerminalAppearanceSettings(m_pPara, this);
    if(m_pFrmParaAppearnce)
        ui->tabWidget->addTab(m_pFrmParaAppearnce,
                          m_pFrmParaAppearnce->windowTitle());
    m_pFrmParaBehavior = new CFrmParameterTerminalBehavior(m_pPara, this);
    if(m_pFrmParaBehavior)
        ui->tabWidget->addTab(m_pFrmParaBehavior,
                              m_pFrmParaBehavior->windowTitle());
}

CDlgSettingsTerminal::~CDlgSettingsTerminal()
{
    qDebug(Terminal) << "CDlgSettingsTerminal::~CDlgSettingsTerminal()";
    delete ui;
}

void CDlgSettingsTerminal::on_pbOk_clicked()
{
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->AcceptSettings();
    if(m_pFrmParaBehavior)
        m_pFrmParaBehavior->AcceptSettings();
    this->accept();
}

void CDlgSettingsTerminal::on_pbCancel_clicked()
{
    this->reject();
}
