#include "DlgSettingsTerminal.h"
#include "ui_DlgSettingsTerminal.h"

#include <QLoggingCategory>
#include <qtermwidget.h>

static Q_LOGGING_CATEGORY(log, "Terminal.DlgSettings")

CDlgSettingsTerminal::CDlgSettingsTerminal(CParameterTerminal *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTerminal),
    m_pPara(pPara)
{
    ui->setupUi(this);

    m_pFrmParaAppearnce =
            new CParameterTerminalUI(this);
    if(m_pFrmParaAppearnce) {
        m_pFrmParaAppearnce->SetParameter(m_pPara);
        ui->tabWidget->addTab(m_pFrmParaAppearnce,
                          m_pFrmParaAppearnce->windowTitle());
    }
}

CDlgSettingsTerminal::~CDlgSettingsTerminal()
{
    qDebug(log) << "CDlgSettingsTerminal::~CDlgSettingsTerminal()";
    delete ui;
}

void CDlgSettingsTerminal::on_pbOk_clicked()
{
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->Accept();

    accept();
}

void CDlgSettingsTerminal::on_pbCancel_clicked()
{
    this->reject();
}
