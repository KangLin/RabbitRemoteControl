#include "ParameterServiceTigerVNC.h"
#include <QSettings>

CParameterServiceTigerVNC::CParameterServiceTigerVNC(QObject *parent)
    : CParameterService(parent)
{
    setPort(5900);
}

int CParameterServiceTigerVNC::OnLoad(QDataStream &d)
{
    int nRet = CParameterService::OnLoad(d);
    d >> m_szPassword;
    return nRet;
}

int CParameterServiceTigerVNC::OnSave(QDataStream &d)
{
    int nRet = CParameterService::OnSave(d);
    d << m_szPassword;
    return nRet;
}

int CParameterServiceTigerVNC::OnLoad(const QString& szFile)
{
    int nRet = CParameterService::OnLoad(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    set.setValue("TigerVNC/Password", m_szPassword);
    return nRet;
}

int CParameterServiceTigerVNC::OnSave(const QString& szFile)
{
    int nRet = CParameterService::OnSave(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    m_szPassword = set.value("TigerVNC/Password", m_szPassword).toString();
    return nRet;
}

const QString &CParameterServiceTigerVNC::getPassword() const
{
    return m_szPassword;
}

void CParameterServiceTigerVNC::setPassword(const QString &newPassword)
{
    if (m_szPassword == newPassword)
        return;
    m_szPassword = newPassword;
    emit PasswordChanged();
}
