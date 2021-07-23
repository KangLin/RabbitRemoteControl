#include "QSocket.h"
#include <QHostAddress>

CQSocket::CQSocket(): QObject()
{
}

char *CQSocket::getPeerAddress()
{
    char* pAddress = m_Socket.peerAddress().toString().toStdString().c_str();
    return pAddress;
}

char *CQSocket::getPeerEndpoint()
{
    QString szEndPoint;
    szEndPoint = m_Socket.peerAddress().toString();
    szEndPoint += "::";
    szEndPoint += QString::number(m_Socket.peerPort());
    return szEndPoint.toStdString().c_str();
}
