// Author: Kang Lin <kl222@126.com>
#include "ParameterPlugin.h"

CParameterPlugin::CParameterPlugin(QObject *parent)
    : CParameter(parent)
    , m_bNativeWindowReceiveKeyboard(false)
    , m_bEnableLocalInputMethod(false)
    , m_bPromptAdministratorPrivilege(true)
    , m_bEnableSystemUserToUser(true)
    , m_bSavePassword(false)
    , m_PromptType(PromptType::No)
    , m_nPromptCount(0)
    , m_bViewPassowrd(false)
    , m_bShowProtocolPrefix(false)
    , m_bShowIpPortInName(false)
    , m_AdaptWindows(CFrmViewer::ADAPT_WINDOWS::KeepAspectRationToWindow)
    , m_Record(this)
#if defined(HAVE_QTERMWIDGET)
    , m_Terminal(this)
#endif
{}

CParameterPlugin::~CParameterPlugin()
{}

int CParameterPlugin::OnLoad(QSettings &set)
{
    set.beginGroup("Client");
    SetNativeWindowReceiveKeyboard(
        set.value("NativeWindowReceiveKeyboard",
                  GetNativeWindowReceiveKeyboard()).toBool());
    SetEnableLocalInputMethod(set.value("InputMethod", GetEnableLocalInputMethod()).toBool());
    // Note: SetShowHookAdministratorPrivilege must precede SetHookKeyboard
    SetPromptAdministratorPrivilege(
        set.value("AdministratorPrivilege/Prompt",
                  GetPromptAdministratorPrivilege()).toBool());
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

int CParameterPlugin::OnSave(QSettings& set)
{
    set.beginGroup("Client");
    set.setValue("NativeWindowReceiveKeyboard",
                 GetNativeWindowReceiveKeyboard());
    set.setValue("InputMethod", GetEnableLocalInputMethod());
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

bool CParameterPlugin::GetNativeWindowReceiveKeyboard() const
{
    return m_bNativeWindowReceiveKeyboard;
}

void CParameterPlugin::SetNativeWindowReceiveKeyboard(bool newNativeWindowReceiveKeyboard)
{
    if(m_bNativeWindowReceiveKeyboard == newNativeWindowReceiveKeyboard)
        return;
    m_bNativeWindowReceiveKeyboard = newNativeWindowReceiveKeyboard;
    SetModified(true);
    emit sigNativeWindowReceiveKeyboard();
}

bool CParameterPlugin::GetEnableLocalInputMethod() const
{
    return m_bEnableLocalInputMethod;
}

void CParameterPlugin::SetEnableLocalInputMethod(bool enable)
{
    if(m_bEnableLocalInputMethod == enable)
        return;
    m_bEnableLocalInputMethod = enable;
    SetModified(true);
}

bool CParameterPlugin::GetPromptAdministratorPrivilege()
{
    return m_bPromptAdministratorPrivilege;
}

void CParameterPlugin::SetPromptAdministratorPrivilege(bool bShow)
{
    if(bShow == m_bPromptAdministratorPrivilege)
        return;
    SetModified(true);
    m_bPromptAdministratorPrivilege = bShow;
    emit sigPromptAdministratorPrivilege();
}

bool CParameterPlugin::GetEnableSystemUserToUser() const
{
    return m_bEnableSystemUserToUser;
}

void CParameterPlugin::SetEnableSystemUserToUser(bool enable)
{
    if(m_bEnableSystemUserToUser == enable)
        return;
    m_bEnableSystemUserToUser = enable;
    SetModified(true);
}

const QString &CParameterPlugin::GetEncryptKey() const
{
    return m_szEncryptKey;
}

void CParameterPlugin::SetEncryptKey(const QString &newPassword)
{
    if (m_szEncryptKey == newPassword)
        return;
    m_szEncryptKey = newPassword;
    SetModified(true);
    emit sigEncryptKeyChanged();
}

const bool &CParameterPlugin::GetSavePassword() const
{
    return m_bSavePassword;
}

void CParameterPlugin::SetSavePassword(bool NewAutoSavePassword)
{
    if (m_bSavePassword == NewAutoSavePassword)
        return;
    SetModified(true);
    m_bSavePassword = NewAutoSavePassword;
    emit sigSavePasswordChanged(m_bSavePassword);
}

CParameterPlugin::PromptType CParameterPlugin::GetPromptType() const
{
    return m_PromptType;
}

void CParameterPlugin::SetPromptType(PromptType NewPromptType)
{
    if (m_PromptType == NewPromptType)
        return;
    SetModified(true);
    m_PromptType = NewPromptType;
    emit sigPromptTypeChanged(m_PromptType);
}

int CParameterPlugin::GetPromptCount() const
{
    return m_nPromptCount;
}

void CParameterPlugin::SetPromptCount(int NewPromptCount)
{
    if (m_nPromptCount == NewPromptCount)
        return;
    m_nPromptCount = NewPromptCount;
    SetModified(true);
    emit sigPromptCountChanged(m_nPromptCount);
}

bool CParameterPlugin::GetViewPassowrd() const
{
    return m_bViewPassowrd;
}

void CParameterPlugin::SetViewPassowrd(bool NewViewPassowrd)
{
    if (m_bViewPassowrd == NewViewPassowrd)
        return;
    m_bViewPassowrd = NewViewPassowrd;
    SetModified(true);
    emit sigViewPassowrdChanged(m_bViewPassowrd);
}

bool CParameterPlugin::GetShowProtocolPrefix() const
{
    return m_bShowProtocolPrefix;
}

void CParameterPlugin::SetShowProtocolPrefix(bool bShowProtocolPrefix)
{
    if(m_bShowProtocolPrefix == bShowProtocolPrefix)
        return;
    m_bShowProtocolPrefix = bShowProtocolPrefix;
    SetModified(true);
    emit sigShowProtocolPrefixChanged();
}

bool CParameterPlugin::GetShowIpPortInName() const
{
    return m_bShowIpPortInName;
}

void CParameterPlugin::SetShowIpPortInName(bool bShowIpPortInName)
{
    if(m_bShowIpPortInName == bShowIpPortInName)
        return;
    m_bShowIpPortInName = bShowIpPortInName;
    SetModified(true);
    emit sigSHowIpPortInNameChanged();
}

CFrmViewer::ADAPT_WINDOWS CParameterPlugin::GetAdaptWindows()
{
    return m_AdaptWindows;
}

void CParameterPlugin::SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw)
{
    if(m_AdaptWindows == aw)
        return;
    m_AdaptWindows = aw;
    SetModified(true);
    emit sigAdaptWindowsChanged();
}
