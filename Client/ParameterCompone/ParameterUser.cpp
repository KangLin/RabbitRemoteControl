#include "ParameterUser.h"
#include "RabbitCommonTools.h"

CParameterUser::CParameterUser(CParameterConnecter *parent, const QString &szPrefix)
    : CParameterConnecter(parent, szPrefix),
    m_bSavePassword(false)
{
    SetUser(RabbitCommon::CTools::GetCurrentUser());
}

int CParameterUser::onLoad(QSettings &set)
{
    set.beginGroup("User");

    SetUser(set.value("Name", GetUser()).toString());

    //! [Initialize parameter]
    bool bDefaultSavePassword = GetSavePassword();
    if(GetParameterClient())
        bDefaultSavePassword = GetParameterClient()->GetSavePassword();
    SetSavePassword(set.value("SavePassword", bDefaultSavePassword).toBool());
    //! [Initialize parameter]
    if(GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Password"), "Password", szPassword, set))
            SetPassword(szPassword);
    }
    set.endGroup();
    return 0;
}

int CParameterUser::onSave(QSettings &set)
{
    set.beginGroup("User");
    set.setValue("Name", GetUser());
    SavePassword("Password", GetPassword(), set, GetSavePassword());
    set.endGroup();
    return 0;
}

const QString CParameterUser::GetUser() const
{
    return m_szUser;
}

void CParameterUser::SetUser(const QString &szUser)
{
    if (m_szUser == szUser)
        return;
    m_szUser = szUser;
    SetModified(true);
}

const QString CParameterUser::GetPassword() const
{
    return m_szPassword;
}

void CParameterUser::SetPassword(const QString &szPassword)
{
    if(m_szPassword == szPassword)
        return;
    m_szPassword = szPassword;
    SetModified(true);
}

const bool CParameterUser::GetSavePassword() const
{
    return m_bSavePassword;
}

void CParameterUser::SetSavePassword(bool save)
{
    if (m_bSavePassword == save)
        return;
    m_bSavePassword = save;
    SetModified(true);
}
