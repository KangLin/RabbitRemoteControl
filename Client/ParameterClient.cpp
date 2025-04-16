#include "ParameterClient.h"

CParameterClient::CParameterClient(QObject *parent)
    : CParameter(parent)
    , m_bNativeWindowReceiveKeyboard(false)
    , m_bHookKeyboard(false)
    , m_bPromptAdministratorPrivilege(true)
    , m_bEnableSystemUserToUser(true)
    , m_bSavePassword(false)
    , m_PromptType(PromptType::No)
    , m_nPromptCount(0)
    , m_bViewPassowrd(false)
    , m_bShowProtocolPrefix(false)
    , m_bShowIpPortInName(false)
    , m_AdaptWindows(CFrmViewer::ADAPT_WINDOWS::ZoomToWindow)
    , m_Record(this)
{}

CParameterClient::~CParameterClient()
{}

int CParameterClient::OnLoad(QSettings &set)
{
    set.beginGroup("Client");
    SetNativeWindowReceiveKeyboard(
        set.value("NativeWindowRecieveKeyboard",
                  GetNativeWindowReceiveKeyboard()).toBool());
    // Note: SetShowHookAdministratorPrivilege must precede SetHookKeyboard
    SetPromptAdministratorPrivilege(
        set.value("AdministratorPrivilege/Prompt",
                  GetPromptAdministratorPrivilege()).toBool());
    SetHookKeyboard(set.value("Hook/Keyboard",
                              GetHookKeyboard()).toBool());
    SetEnableSystemUserToUser(set.value("UserName/Enable",
                                GetEnableSystemUserToUser()).toBool());
    SetPromptType(static_cast<PromptType>(
                    set.value("Password/Prompty/Type",
                              static_cast<int>(GetPromptType())).toInt()
                              ));
    SetSavePassword(set.value("Password/Save", GetSavePassword()).toBool());
    SetViewPassowrd(set.value("Password/View", GetViewPassowrd()).toBool());
    SetShowProtocolPrefix(set.value("Connecter/Name/ShowProtocolPrefix", GetShowProtocolPrefix()).toBool());
    SetShowIpPortInName(set.value("Connecter/Name/ShowIpPort", GetShowIpPortInName()).toBool());
    SetAdaptWindows((CFrmViewer::ADAPT_WINDOWS)set.value("Viewer/AdaptWindows",
                                         (int)GetAdaptWindows()).toInt());
    set.endGroup();
    return 0;
}

int CParameterClient::OnSave(QSettings& set)
{
    set.beginGroup("Client");
    set.setValue("NativeWindowRecieveKeyboard",
                 GetNativeWindowReceiveKeyboard());
    set.setValue("Hook/Keyboard", GetHookKeyboard());
    set.setValue("AdministratorPrivilege/Prompt", GetPromptAdministratorPrivilege());
    set.setValue("UserName/Enable", GetEnableSystemUserToUser());
    set.setValue("Password/Prompty/Type",
                 static_cast<int>(GetPromptType()));
    set.setValue("Password/Save", GetSavePassword());
    set.setValue("Password/View", GetViewPassowrd());
    set.setValue("Connecter/Name/ShowProtocolPrefix", GetShowProtocolPrefix());
    set.setValue("Connecter/Name/ShowIpPort", GetShowIpPortInName());
    set.setValue("Viewer/AdaptWindows", (int)GetAdaptWindows());
    set.endGroup();
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

bool CParameterClient::GetNativeWindowReceiveKeyboard() const
{
    return m_bNativeWindowReceiveKeyboard;
}

void CParameterClient::SetNativeWindowReceiveKeyboard(bool newNativeWindowRecieveKeyboard)
{
    if(m_bNativeWindowReceiveKeyboard == newNativeWindowRecieveKeyboard)
        return;
    m_bNativeWindowReceiveKeyboard = newNativeWindowRecieveKeyboard;
    SetModified(true);
    emit sigNativeWindowRecieveKeyboard();
}

bool CParameterClient::GetPromptAdministratorPrivilege()
{
    return m_bPromptAdministratorPrivilege;
}

void CParameterClient::SetPromptAdministratorPrivilege(bool bShow)
{
    if(bShow == m_bPromptAdministratorPrivilege)
        return;
    SetModified(true);
    m_bPromptAdministratorPrivilege = bShow;
    emit sigPromptAdministratorPrivilege();
}

bool CParameterClient::GetEnableSystemUserToUser() const
{
    return m_bEnableSystemUserToUser;
}

void CParameterClient::SetEnableSystemUserToUser(bool enable)
{
    if(m_bEnableSystemUserToUser == enable)
        return;
    m_bEnableSystemUserToUser = enable;
    SetModified(true);
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
    SetModified(true);
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
    SetModified(true);
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

CFrmViewer::ADAPT_WINDOWS CParameterClient::GetAdaptWindows()
{
    return m_AdaptWindows;
}

void CParameterClient::SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw)
{
    if(m_AdaptWindows == aw)
        return;
    m_AdaptWindows = aw;
    SetModified(true);
    emit sigAdaptWindowsChanged();
}
