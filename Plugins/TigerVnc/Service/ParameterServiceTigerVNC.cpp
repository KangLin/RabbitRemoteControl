#include "ParameterServiceTigerVNC.h"

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
