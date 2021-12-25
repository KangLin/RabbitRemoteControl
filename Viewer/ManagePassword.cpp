// Author: Kang Lin <kl222@126.com>

#include "ManagePassword.h"

CManagePassword::CManagePassword(QObject *parent) : QObject(parent),
    m_bSavePassword(false)
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

const bool &CManagePassword::GetSavePassword() const
{
    return m_bSavePassword;
}

void CManagePassword::SetSavePassword(bool NewAutoSavePassword)
{
    if (m_bSavePassword == NewAutoSavePassword)
        return;
    m_bSavePassword = NewAutoSavePassword;
    emit sigSavePasswordChanged(m_bSavePassword);
}
