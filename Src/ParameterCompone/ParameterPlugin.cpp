// Author: Kang Lin <kl222@126.com>

#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#include "ParameterPlugin.h"

CParameterPlugin::CParameterPlugin(QObject *parent)
    : CParameter(parent)
    , m_bCaptureAllKeyboard(true)
    , m_bDesktopShortcutsScript(false)
    , m_bEnableLocalInputMethod(false)
    , m_bPromptAdministratorPrivilege(!RabbitCommon::CTools::Instance()->HasAdministratorPrivilege())
    , m_bEnableSystemUserToUser(true)
    , m_bSavePassword(false)
    , m_PromptType(PromptType::First)
    , m_nPromptCount(0)
    , m_bViewPassowrd(false)
    , m_bUseSystemCredential(true)
    , m_bShowProtocolPrefix(false)
    , m_bShowIpPortInName(false)
    , m_AdaptWindows(CFrmViewer::ADAPT_WINDOWS::KeepAspectRationToWindow)
    , m_szPluginsPath(RabbitCommon::CDir::Instance()->GetDirPlugins())
    , m_WhiteList(this, "Whilelist")
    , m_BlackList(this, "BlackList")
    , m_Record(this)
    , m_MediaDevices(this)
#if defined(HAVE_QTERMWIDGET)
    , m_Terminal(this)
#endif
{}

CParameterPlugin::~CParameterPlugin()
{}

int CParameterPlugin::OnLoad(QSettings &set)
{
    set.beginGroup("Client");
    SetCaptureAllKeyboard(
        set.value("CaptureAllKeyboard", GetCaptureAllKeyboard()).toBool());
    SetDesktopShortcutsScript(set.value("DesktopShortcutsScript/Enable",
                                        GetDesktopShortcutsScript()).toBool());
    SetDisableDesktopShortcutsScript(
        set.value("DesktopShortcutsScript/Disable",
                  GetDisableDesktopShortcutsScript()).toString());
    SetRestoreDesktopShortcutsScript(
        set.value("DesktopShortcutsScript/Restore",
                  GetRestoreDesktopShortcutsScript()).toString());
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
    SetUseSystemCredential(set.value("Password/UseSystemCredential", GetUseSystemCredential()).toBool());
    SetShowProtocolPrefix(set.value("Connecter/Name/ShowProtocolPrefix", GetShowProtocolPrefix()).toBool());
    SetShowIpPortInName(set.value("Connecter/Name/ShowIpPort", GetShowIpPortInName()).toBool());
    SetAdaptWindows((CFrmViewer::ADAPT_WINDOWS)set.value("Viewer/AdaptWindows",
                                         (int)GetAdaptWindows()).toInt());
    SetPluginsPath(set.value("PluginsPath", GetPluginsPath()).toStringList());
    set.endGroup();
    return 0;
}

int CParameterPlugin::OnSave(QSettings& set)
{
    set.beginGroup("Client");
    set.setValue("CaptureAllKeyboard", GetCaptureAllKeyboard());
    set.setValue("DesktopShortcutsScript/Enable", GetDesktopShortcutsScript());
    set.setValue("DesktopShortcutsScript/Disable", GetDisableDesktopShortcutsScript());
    set.setValue("DesktopShortcutsScript/Restore", GetRestoreDesktopShortcutsScript());
    set.setValue("InputMethod", GetEnableLocalInputMethod());
    set.setValue("AdministratorPrivilege/Prompt", GetPromptAdministratorPrivilege());
    set.setValue("UserName/Enable", GetEnableSystemUserToUser());
    set.setValue("Password/Prompty/Type", static_cast<int>(GetPromptType()));
    set.setValue("Password/Save", GetSavePassword());
    set.setValue("Password/View", GetViewPassowrd());
    set.setValue("Password/UseSystemCredential", GetUseSystemCredential());
    set.setValue("Connecter/Name/ShowProtocolPrefix", GetShowProtocolPrefix());
    set.setValue("Connecter/Name/ShowIpPort", GetShowIpPortInName());
    set.setValue("Viewer/AdaptWindows", (int)GetAdaptWindows());
    set.setValue("PluginsPath", GetPluginsPath());
    set.endGroup();
    return 0;
}

bool CParameterPlugin::GetCaptureAllKeyboard() const
{
    return m_bCaptureAllKeyboard;
}

void CParameterPlugin::SetCaptureAllKeyboard(bool bCapture)
{
    if(m_bCaptureAllKeyboard == bCapture)
        return;
    m_bCaptureAllKeyboard = bCapture;
    SetModified(true);
    emit sigCaptureAllKeyboard();
}

bool CParameterPlugin::GetDesktopShortcutsScript() const
{
    return m_bDesktopShortcutsScript;
}

void CParameterPlugin::SetDesktopShortcutsScript(bool newDesktopShortcutsScript)
{
    if(m_bDesktopShortcutsScript == newDesktopShortcutsScript)
        return;
    m_bDesktopShortcutsScript = newDesktopShortcutsScript;
    SetModified(true);
}

QString CParameterPlugin::GetRestoreDesktopShortcutsScript() const
{
    return m_szRestoreDesktopShortcutsScript;
}

void CParameterPlugin::SetRestoreDesktopShortcutsScript(const QString &newRestoreDesktopShortcutsScript)
{
    if(m_szRestoreDesktopShortcutsScript == newRestoreDesktopShortcutsScript)
        return;
    m_szRestoreDesktopShortcutsScript = newRestoreDesktopShortcutsScript;
    SetModified(true);
}

QString CParameterPlugin::GetDisableDesktopShortcutsScript() const
{
    return m_szDisableDesktopShortcutsScript;
}

void CParameterPlugin::SetDisableDesktopShortcutsScript(const QString &newDisableDesktopShortcutsScript)
{
    if(m_szDisableDesktopShortcutsScript == newDisableDesktopShortcutsScript)
        return;
    m_szDisableDesktopShortcutsScript = newDisableDesktopShortcutsScript;
    SetModified(true);
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

bool CParameterPlugin::GetUseSystemCredential() const
{
    return m_bUseSystemCredential;
}

void CParameterPlugin::SetUseSystemCredential(bool newUseSystemCredential)
{
    if(m_bUseSystemCredential == newUseSystemCredential)
        return;
    m_bUseSystemCredential = newUseSystemCredential;
    SetModified(true);
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

QStringList CParameterPlugin::GetPluginsPath() const
{
    return m_szPluginsPath;
}

void CParameterPlugin::SetPluginsPath(const QStringList &newPluginsPath)
{
    m_szPluginsPath = newPluginsPath;
}
