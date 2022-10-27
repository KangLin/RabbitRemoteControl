#include "FrmParameterRabbitVNC.h"
#include "ui_FrmParameterRabbitVNC.h"

#ifdef HAVE_ICE
    #include "Ice.h"
#endif

CFrmParameterRabbitVNC::CFrmParameterRabbitVNC(CParameterServiceRabbitVNC *para, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterRabbitVNC),
    m_pParameters(para)
{
    ui->setupUi(this);
    
#ifdef HAVE_ICE
    QSharedPointer<CIceSignal> signal = CICE::Instance()->GetSignal();
    if(signal)
        ui->cbEnableICE->setEnabled(true);
    else
        ui->cbEnableICE->setEnabled(false);
#endif

    ui->cbEnable->setChecked(m_pParameters->getEnable());
    ui->sbPort->setValue(m_pParameters->getPort());
    ui->lePassword->setText(m_pParameters->getPassword());
    ui->cbEnableICE->setChecked(m_pParameters->getIce());
    ui->cbEnableSocket->setChecked(m_pParameters->GetEnableSocket());
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
    m_pParameters->setIce(ui->cbEnableICE->isChecked());
    m_pParameters->SetEnableSocket(ui->cbEnableSocket->isChecked());
}

void CFrmParameterRabbitVNC::on_pbShow_clicked()
{
    if(ui->lePassword->echoMode() == QLineEdit::Password)
    {
        ui->lePassword->setEchoMode(QLineEdit::Normal);
        ui->pbShow->setIcon(QIcon::fromTheme("eye-off"));
    } else {
        ui->lePassword->setEchoMode(QLineEdit::Password);
        ui->pbShow->setIcon(QIcon::fromTheme("eye-on"));
    }
}
