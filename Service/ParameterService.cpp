#include "ParameterService.h"
#include <QSettings>

CParameterService::CParameterService(QObject *parent) : QObject(parent),
    m_nPort(0),
    m_bEnable(true)
{}

int CParameterService::OnLoad(QDataStream &d)
{
    d >> m_nPort;
    d >> m_bEnable;
    return 0;
}

int CParameterService::OnSave(QDataStream &d)
{
    d << m_nPort;
    d << m_bEnable;
    return 0;
}

int CParameterService::OnLoad(const QString& szFile)
{
    QSettings set(szFile, QSettings::IniFormat);
    m_nPort = set.value("Port", m_nPort).toUInt();
    m_bEnable = set.value("Enable", m_bEnable).toBool();
    return 0;
}

int CParameterService::OnSave(const QString& szFile)
{
    QSettings set(szFile, QSettings::IniFormat);
    set.setValue("Port", m_nPort);
    set.setValue("Enable", m_bEnable);
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

bool CParameterService::getEnable() const
{
    return m_bEnable;
}

void CParameterService::setEnable(bool newEnable)
{
    if (m_bEnable == newEnable)
        return;
    m_bEnable = newEnable;
    emit EnableChanged();
}
