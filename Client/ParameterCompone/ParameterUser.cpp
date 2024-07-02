#include "ParameterUser.h"
#include "RabbitCommonTools.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Parameter.User")

CParameterUser::CParameterUser(CParameterConnecter *parent, const QString &szPrefix)
    : CParameterConnecter(parent, szPrefix),
    m_bSavePassword(false)
{
    SetUser(RabbitCommon::CTools::GetCurrentUser());
}

int CParameterUser::OnLoad(QSettings &set)
{
    set.beginGroup("User");

    SetUser(set.value("Name", GetUser()).toString());

    SetSavePassword(set.value("SavePassword", GetSavePassword()).toBool());

    if(GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Password"), "Password", szPassword, set))
            SetPassword(szPassword);
    }
    set.endGroup();
    return 0;
}

int CParameterUser::OnSave(QSettings &set)
{
    set.beginGroup("User");
    set.setValue("Name", GetUser());
    set.setValue("SavePassword", GetSavePassword());
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

//! [Initialize parameter after set CParameterClient]
void CParameterUser::slotSetParameterClient()
{
    if(!GetParameterClient()) {
        QString szErr = "The CParameterClient is null";
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "CParameterUser", szErr.toStdString().c_str());
        return;
    }

    if(GetParameterClient())
        SetSavePassword(GetParameterClient()->GetSavePassword());

    return;
}
//! [Initialize parameter after set CParameterClient]
