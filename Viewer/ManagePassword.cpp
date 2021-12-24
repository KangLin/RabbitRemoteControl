// Author: Kang Lin <kl222@126.com>

#include "ManagePassword.h"

CManagePassword::CManagePassword(QObject *parent) : QObject(parent)
{}

CManagePassword* CManagePassword::Instance()
{
    static CManagePassword* p = nullptr;
    if(!p)
        p = new CManagePassword();
    return p;
}

const QString &CManagePassword::GetPassword() const
{
    return m_szPassword;
}

void CManagePassword::SetPassword(const QString &newPassword)
{
    if (m_szPassword == newPassword)
        return;
    m_szPassword = newPassword;
    emit PasswordChanged();
}
