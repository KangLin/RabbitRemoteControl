// Author: Kang Lin <kl222@126.com>

#include "WakeOnLanQt.h"

#include <QLoggingCategory>
#include <QUdpSocket>

static Q_LOGGING_CATEGORY(log, "WOL")
CWakeOnLanQt::CWakeOnLanQt(const QString &szBroadcastAddress, QObject *parent)
    : QObject(parent), CWakeOnLan(szBroadcastAddress.toStdString())
{}

void CWakeOnLanQt::SetBroadcastAddress(const QString &szBroadcastAddress)
{
    CWakeOnLan::SetBroadcastAddress(szBroadcastAddress.toStdString());
}

void CWakeOnLanQt::SetBroadcastAddress(const QString &szIP, const QString &szMask)
{
    CWakeOnLan::SetBroadcastAddress(szIP.toStdString(), szMask.toStdString());
}

bool CWakeOnLanQt::SendMagicPacket(const QString &szMac, uint16_t portNum) {
    return CWakeOnLan::SendMagicPacket(szMac.toStdString(), portNum);
}

bool CWakeOnLanQt::SendSecureMagicPacket(const QString &szMac,
                                         const QString &szPassword,
                                         uint16_t portNum) {
    if(szPassword.length() < 6) {
        qCritical(log) << "The password must be 6. current length:"
                       << szPassword.length();
        return false;
    }
    // character
    if(szPassword.length() == 6) {
        QByteArray data = szPassword.toLocal8Bit().toHex(':');
        return SendSecureMagicPacket(szMac.toStdString().c_str(),
                                     data.data(), portNum);
    }
    // Hexadecimal characters
    return CWakeOnLan::SendSecureMagicPacket(szMac.toStdString(),
                                 szPassword.toStdString(), portNum);
}

int CWakeOnLanQt::SendPacket(const char *data, int64_t size,
                             std::string addr, uint16_t port)
{
    // Send the packet via UDP
    QUdpSocket udpSocket;
    int nRet = udpSocket.writeDatagram(data, size,
                                       QHostAddress(addr.c_str()), port);
    return nRet;
}
