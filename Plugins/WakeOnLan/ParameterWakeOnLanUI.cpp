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

#define RegularIp "^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$"
#define RegularMac "^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$"
#define RegularPassword "(.{6})?|^[A-Fa-f0-9]{2}(-[A-Fa-f0-9]{2}){5}$|^[A-Fa-f0-9]{2}(:[A-Fa-f0-9]{2}){5}$"
CParameterWakeOnLanUI::CParameterWakeOnLanUI(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CParameterWakeOnLanUI)
{
    ui->setupUi(this);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp rxMac(RegularMac);
    QRegExpValidator* macValidator = new QRegExpValidator(rxMac, this);
    QRegExp rxIP(RegularIp);
    QRegExpValidator* ipValidator = new QRegExpValidator(rxIP, this);
    QRegExp rxPassword(RegularPassword);
    QRegExpValidator* passwordValidator = new QRegExpValidator(rxPassword, this);
#else
    QRegularExpression rxMac(RegularMac);
    QRegularExpressionValidator* macValidator = new QRegularExpressionValidator(rxMac, this);
    QRegularExpression rxIP(RegularIp);
    QRegularExpressionValidator* ipValidator = new QRegularExpressionValidator(rxIP, this);
    QRegularExpression rxPassword(RegularPassword);
    QRegularExpressionValidator* passwordValidator = new QRegularExpressionValidator(rxPassword, this);
#endif
    ui->leIP->setValidator(ipValidator);
    ui->leMac->setValidator(macValidator);
    ui->leMac->setPlaceholderText("FF:FF:FF:FF:FF:FF");
    ui->lePassword->setValidator(passwordValidator);

    ui->leBroadcastAddress->setValidator(ipValidator);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    ui->leBroadcastAddress->setPlaceholderText("255.255.255.255");
#endif

    foreach(auto iface, QNetworkInterface::allInterfaces()) {
        qDebug(log) << iface;
        auto entry = iface.addressEntries();
        if(iface.flags() & QNetworkInterface::IsLoopBack)
            continue;
        if(!(iface.flags() & QNetworkInterface::CanBroadcast))
            continue;
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
    qDebug(log) << Q_FUNC_INFO;
    m_pWakeOnLan = qobject_cast<CParameterWakeOnLan*>(pParameter);
    if(!m_pWakeOnLan) return -1;

    ui->leIP->setText(m_pWakeOnLan->m_Net.GetHost());
    ui->gbWakeOnLan->setChecked(m_pWakeOnLan->GetEnable());
    ui->leMac->setText(m_pWakeOnLan->GetMac());
    ui->leBroadcastAddress->setText(m_pWakeOnLan->GetBroadcastAddress());
    ui->cbNetworkInterface->setCurrentText(m_pWakeOnLan->GetNetworkInterface());
    if(m_pWakeOnLan->GetBroadcastAddress().isEmpty()
        && ui->cbNetworkInterface->count() > 0)
        on_cbNetworkInterface_currentIndexChanged(
            ui->cbNetworkInterface->currentIndex());
    ui->sbPort->setValue(m_pWakeOnLan->GetPort());
    ui->lePassword->setText(m_pWakeOnLan->GetPassword());
    ui->pbSave->setChecked(m_pWakeOnLan->GetSavePassword());
    on_pbSave_clicked();
    ui->pbShow->setEnabled(m_pWakeOnLan->GetParameterClient()->GetViewPassowrd());
    ui->sbRepeat->setValue(m_pWakeOnLan->GetRepeat());
    ui->sbInterval->setValue(m_pWakeOnLan->GetInterval());
    ui->sbTimeOut->setValue(m_pWakeOnLan->GetTimeOut());
    return 0;
}

bool CParameterWakeOnLanUI::CheckValidity(bool validity)
{
    if(!validity) return true;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp rxMac(RegularMac);
    QRegExpValidator macValidator(rxMac, this);
    QRegExp rxIP(RegularIp);
    QRegExpValidator ipValidator(rxIP, this);
    QRegExp rxPassword(RegularPassword);
    QRegExpValidator passwordValidator(rxPassword, this);
#else
    QRegularExpression rxMac(RegularMac);
    QRegularExpressionValidator macValidator(rxMac, this);
    QRegularExpression rxIP(RegularIp);
    QRegularExpressionValidator ipValidator(rxIP, this);
    QRegularExpression rxPassword(RegularPassword);
    QRegularExpressionValidator passwordValidator(rxPassword, this);
#endif

    int pos = 0;
    QString szIp = ui->leIP->text();
    if(QValidator::Acceptable != ipValidator.validate(szIp, pos))
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("The ip address is error"));
        qCritical(log) << "The ip address is error";
        ui->leIP->setFocus();
        return false;
    }
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

    m_pWakeOnLan->m_Net.SetHost(ui->leIP->text());
    m_pWakeOnLan->SetEnable(ui->gbWakeOnLan->isChecked());
    m_pWakeOnLan->SetMac(ui->leMac->text());
    m_pWakeOnLan->SetNetworkInterface(ui->cbNetworkInterface->currentText());
    m_pWakeOnLan->SetBroadcastAddress(ui->leBroadcastAddress->text());
    m_pWakeOnLan->SetPort(ui->sbPort->value());
    m_pWakeOnLan->SetPassword(ui->lePassword->text());
    m_pWakeOnLan->SetSavePassword(ui->pbSave->isChecked());
    m_pWakeOnLan->SetRepeat(ui->sbRepeat->value());
    m_pWakeOnLan->SetInterval(ui->sbInterval->value());
    m_pWakeOnLan->SetTimeOut(ui->sbTimeOut->value());

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
    qDebug(log) << Q_FUNC_INFO << index;
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

void CParameterWakeOnLanUI::on_pbOK_clicked()
{
    if(!CheckValidity(false))
        return;
    int nRet = Accept();
    if(nRet) return;
    accept();
}

void CParameterWakeOnLanUI::on_pbCancel_clicked()
{
    reject();
}

void CParameterWakeOnLanUI::on_leIP_editingFinished()
{
    slotHostChanged(ui->leIP->text());
}
