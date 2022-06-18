#include "FrmParameterICE.h"
#include "ui_FrmParameterICE.h"
#include <QDebug>

CFrmParameterICE::CFrmParameterICE(CParameterICE *para, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterICE),
    m_pParameters(para)
{
    ui->setupUi(this);
    Init();
}

CFrmParameterICE::~CFrmParameterICE()
{
    delete ui;
    qDebug() << "CFrmParameterICE::~CFrmParameterICE()";
}

void CFrmParameterICE::slotAccept()
{
    m_pParameters->setIce(ui->cbEnableICE->isChecked());
    
    m_pParameters->setSignalServer(ui->leSignalServer->text());
    m_pParameters->setSignalPort(ui->sbSignalPort->value());
    m_pParameters->setSignalUser(ui->leSignalName->text());
    m_pParameters->setSignalPassword(ui->leSignalPassword->text());
    
    m_pParameters->setStunServer(ui->leStunServer->text());
    m_pParameters->setStunPort(ui->sbStunPort->value());
    
    m_pParameters->setTurnServer(ui->leTurnServer->text());
    m_pParameters->setTurnPassword(ui->leTrunPassword->text());
    m_pParameters->setTurnUser(ui->leTurnUser->text());
    m_pParameters->setTurnPassword(ui->leTrunPassword->text());
}

void CFrmParameterICE::Init()
{    
    ui->cbEnableICE->setChecked(m_pParameters->getIce());
    
    ui->leSignalServer->setText(m_pParameters->getSignalServer());
    ui->sbSignalPort->setValue(m_pParameters->getSignalPort());
    ui->leSignalName->setText(m_pParameters->getSignalUser());
    ui->leSignalPassword->setText(m_pParameters->getSignalPassword());
    
    ui->leStunServer->setText(m_pParameters->getStunServer());
    ui->sbStunPort->setValue(m_pParameters->getStunPort());
    
    ui->leTurnServer->setText(m_pParameters->getTurnServer());
    ui->sbTurnPort->setValue(m_pParameters->getTurnPort());
    ui->leTurnUser->setText(m_pParameters->getTurnUser());
    ui->leTrunPassword->setText(m_pParameters->getTurnPassword());
}

void CFrmParameterICE::on_cbEnableICE_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
        EnableCompone(true);
    else
        EnableCompone(false);
}

void CFrmParameterICE::EnableCompone(bool bEnable)
{
    ui->leSignalServer->setEnabled(bEnable);
    ui->sbSignalPort->setEnabled(bEnable);
    ui->leSignalName->setEnabled(bEnable);
    ui->leSignalPassword->setEnabled(bEnable);
    
    ui->leStunServer->setEnabled(bEnable);
    ui->sbStunPort->setEnabled(bEnable);
    
    ui->leTurnServer->setEnabled(bEnable);
    ui->sbTurnPort->setEnabled(bEnable);
    ui->leTurnUser->setEnabled(bEnable);
    ui->leTrunPassword->setEnabled(bEnable);
}
