// Author: Kang Lin <kl222@126.com>

#include "ParameterService.h"
#include <QSettings>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Service)
CParameterService::CParameterService(QObject *parent) : QObject(parent),
    m_nPort(0),
    m_bEnable(true)
{
}

CParameterService::~CParameterService()
{
    qDebug(Service) << "CParameterService::~CParameterService";
}

int CParameterService::Load(const QString& szFile)
{
    QSettings set(szFile, QSettings::IniFormat);
    m_nPort = set.value("Port", m_nPort).toUInt();
    m_bEnable = set.value("Enable", m_bEnable).toBool();
    m_szPassword = set.value("Password", m_szPassword).toString();
    return 0;
}

int CParameterService::Save(const QString& szFile)
{
    QSettings set(szFile, QSettings::IniFormat);
    set.setValue("Port", m_nPort);
    set.setValue("Enable", m_bEnable);
    set.setValue("Password", m_szPassword);
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

const QString &CParameterService::getPassword() const
{
    return m_szPassword;
}

void CParameterService::setPassword(const QString &newPassword)
{
    if (m_szPassword == newPassword)
        return;
    m_szPassword = newPassword;
    emit PasswordChanged();
}
