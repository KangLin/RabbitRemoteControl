#include "FrmParameterTigerVNC.h"
#include "ui_FrmParameterTigerVNC.h"

CFrmParameterTigerVNC::CFrmParameterTigerVNC(CParameterServiceTigerVNC *para, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterTigerVNC),
    m_pParameters(para)
{
    ui->setupUi(this);
    Init();
}

CFrmParameterTigerVNC::~CFrmParameterTigerVNC()
{
    delete ui;
}

void CFrmParameterTigerVNC::slotAccept()
{
    m_pParameters->setEnable(ui->cbEnable->isChecked());
    m_pParameters->setPort(ui->sbPort->value());
    m_pParameters->setPassword(ui->lePassword->text());
}

void CFrmParameterTigerVNC::Init()
{
    ui->cbEnable->setChecked(m_pParameters->getEnable());
    ui->sbPort->setValue(m_pParameters->getPort());
    ui->lePassword->setText(m_pParameters->getPassword());
}

void CFrmParameterTigerVNC::on_pbShow_clicked()
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

