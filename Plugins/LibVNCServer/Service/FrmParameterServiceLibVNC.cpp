#include "FrmParameterServiceLibVNC.h"
#include "ui_FrmParameterServiceLibVNC.h"

CFrmParameterServiceLibVNC::CFrmParameterServiceLibVNC(CParameterServiceLibVNC *para) :
    QWidget(nullptr),
    ui(new Ui::CFrmParameterServiceLibVNC),
    m_pParameters(para)
{
    ui->setupUi(this);
}

CFrmParameterServiceLibVNC::~CFrmParameterServiceLibVNC()
{
    delete ui;
}

void CFrmParameterServiceLibVNC::slotSave()
{
    m_pParameters->setEnable(ui->cbEnable->isChecked());
    m_pParameters->setPort(ui->sbPort->value());
    m_pParameters->setPassword(ui->lePassword->text());
}

void CFrmParameterServiceLibVNC::showEvent(QShowEvent *event)
{
    ui->cbEnable->setChecked(m_pParameters->getEnable());
    ui->sbPort->setValue(m_pParameters->getPort());
    ui->lePassword->setText(m_pParameters->getPassword());
}
