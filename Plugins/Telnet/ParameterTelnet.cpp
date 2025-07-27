#include "ParameterTelnet.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Plugin.Telnet.Parameter")

CParameterTelnet::CParameterTelnet(CParameterOperate *parent, const QString &szPrefix)
    : CParameterTerminalBase{parent}
    , m_Net(this)
    , m_szLogin(".*login:")
    , m_szPassword(".*Password:")
{}

CParameterTelnet::~CParameterTelnet()
{}


int CParameterTelnet::OnLoad(QSettings &set)
{
    set.setValue("Login", GetLogin());
    set.setValue("Password", GetPassword());
    return CParameterTerminalBase::OnLoad(set);
}

int CParameterTelnet::OnSave(QSettings &set)
{
    SetLogin(set.value("Login", GetLogin()).toString());
    SetPassword(set.value("Password", GetPassword()).toString());
    return CParameterTerminalBase::OnSave(set);
}

QString CParameterTelnet::GetLogin() const
{
    return m_szLogin;
}

void CParameterTelnet::SetLogin(const QString &newLogin)
{
    if(m_szLogin == newLogin)
        return;
    m_szLogin = newLogin;
    SetModified(true);
}

QString CParameterTelnet::GetPassword() const
{
    return m_szPassword;
}

void CParameterTelnet::SetPassword(const QString &newPassword)
{
    if(m_szPassword == newPassword)
        return;
    m_szPassword = newPassword;
    SetModified(true);
}
