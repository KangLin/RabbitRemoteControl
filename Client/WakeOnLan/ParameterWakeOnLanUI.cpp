// Author: Kang Lin <kl222@126.com>

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QRegExpValidator>
#include <QRegExp>
#else
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#endif

#include "ParameterWakeOnLanUI.h"
#include "ui_ParameterWakeOnLanUI.h"

CParameterWakeOnLanUI::CParameterWakeOnLanUI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CParameterWakeOnLanUI)
{
    ui->setupUi(this);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp rxMac("/^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$/");
    QRegExpValidator* macValidator = new QRegExpValidator(rxMac, this);
    QRegExp rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegExpValidator* ipValidator = new QRegExpValidator(rxIP, this);
    QRegExp rxPassword("\w{6}|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegExpValidator* passwordValidator = new QRegExpValidator(rxPassword, this);
#else
    QRegularExpression rxMac("/^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$/");
    QRegularExpressionValidator* macValidator = new QRegularExpressionValidator(rxMac, this);
    QRegularExpression rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegularExpressionValidator* ipValidator = new QRegularExpressionValidator(rxIP, this);
    QRegularExpression rxPassword("\w{6}|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegularExpressionValidator* passwordValidator = new QRegularExpressionValidator(rxPassword, this);
#endif
    ui->leMac->setValidator(macValidator);
    ui->leMac->setPlaceholderText("FF:FF:FF:FF:FF:FF");
    ui->leBroadcastAddress->setValidator(ipValidator);
    ui->leBroadcastAddress->setPlaceholderText("255.255.255.255");
    ui->lePassword->setValidator(passwordValidator);
}

CParameterWakeOnLanUI::~CParameterWakeOnLanUI()
{
    delete ui;
}

int CParameterWakeOnLanUI::SetParameter(CParameterWakeOnLan *pParameter)
{
    m_pWakeOnLan = pParameter;
    if(!m_pWakeOnLan) return -1;

    ui->gbWakeOnLan->setChecked(m_pWakeOnLan->GetEnable());
    ui->leMac->setText(m_pWakeOnLan->GetMac());
    ui->leBroadcastAddress->setText(m_pWakeOnLan->GetBroadcastAddress());
    ui->sbPort->setValue(m_pWakeOnLan->GetPort());
    ui->lePassword->setText(m_pWakeOnLan->GetPassword());
    ui->pbSave->setChecked(m_pWakeOnLan->GetSavePassword());
    on_pbSave_clicked();
    ui->pbShow->setEnabled(m_pWakeOnLan->GetParameterClient()->GetViewPassowrd());
    ui->sbRepeat->setValue(m_pWakeOnLan->GetRepeat());
    ui->sbInterval->setValue(m_pWakeOnLan->GetInterval());
    ui->sbDelay->setValue(m_pWakeOnLan->GetDelay());
    return 0;
}

int CParameterWakeOnLanUI::slotAccept(bool validity)
{
    if(!m_pWakeOnLan) return -1;

    m_pWakeOnLan->SetEnable(ui->gbWakeOnLan->isChecked());
    m_pWakeOnLan->SetMac(ui->leMac->text());
    m_pWakeOnLan->SetBroadcastAddress(ui->leBroadcastAddress->text());
    m_pWakeOnLan->SetPort(ui->sbPort->value());
    m_pWakeOnLan->SetPassword(ui->lePassword->text());
    m_pWakeOnLan->SetSavePassword(ui->pbSave->isChecked());
    m_pWakeOnLan->SetRepeat(ui->sbRepeat->value());
    m_pWakeOnLan->SetInterval(ui->sbInterval->value());
    m_pWakeOnLan->SetDelay(ui->sbDelay->value());

    return 0;
}

void CParameterWakeOnLanUI::on_pbShow_clicked()
{
    switch(ui->lePassword->echoMode())
    {
    case QLineEdit::Password:
        ui->lePassword->setEchoMode(QLineEdit::Normal);
        ui->pbShow->setIcon(QIcon::fromTheme("eye-off"));
        break;
    case QLineEdit::Normal:
        ui->lePassword->setEchoMode(QLineEdit::Password);
        ui->pbShow->setIcon(QIcon::fromTheme("eye-on"));
        break;
    default:
        ui->pbShow->setIcon(QIcon::fromTheme("eye-on"));
    }
}

void CParameterWakeOnLanUI::on_pbSave_clicked()
{
    QString szText;
    if(ui->pbSave->isChecked())
    {
        ui->lePassword->setEnabled(true);
        szText = tr("A password string of length 6. or Hexadecimal representation of 6 bytes");
        ui->lePassword->setToolTip(szText);
        ui->lePassword->setStatusTip(szText);
        ui->lePassword->setPlaceholderText(szText);
    } else {
        szText = tr("Please checked save password to enable");
        ui->lePassword->setToolTip(szText);
        ui->lePassword->setStatusTip(szText);
        ui->lePassword->setPlaceholderText(szText);
        ui->lePassword->setEnabled(false);
    }
}
