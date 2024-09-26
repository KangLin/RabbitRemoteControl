// Author: Kang Lin <kl222@126.com>

#ifndef __WAKE_ON_LAN_H_2024_09_25__
#define __WAKE_ON_LAN_H_2024_09_25__

#pragma once

#include <stdint.h>
#include <string>

class CWakeOnLan {
   public:
    CWakeOnLan(const std::string& szBroadcastAddress = "255.255.255.255");

	void SetBroadcastAddress(const std::string& szBroadcastAddress = "255.255.255.255");
    void SetBroadcastAddress(const std::string& szIP, const std::string& szMask);

	bool StringToArray(/*[out]*/uint8_t* mac, /*[in]*/const char* szMac);

    /*!
     * \brief SendMagicPacket
     * \param szMac: the MAC address of wake on lan device.
     *          the format: Hexadecimal characters. eg: FF:FF:FF:FF:FF:FF
     * \param portNum: 9 or 7. the default is 9.
     * \return
     */
	bool SendMagicPacket(const std::string& szMac, uint16_t portNum = 9);

    /*!
	 * \brief SendSecureMagicPacket
	 * \param szMac: the MAC address of wake on lan device.
     *          the format: Hexadecimal characters. eg: FF:FF:FF:FF:FF:FF
	 * \param szPassword: Hexadecimal characters of password
	 * \param portNum
	 * \return
	 */
	bool SendSecureMagicPacket(
        const std::string& szMac,
        const std::string szPassword,
        uint16_t portNum = 9);

	bool SendMagicPacket(const char* pszMacAddress, uint16_t portNum = 9);
	bool SendSecureMagicPacket(
        const char* pszMacAddress,
        const char* pszPassword,
        uint16_t portNum = 9);

	bool SendMagicPacket(uint8_t* pMacAddress,
                         size_t sizeOfMacAddress,
                         uint16_t portNum = 9);
	bool SendSecureMagicPacket(uint8_t* pMacAddress,
                               size_t sizeOfMacAddress,
                               uint8_t* pszPassword,
                               size_t nPasswordSize,
                               uint16_t portNum = 9);

	void GenerateMagicPacket(uint8_t*& pMagicPacket, size_t& sizeOfMagicPacket,
                             uint8_t* pMacAddress, size_t sizeOfMacAddress);
	void GenerateSecureMagicPacket(uint8_t*& pMagicPacket,
                                   size_t& sizeOfMagicPacket,
                                   uint8_t* pMacAddress,
                                   size_t sizeOfMacAddress,
                                   uint8_t* pSecureOn,
                                   size_t sizeOfSecureOn);

protected:
    virtual int SendPacket(const char* data, int64_t size, std::string addr, uint16_t port) = 0;

private:
    std::string m_szBroadcastAddress;
};

#endif // __WAKE_ON_LAN_H_2024_09_25__
