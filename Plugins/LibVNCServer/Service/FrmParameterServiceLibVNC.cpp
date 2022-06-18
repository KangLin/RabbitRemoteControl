#include "FrmParameterServiceLibVNC.h"
#include "ui_FrmParameterServiceLibVNC.h"

CFrmParameterServiceLibVNC::CFrmParameterServiceLibVNC(CParameterServiceLibVNC *para, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterServiceLibVNC),
    m_pParameters(para)
{
    ui->setupUi(this);
    Init();
}

CFrmParameterServiceLibVNC::~CFrmParameterServiceLibVNC()
{
    delete ui;
}

void CFrmParameterServiceLibVNC::slotAccept()
{
    m_pParameters->setEnable(ui->cbEnable->isChecked());
    m_pParameters->setPort(ui->sbPort->value());
    m_pParameters->setPassword(ui->lePassword->text());
}

void CFrmParameterServiceLibVNC::Init()
{
    ui->cbEnable->setChecked(m_pParameters->getEnable());
    ui->sbPort->setValue(m_pParameters->getPort());
    ui->lePassword->setText(m_pParameters->getPassword());
}
