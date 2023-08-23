#include "ParameterClient.h"

CParameterClient::CParameterClient(QObject *parent)
    : CParameter(parent),
      m_bHookKeyboard(true),
      m_bSavePassword(false),
      m_PromptType(PromptType::No),
      m_nPromptCount(0),
      m_bViewPassowrd(false),
      m_bShowProtocolPrefix(false),
      m_bShowIpPortInName(false)
{}

CParameterClient::~CParameterClient()
{}

int CParameterClient::Load(QSettings &set)
{
    SetHookKeyboard(set.value("Client/Hook/Keyboard",
                              GetHookKeyboard()).toBool());
    SetPromptType(static_cast<PromptType>(
                    set.value("Client/Password/Prompty/Type",
                              static_cast<int>(GetPromptType())).toInt()
                              ));
    SetSavePassword(set.value("Client/Password/Save", GetSavePassword()).toBool());
    SetViewPassowrd(set.value("Client/Password/View", GetViewPassowrd()).toBool());
    SetShowProtocolPrefix(set.value("Client/Connecter/Name/ShowProtocolPrefix", GetShowProtocolPrefix()).toBool());
    SetShowIpPortInName(set.value("Client/Connecter/Name/ShowIpPort", GetShowIpPortInName()).toBool());
    return 0;
}

int CParameterClient::Save(QSettings& set)
{
    set.setValue("Client/Hook/Keyboard", GetHookKeyboard());
    set.setValue("Client/Password/Prompty/Type",
                 static_cast<int>(GetPromptType()));
    set.setValue("Client/Password/Save", GetSavePassword());
    set.setValue("Client/Password/View", GetViewPassowrd());
    set.setValue("Client/Connecter/Name/ShowProtocolPrefix", GetShowProtocolPrefix());
    set.setValue("Client/Connecter/Name/ShowIpPort", GetShowIpPortInName());
    return 0;
}

bool CParameterClient::GetHookKeyboard() const
{
    return m_bHookKeyboard;
}

void CParameterClient::SetHookKeyboard(bool newHookKeyboard)
{
    if (m_bHookKeyboard == newHookKeyboard)
        return;
    SetModified(true);
    m_bHookKeyboard = newHookKeyboard;
    emit sigHookKeyboardChanged();
}

const QString &CParameterClient::GetEncryptKey() const
{
    return m_szEncryptKey;
}

void CParameterClient::SetEncryptKey(const QString &newPassword)
{
    if (m_szEncryptKey == newPassword)
        return;
    m_szEncryptKey = newPassword;
    emit sigEncryptKeyChanged();
}

const bool &CParameterClient::GetSavePassword() const
{
    return m_bSavePassword;
}

void CParameterClient::SetSavePassword(bool NewAutoSavePassword)
{
    if (m_bSavePassword == NewAutoSavePassword)
        return;
    SetModified(true);
    m_bSavePassword = NewAutoSavePassword;
    emit sigSavePasswordChanged(m_bSavePassword);
}

CParameterClient::PromptType CParameterClient::GetPromptType() const
{
    return m_PromptType;
}

void CParameterClient::SetPromptType(PromptType NewPromptType)
{
    if (m_PromptType == NewPromptType)
        return;
    SetModified(true);
    m_PromptType = NewPromptType;
    emit sigPromptTypeChanged(m_PromptType);
}

int CParameterClient::GetPromptCount() const
{
    return m_nPromptCount;
}

void CParameterClient::SetPromptCount(int NewPromptCount)
{
    if (m_nPromptCount == NewPromptCount)
        return;
    m_nPromptCount = NewPromptCount;
    emit sigPromptCountChanged(m_nPromptCount);
}

bool CParameterClient::GetViewPassowrd() const
{
    return m_bViewPassowrd;
}

void CParameterClient::SetViewPassowrd(bool NewViewPassowrd)
{
    if (m_bViewPassowrd == NewViewPassowrd)
        return;
    m_bViewPassowrd = NewViewPassowrd;
    SetModified(true);
    emit sigViewPassowrdChanged(m_bViewPassowrd);
}

bool CParameterClient::GetShowProtocolPrefix() const
{
    return m_bShowProtocolPrefix;
}

void CParameterClient::SetShowProtocolPrefix(bool bShowProtocolPrefix)
{
    if(m_bShowProtocolPrefix == bShowProtocolPrefix)
        return;
    m_bShowProtocolPrefix = bShowProtocolPrefix;
    SetModified(true);
    emit sigShowProtocolPrefixChanged();
}

bool CParameterClient::GetShowIpPortInName() const
{
    return m_bShowIpPortInName;
}

void CParameterClient::SetShowIpPortInName(bool bShowIpPortInName)
{
    if(m_bShowIpPortInName == bShowIpPortInName)
        return;
    m_bShowIpPortInName = bShowIpPortInName;
    SetModified(true);
    emit sigSHowIpPortInNameChanged();
}
