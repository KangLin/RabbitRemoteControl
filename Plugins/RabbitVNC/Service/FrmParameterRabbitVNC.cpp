#include "FrmParameterRabbitVNC.h"
#include "ui_FrmParameterRabbitVNC.h"

CFrmParameterRabbitVNC::CFrmParameterRabbitVNC(CParameterServiceRabbitVNC *para, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterRabbitVNC),
    m_pParameters(para)
{
    ui->setupUi(this);

    Init();
}

CFrmParameterRabbitVNC::~CFrmParameterRabbitVNC()
{
    delete ui;
}

void CFrmParameterRabbitVNC::slotAccept()
{
    m_pParameters->setEnable(ui->cbEnable->isChecked());
    m_pParameters->setPort(ui->sbPort->value());
    m_pParameters->setPassword(ui->lePassword->text());
}

void CFrmParameterRabbitVNC::Init()
{
    ui->cbEnable->setChecked(m_pParameters->getEnable());
    ui->sbPort->setValue(m_pParameters->getPort());
    ui->lePassword->setText(m_pParameters->getPassword());
}

void CFrmParameterRabbitVNC::on_pbShow_clicked()
{
    if(ui->lePassword->echoMode() == QLineEdit::Password)
    {
        ui->lePassword->setEchoMode(QLineEdit::Normal);
        ui->pbShow->setIcon(QIcon(":/image/EyeOff"));
    } else {
        ui->lePassword->setEchoMode(QLineEdit::Password);
        ui->pbShow->setIcon(QIcon(":/image/EyeOn"));
    }
}
