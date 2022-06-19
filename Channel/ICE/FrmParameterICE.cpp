#include "FrmParameterICE.h"
#include "ui_FrmParameterICE.h"
#include <QDebug>
#include "Ice.h"

CFrmParameterICE::CFrmParameterICE(CParameterICE *para, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterICE),
    m_pParameters(para)
{
    ui->setupUi(this);

    if(CICE::Instance()->GetSignal()
            && CICE::Instance()->GetSignal()->IsConnected())
    {
        ui->pbConnect->setChecked(true);
        ui->pbConnect->setText(tr("Disconnect"));
        ui->pbConnect->setIcon(QIcon(":/image/Disconnect"));
    }
    else
    {
        ui->pbConnect->setChecked(false);
        ui->pbConnect->setText(tr("Connect"));
        ui->pbConnect->setIcon(QIcon(":/image/Connect"));
    }
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

void CFrmParameterICE::showEvent(QShowEvent *event)
{    
    ui->cbEnableICE->setChecked(m_pParameters->getIce());
    on_cbEnableICE_stateChanged(ui->cbEnableICE->isChecked());
    
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
    EnableCompone(arg1);
}

void CFrmParameterICE::EnableCompone(bool bEnable)
{
    ui->pbConnect->setEnabled(bEnable);
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

void CFrmParameterICE::on_pbConnect_clicked(bool checked)
{
    if(checked)
    {
        ui->pbConnect->setIcon(QIcon(":/image/Disconnect"));
        ui->pbConnect->setText(tr("Disconnect"));
        CICE::Instance()->slotStart();
    }
    else
    {
        ui->pbConnect->setIcon(QIcon(":/image/Connect"));
        ui->pbConnect->setText(tr("Connect"));
        CICE::Instance()->slotStop();
    }
}
