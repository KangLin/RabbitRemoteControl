// Author: Kang Lin <kl222@126.com>

#include "ManagePassword.h"

CManagePassword::CManagePassword(QObject *parent) : QObject(parent),
    m_bSavePassword(false),
    m_PromptType(PromptType::First),
    m_nPromptCount(0)
{}

CManagePassword* CManagePassword::Instance()
{
    static CManagePassword* p = nullptr;
    if(!p)
        p = new CManagePassword();
    return p;
}

int CManagePassword::Load(QSettings &set)
{
    SetPromptType(static_cast<PromptType>(
                    set.value("Manage/Password/Prompty/Type",
                              static_cast<int>(GetPromptType())).toInt()
                              ));
    SetSavePassword(set.value("Manage/Password/Save", GetSavePassword()).toBool());
    return 0;
}

int CManagePassword::Save(QSettings &set)
{
    set.setValue("Manage/Password/Prompty/Type",
                 static_cast<int>(GetPromptType()));
    set.setValue("Manage/Password/Save", GetSavePassword());
    return 0;
}

const QString &CManagePassword::GetEncryptKey() const
{
    return m_szEncryptKey;
}

void CManagePassword::SetEncryptKey(const QString &newPassword)
{
    if (m_szEncryptKey == newPassword)
        return;
    m_szEncryptKey = newPassword;
    emit sigEncryptKeyChanged();
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

CManagePassword::PromptType CManagePassword::GetPromptType() const
{
    return m_PromptType;
}

void CManagePassword::SetPromptType(PromptType NewPromptType)
{
    if (m_PromptType == NewPromptType)
        return;
    m_PromptType = NewPromptType;
    emit sigPromptTypeChanged(m_PromptType);
}

int CManagePassword::GetPromptCount() const
{
    return m_nPromptCount;
}

void CManagePassword::SetPromptCount(int NewPromptCount)
{
    if (m_nPromptCount == NewPromptCount)
        return;
    m_nPromptCount = NewPromptCount;
    emit sigPromptCountChanged(m_nPromptCount);
}
