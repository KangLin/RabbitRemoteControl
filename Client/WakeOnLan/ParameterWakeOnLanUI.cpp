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

    ui->leBroadcastAddress->setValidator(ipValidator);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    ui->leBroadcastAddress->setPlaceholderText("255.255.255.255");
#endif

    foreach(auto iface, QNetworkInterface::allInterfaces()) {
        //qDebug(log) << iface;
        auto entry = iface.addressEntries();
        if(iface.flags() & QNetworkInterface::IsLoopBack)
            continue;
        if(!(iface.flags() & QNetworkInterface::CanBroadcast))
            continue;
        QString szBroadcast;
        foreach(auto e, entry) {
            if(!e.broadcast().isNull()) {
                ui->cbNetworkInterface->addItem(
                    e.ip().toString(),
                    QStringList() <<  e.broadcast().toString()
                                  << e.netmask().toString());
            }
        }
    }
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
    ui->cbNetworkInterface->setCurrentText(m_pWakeOnLan->GetNetworkInterface());
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
    QString szBroadAddress = ui->leBroadcastAddress->text();
    if(QValidator::Acceptable != ipValidator.validate(szBroadAddress, pos))
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("The broadcast address is error"));
        qCritical(log) << "The broadcast address is error";
        ui->leBroadcastAddress->setFocus();
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
    m_pWakeOnLan->SetNetworkInterface(ui->cbNetworkInterface->currentText());
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

void CParameterWakeOnLanUI::on_cbNetworkInterface_currentIndexChanged(int index)
{
    ui->leBroadcastAddress->setText(ui->cbNetworkInterface->itemData(index).toStringList().at(0));
}

void CParameterWakeOnLanUI::slotHostChanged(const QString& szHost)
{
    QStringList lstMask = ui->cbNetworkInterface->currentData().toStringList();
    if(lstMask.length() != 2)
        return;
    QString szMask = lstMask[1];
    QString szIP = ui->cbNetworkInterface->currentText();
    if(szMask.isEmpty()) {
        qCritical(log) << "The mask is empty";
        return;
    }
    if(GetSubNet(szHost, szMask) == GetSubNet(szIP, szMask))
        return;
    for(int i = 0; i < ui->cbNetworkInterface->count(); i++) {
        szIP = ui->cbNetworkInterface->itemText(i);
        lstMask = ui->cbNetworkInterface->itemData(i).toStringList();
        if(lstMask.length() != 2) continue;
        szMask = lstMask[1];
        if(GetSubNet(szIP, szMask) == GetSubNet(szHost, szMask))
        {
            ui->cbNetworkInterface->setCurrentIndex(i);
            return;
        }
    }
}

#ifdef WIN32
#include <winsock2.h>
typedef unsigned long in_addr_t;
#else
#include <arpa/inet.h>
#endif

QString CParameterWakeOnLanUI::GetSubNet(const QString& szIP, const QString& szMask)
{
    in_addr_t ip = inet_addr(szIP.toStdString().c_str());
    in_addr_t mask = inet_addr(szMask.toStdString().c_str());
    in_addr_t subNet = 0;
    char* pIP = (char*)&ip;
    char* pMask = (char*)&mask;
    char* pSubNet = (char*)&subNet;
    for (int i = 0; i < 4; i++)
        pSubNet[i] = pMask[i] & pIP[i];
    std::string szSubNet = inet_ntoa((struct in_addr&)subNet);
    qDebug(log) << "SubNet:" << szSubNet.c_str();
    return szSubNet.c_str();
}
