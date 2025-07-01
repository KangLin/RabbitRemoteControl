//! @author Kang Lin <kl222@126.com>

#include "FrmParameterICE.h"
#include "ui_FrmParameterICE.h"

#include "Ice.h"
#ifdef HAVE_QXMPP
    #include "QXmppUtils.h"
#endif

#include <QDebug>
#include <QMessageBox>

CFrmParameterICE::CFrmParameterICE(CParameterICE *para, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterICE),
    m_pParameters(para)
{
    bool check = false;
    ui->setupUi(this);

    QSharedPointer<CIceSignal> signal = CICE::Instance()->GetSignal();
    if(signal)
    {
        SetConnectButton(signal->IsConnected());
        check = connect(signal.data(), SIGNAL(sigConnected()),
                        this, SLOT(slotConnected()));
        Q_ASSERT(check);
        check = connect(signal.data(), SIGNAL(sigDisconnected()),
                        this, SLOT(slotDisconnected()));
        Q_ASSERT(check);
        check = connect(signal.data(), SIGNAL(sigError(int, const QString&)),
                        this, SLOT(slotError(int, const QString&)));
        Q_ASSERT(check);
    }
    
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
    m_pParameters->SetIceDebug(ui->cbEnableIceDebug->isChecked());
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
    on_cbEnableICE_stateChanged(ui->cbEnableICE->isChecked());
    ui->cbEnableIceDebug->setChecked(m_pParameters->GetIceDebug());
    
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
    SetConnectButton(!checked);
    if(checked)
        CICE::Instance()->slotStart();
    else
        CICE::Instance()->slotStop();
}

void CFrmParameterICE::on_leSignalName_editingFinished()
{
#ifdef HAVE_QXMPP
    QString user = ui->leSignalName->text();
    if(QXmppUtils::jidToDomain(user).isEmpty()
            || QXmppUtils::jidToResource(user).isEmpty())
    {
        QString szMsg = tr("The user name format is error. please use format: user@domain/resource");
        QMessageBox::critical(this, tr("Error"), szMsg);
    }
#endif
}

void CFrmParameterICE::slotConnected()
{
    QSharedPointer<CIceSignal> signal = CICE::Instance()->GetSignal();
    if(signal)
        SetConnectButton(signal->IsConnected());
}

void CFrmParameterICE::slotDisconnected()
{
    QSharedPointer<CIceSignal> signal = CICE::Instance()->GetSignal();
    if(signal)
        SetConnectButton(signal->IsConnected());
}

void CFrmParameterICE::slotError(int nError, const QString& szError)
{
    QSharedPointer<CIceSignal> signal = CICE::Instance()->GetSignal();
    if(signal)
        SetConnectButton(signal->IsConnected());
}

void CFrmParameterICE::SetConnectButton(bool bConnected)
{
    if(bConnected)
    {
        ui->pbConnect->setChecked(true);
        ui->pbConnect->setText(tr("Disconnect"));
        ui->pbConnect->setIcon(QIcon::fromTheme("network-wireless"));
    } else {
        ui->pbConnect->setChecked(false);
        ui->pbConnect->setText(tr("Connect"));
        ui->pbConnect->setIcon(QIcon::fromTheme("network-wired"));
    }
}
