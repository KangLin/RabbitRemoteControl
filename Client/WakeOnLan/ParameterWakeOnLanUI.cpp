// Author: Kang Lin <kl222@126.com>

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QRegExpValidator>
#include <QRegExp>
#else
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#endif
#include <QMessageBox>
#include <QNetworkInterface>
#include <QLoggingCategory>
#include "ParameterWakeOnLanUI.h"
#include "ui_ParameterWakeOnLanUI.h"

static Q_LOGGING_CATEGORY(log, "WOL.Parameter.UI")
CParameterWakeOnLanUI::CParameterWakeOnLanUI(QWidget *parent)
    : CParameterUI(parent)
    , ui(new Ui::CParameterWakeOnLanUI)
{
    ui->setupUi(this);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp rxMac("^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegExpValidator* macValidator = new QRegExpValidator(rxMac, this);
    QRegExp rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegExpValidator* ipValidator = new QRegExpValidator(rxIP, this);
    QRegExp rxPassword("(.{6})?|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegExpValidator* passwordValidator = new QRegExpValidator(rxPassword, this);
#else
    QRegularExpression rxMac("^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegularExpressionValidator* macValidator = new QRegularExpressionValidator(rxMac, this);
    QRegularExpression rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegularExpressionValidator* ipValidator = new QRegularExpressionValidator(rxIP, this);
    QRegularExpression rxPassword("(.{6})?|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegularExpressionValidator* passwordValidator = new QRegularExpressionValidator(rxPassword, this);
#endif
    ui->leMac->setValidator(macValidator);
    ui->leMac->setPlaceholderText("FF:FF:FF:FF:FF:FF");
    ui->lePassword->setValidator(passwordValidator);

    ui->cbBroadcastAddress->setValidator(ipValidator);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    ui->cbBroadcastAddress->setPlaceholderText("255.255.255.255");
#endif
    foreach(auto interface, QNetworkInterface::allInterfaces()) {
        //qDebug(log) << interface;
        auto entry = interface.addressEntries();
        if(interface.flags() & QNetworkInterface::IsLoopBack)
            continue;
        if(!(interface.flags() & QNetworkInterface::CanBroadcast))
            continue;
        foreach(auto e, entry) {
            if(!e.broadcast().isNull()
                && -1 == ui->cbBroadcastAddress->findText(e.broadcast().toString()))
                ui->cbBroadcastAddress->addItem(e.broadcast().toString());
        }
    }
    if(-1 == ui->cbBroadcastAddress->findText("255.255.255.255"))
        ui->cbBroadcastAddress->addItem("255.255.255.255");
}

CParameterWakeOnLanUI::~CParameterWakeOnLanUI()
{
    delete ui;
}

int CParameterWakeOnLanUI::SetParameter(CParameter *pParameter)
{
    m_pWakeOnLan = qobject_cast<CParameterWakeOnLan*>(pParameter);
    if(!m_pWakeOnLan) return -1;

    ui->gbWakeOnLan->setChecked(m_pWakeOnLan->GetEnable());
    ui->leMac->setText(m_pWakeOnLan->GetMac());
    int nIndex = ui->cbBroadcastAddress->findText(m_pWakeOnLan->GetBroadcastAddress());
    if(-1 == nIndex)
        ui->cbBroadcastAddress->addItem(m_pWakeOnLan->GetBroadcastAddress());
    ui->cbBroadcastAddress->setCurrentText(m_pWakeOnLan->GetBroadcastAddress());
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

bool CParameterWakeOnLanUI::CheckValidity(bool validity)
{
    if(!validity) return true;
    if(!ui->gbWakeOnLan->isChecked()) return true;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp rxMac("^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegExpValidator macValidator(rxMac, this);
    QRegExp rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegExpValidator ipValidator(rxIP, this);
    QRegExp rxPassword("(.{6})?|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegExpValidator passwordValidator(rxPassword, this);
#else
    QRegularExpression rxMac("^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegularExpressionValidator macValidator(rxMac, this);
    QRegularExpression rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegularExpressionValidator ipValidator(rxIP, this);
    QRegularExpression rxPassword("(.{6})?|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
    QRegularExpressionValidator passwordValidator(rxPassword, this);
#endif

    QValidator::State state = QValidator::Invalid;
    int pos = 0;
    QString szBroadAddress = ui->cbBroadcastAddress->currentText();
    if(QValidator::Acceptable != ipValidator.validate(szBroadAddress, pos))
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("The broadcast address is error"));
        qCritical(log) << "The broadcast address is error";
        ui->cbBroadcastAddress->setFocus();
        return false;
    }
    pos = 0;
    QString szPassword = ui->lePassword->text();
    if(QValidator::Acceptable != passwordValidator.validate(szPassword, pos))
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("The password is error."
                                 " A password string of length 6. "
                                 "or Hexadecimal representation of 6 bytes"));
        qCritical(log) << "The password is error";
        ui->lePassword->setFocus();
        return false;
    }
    pos = 0;
    QString szMac = ui->leMac->text();
    if(QValidator::Acceptable != macValidator.validate(szMac, pos)) {
        QMessageBox::critical(this, tr("Error"),
                              tr("The mac address is error"));
        qCritical(log) << "The mac address is error";
        ui->leMac->setFocus();
        return false;
    }
    return true;
}

int CParameterWakeOnLanUI::Accept()
{
    if(!m_pWakeOnLan) return -1;

    m_pWakeOnLan->SetEnable(ui->gbWakeOnLan->isChecked());
    m_pWakeOnLan->SetMac(ui->leMac->text());
    m_pWakeOnLan->SetBroadcastAddress(ui->cbBroadcastAddress->currentText());
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
        szText = tr("A password string of length 6."
                    " or Hexadecimal representation of 6 bytes");
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

// void CParameterWakeOnLanUI::on_cbBroadcastAddress_editTextChanged(const QString &arg1)
// {
//     qDebug(log) << "CParameterWakeOnLanUI::on_cbBroadcastAddress_editTextChanged" << arg1;
// #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//     QRegExp rxMac("^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
//     QRegExpValidator macValidator(rxMac, this);
//     QRegExp rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
//     QRegExpValidator ipValidator(rxIP, this);
//     QRegExp rxPassword("(.{6})?|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
//     QRegExpValidator passwordValidator(rxPassword, this);
// #else
//     QRegularExpression rxMac("^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
//     QRegularExpressionValidator macValidator(rxMac, this);
//     QRegularExpression rxIP("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
//     QRegularExpressionValidator ipValidator(rxIP, this);
//     QRegularExpression rxPassword("(.{6})?|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$");
//     QRegularExpressionValidator passwordValidator(rxPassword, this);
// #endif
//     int pos = 0;
//     QString szBroadAddress = arg1;
//     if(QValidator::Acceptable != ipValidator.validate(szBroadAddress, pos))
//         return;
//     if(!arg1.isEmpty() && -1 == ui->cbBroadcastAddress->findText(arg1))
//         ui->cbBroadcastAddress->addItem(arg1);
// }
