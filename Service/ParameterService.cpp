#include "ParameterService.h"

CParameterService::CParameterService(QObject *parent) : QObject(parent)
{
    m_nPort = 0;
}

int CParameterService::OnLoad(QDataStream &d)
{
    d >> m_nPort;
    return 0;
}

int CParameterService::OnSave(QDataStream &d)
{
    d << m_nPort;
    return 0;
}

quint16 CParameterService::getPort() const
{
    return m_nPort;
}

void CParameterService::setPort(quint16 newPort)
{
    if (m_nPort == newPort)
        return;
    m_nPort = newPort;
    emit PortChanged();
}
