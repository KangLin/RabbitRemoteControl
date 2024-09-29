// Author: Kang Lin <kl222@126.com>

#ifndef WAKEONLANQT_H
#define WAKEONLANQT_H

#include "WakeOnLan.h"
#include <QHostAddress>
#include "client_export.h"

class CLIENT_EXPORT CWakeOnLanQt : public QObject, protected CWakeOnLan
{
    Q_OBJECT
public:
    CWakeOnLanQt(const QString& szBroadcastAddress = "255.255.255.255",
                 QObject *parent=nullptr);

    void SetBroadcastAddress(const QString& szBroadcastAddress = "255.255.255.255");
    QString SetBroadcastAddress(const QString& szIP, const QString& szMask);

    /*!
     * \brief SendMagicPacket
     * \param szMac: the MAC address of wake on lan device.
     *          the format: Hexadecimal characters. eg: FF:FF:FF:FF:FF:FF
     * \param portNum: 9 or 7. the default is 9.
     * \return
     */
    bool SendMagicPacket(const QString& szMac, uint16_t portNum = 9);
    /*!
     * \brief SendSecureMagicPacket
     * \param szMac: the MAC address of wake on lan device.
     *          the format: Hexadecimal characters. eg: FF:FF:FF:FF:FF:FF
     * \param szPassword: A password string of length 6.
     *          or Hexadecimal representation of 6 bytes
     * \param portNum: default is 9. 9 or 7
     * \return
     */
    bool SendSecureMagicPacket(const QString& szMac,
                               const QString& szPassword, uint16_t portNum = 9);

protected:
    virtual int SendPacket(const char *data, int64_t size,
                           std::string addr, uint16_t port) override;
};

#endif // WAKEONLANQT_H
