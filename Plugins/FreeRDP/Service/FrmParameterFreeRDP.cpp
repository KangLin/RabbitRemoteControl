// Author: Kang Lin <kl222@126.com>

#include "FrmParameterFreeRDP.h"
#include "ui_FrmParameterFreeRDP.h"

CFrmParameterFreeRDP::CFrmParameterFreeRDP(CParameterServiceFreeRDP *para, QWidget* parent)
    : QWidget(parent),
    ui(new Ui::CFrmParameterFreeRDP),
    m_Log("FreeRDP"),
    m_Para(para)
{
    ui->setupUi(this);
    Init();
}

CFrmParameterFreeRDP::~CFrmParameterFreeRDP()
{
    qDebug(m_Log) << "CFrmParameterFreeRDP::~CFrmParameterFreeRDP";
    delete ui;
}

void CFrmParameterFreeRDP::Init()
{
    ui->cbEnable->setChecked(m_Para->getEnable());
    ui->lePort->setText(QString::number(m_Para->getPort()));
    ui->lePassword->setText(m_Para->getPassword());
    
    ui->cbAuthentication->setChecked(m_Para->getAuthentication());
    ui->cbMayInteract->setChecked(m_Para->getMayInteract());
    ui->cbMayView->setChecked(m_Para->getMayView());
    
    ui->cbTlsSecurity->setChecked(m_Para->getTlsSecurity());
    ui->cbRdpSecurity->setChecked(m_Para->getRdpSecurity());
    ui->cbNlaSecurity->setChecked(m_Para->getNlaSecurity());
    ui->cbNlaExtSecurity->setChecked(m_Para->getNlaExtSecurity());
    ui->leSamFile->setText(m_Para->getSamFile());
}

void CFrmParameterFreeRDP::slotAccept()
{
    m_Para->setEnable(ui->cbEnable->isChecked());
    m_Para->setPort(ui->lePort->text().toUInt());
    m_Para->setPassword(ui->lePassword->text());
    
    m_Para->setAuthentication(ui->cbAuthentication->isChecked());
    m_Para->setMayInteract(ui->cbMayInteract->isChecked());
    m_Para->setMayView(ui->cbMayView->isChecked());
    
    m_Para->setTlsSecurity(ui->cbTlsSecurity->isChecked());
    m_Para->setRdpSecurity(ui->cbRdpSecurity->isChecked());
    m_Para->setNlaSecurity(ui->cbNlaSecurity->isChecked());
    m_Para->setNlaExtSecurity(ui->cbNlaExtSecurity->isChecked());
    m_Para->setSamFile(ui->leSamFile->text());
}
