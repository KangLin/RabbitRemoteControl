// Author: Kang Lin <kl222@126.com>

#include <string.h>
#include "WakeOnLan.h"

#ifdef WIN32
    #include <winsock2.h>
    typedef unsigned long in_addr_t;
#else
    #include <arpa/inet.h>
#endif

CWakeOnLan::CWakeOnLan(const std::string &szBroadcastAddress) {
    SetBroadcastAddress(szBroadcastAddress);
}

void CWakeOnLan::SetBroadcastAddress(const std::string &szBroadcastAddress) {
	m_szBroadcastAddress = szBroadcastAddress;
}

std::string CWakeOnLan::SetBroadcastAddress(
    const std::string &szIP, const std::string &szMask) {
    in_addr_t ip = inet_addr(szIP.c_str());
    in_addr_t mask = inet_addr(szMask.c_str());
    in_addr_t broadcast = 0;
    char* pIP = (char*)&ip;
    char* pMask = (char*)&mask;
    char* pBroadcast = (char*)&broadcast;
    for (int i = 0; i < 4; i++)
        pBroadcast[i] = ~ pMask[i] | pIP[i];
    std::string szBroadcast = inet_ntoa((struct in_addr&)broadcast);
    if(!szBroadcast.empty())
        SetBroadcastAddress(szBroadcast);
    return szBroadcast;
}

bool CWakeOnLan::StringToArray(
    /*[out]*/uint8_t* mac, /*[in]*/const char* szMac) {
	char macFormat[23 + 1];
	unsigned int tempMACAddress[6];

	if (strlen(szMac) == 12)  // FFFFFFFFFFFF
		sprintf(macFormat, "%%2x%%2x%%2x%%2x%%2x%%2x");
	else if (strlen(szMac) == 14)  // FFFF-FFFF-FFFF
		sprintf(macFormat, "%%2x%%2x%c%%2x%%2x%c%%2x%%2x", szMac[4], szMac[9]);
	else if (strlen(szMac) == 17)  // FF-FF-FF-FF-FF-FF
		sprintf(macFormat, "%%2x%c%%2x%c%%2x%c%%2x%c%%2x%c%%2x",
                szMac[2], szMac[5], szMac[8], szMac[11], szMac[14]);
	else
		return false;

	int j = sscanf(szMac, (const char*)macFormat, &tempMACAddress[0],
                   &tempMACAddress[1], &tempMACAddress[2], &tempMACAddress[3],
                   &tempMACAddress[4], &tempMACAddress[5]);
	if (j == 6) {
		for (uint8_t i = 0;
             i < sizeof(tempMACAddress) / sizeof(*tempMACAddress); i++)
			mac[i] = (uint8_t)tempMACAddress[i];

		return true;
	}

	return false;
}

bool CWakeOnLan::SendMagicPacket(const std::string &szMac, uint16_t portNum) {
	return SendMagicPacket(szMac.c_str(), portNum);
}

bool CWakeOnLan::SendSecureMagicPacket(
    const std::string &szMac,
    const std::string szPassword,
    uint16_t portNum) {
	return SendSecureMagicPacket(szMac.c_str(), szPassword.c_str(), portNum);
}

bool CWakeOnLan::SendMagicPacket(const char* pszMacAddress, uint16_t portNum) {
	uint8_t macAddress[6];

	bool res = StringToArray(macAddress, pszMacAddress);
	if (!res)
		return false;

	return SendMagicPacket(macAddress, sizeof(macAddress), portNum);
}

bool CWakeOnLan::SendSecureMagicPacket(
    const char* pszMacAddress,
    const char* pszPassword,
    uint16_t portNum) {

	uint8_t macAddress[6];
	uint8_t secureOn[6];

	bool res = StringToArray(macAddress, pszMacAddress);
	if (!res)
		return false;

	bool res2 = StringToArray(secureOn, pszPassword);
	if (!res2)
		return false;

	return SendSecureMagicPacket(macAddress, sizeof(macAddress),
                                 secureOn, sizeof(secureOn), portNum);
}

bool CWakeOnLan::SendMagicPacket(uint8_t* pMacAddress,
                                 size_t sizeOfMacAddress, uint16_t portNum) {
	size_t magicPacketSize = 6 + (6 * 16);  // FF*6 + MAC*16
	uint8_t* magicPacket = new uint8_t[magicPacketSize];

	GenerateMagicPacket(magicPacket, magicPacketSize,
                        pMacAddress, sizeOfMacAddress);

    SendPacket((const char*)magicPacket, magicPacketSize,
               m_szBroadcastAddress, portNum);

	delete[] magicPacket;

    return true;
}

bool CWakeOnLan::SendSecureMagicPacket(uint8_t* pMacAddress,
                                       size_t sizeOfMacAddress,
                                       uint8_t* pszPassword,
                                       size_t nPasswordSize,
                                       uint16_t portNum) {
	size_t magicPacketSize = 6 + (6 * 16) + 6;  // FF*6 + MAC*16 + SecureOn
	uint8_t* magicPacket = new uint8_t[magicPacketSize];

	GenerateSecureMagicPacket(magicPacket, magicPacketSize, pMacAddress,
                              sizeOfMacAddress, pszPassword, nPasswordSize);

    SendPacket((const char*)magicPacket, magicPacketSize,
               m_szBroadcastAddress, portNum);

	delete[] magicPacket;

    return true;
}

void CWakeOnLan::GenerateMagicPacket(uint8_t*& pMagicPacket,
                                     size_t& sizeOfMagicPacket,
                                     uint8_t* pMacAddress,
                                     size_t sizeOfMacAddress) {
	uint8_t macAddress[6];

	memcpy(macAddress, pMacAddress, sizeOfMacAddress);
	memset(pMagicPacket, 0xFF, 6);

	for (uint8_t i = 0; i < 16; i++) {
		uint8_t index = (i + 1) * sizeOfMacAddress;
		memcpy(&pMagicPacket[index], &macAddress, sizeOfMacAddress);
	}
}

void CWakeOnLan::GenerateSecureMagicPacket(uint8_t*& pMagicPacket,
                                           size_t& sizeOfMagicPacket,
                                           uint8_t* pMacAddress,
                                           size_t sizeOfMacAddress,
                                           uint8_t* pSecureOn,
                                           size_t sizeOfSecureOn) {
	uint8_t macAddress[6];
	uint8_t secureOn[6];

	memcpy(macAddress, pMacAddress, sizeOfMacAddress);
	memcpy(secureOn, pSecureOn, sizeOfSecureOn);

	memset(pMagicPacket, 0xFF, 6);

	for (uint8_t i = 0; i < 16; i++) {
		uint8_t index = (i + 1) * sizeOfMacAddress;
		memcpy(&pMagicPacket[index], &macAddress, sizeOfMacAddress);
	}

	memcpy(&pMagicPacket[17 * sizeOfSecureOn], &secureOn, sizeOfSecureOn);
}
