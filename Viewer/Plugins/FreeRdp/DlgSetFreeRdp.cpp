#include "DlgSetFreeRdp.h"
#include "ui_DlgSetFreeRdp.h"

CDlgSetFreeRdp::CDlgSetFreeRdp(rdpSettings *pSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSetFreeRdp),
    m_pSettings(pSettings)
{
    ui->setupUi(this);
}

CDlgSetFreeRdp::~CDlgSetFreeRdp()
{
    delete ui;
}

void CDlgSetFreeRdp::on_pbOk_clicked()
{
    freerdp_settings_set_string(m_pSettings,
                                FreeRDP_ServerHostname,
                                ui->leServer->text().toStdString().c_str());
    m_pSettings->ServerPort = ui->spPort->value();
    freerdp_settings_set_string(m_pSettings,
                                FreeRDP_Username,
                                ui->leName->text().toStdString().c_str());
    freerdp_settings_set_string(m_pSettings,
                                FreeRDP_Password,
                                ui->lePassword->text().toStdString().c_str());
    accept();
}

void CDlgSetFreeRdp::on_pbCancel_clicked()
{
    reject();
}

void CDlgSetFreeRdp::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    Q_ASSERT(m_pSettings);

    ui->leServer->setText(freerdp_settings_get_string(m_pSettings, FreeRDP_ServerHostname));
    ui->spPort->setValue(m_pSettings->ServerPort);
    ui->leName->setText(freerdp_settings_get_string(m_pSettings, FreeRDP_Username));
    ui->lePassword->setText(freerdp_settings_get_string(m_pSettings, FreeRDP_Password));
}
