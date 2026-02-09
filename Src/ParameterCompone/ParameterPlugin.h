// Author: Kang Lin <kl222@126.com>

#pragma once

#include "FrmViewer.h"
#include "ParameterRecord.h"
#include "ParameterMediaDevices.h"

#if defined(HAVE_QTERMWIDGET)
#include "ParameterTerminal.h"
#endif

#include "ParameterFilter.h"

/*!
 * \~chinese 插件的全局参数。
 * \details
 * - 仅在插件管理者 (CManager) 和插件中使用。
 * - 应用程序不能访问，但是可以通过 CManager::GetSettingsWidgets 进行设置。
 * - 插件可通过 CParameterOperate::GetGlobalParameters() 访问。
 *
 * \~english Global parameters of plugins
 * - The parameters is valid in the CManager and plugin.
 * - The application cannot access it,
 *   but the application can be set it via CManager::GetSettingsWidgets .
 * - The plugin can be accessed via CParameterOperate::GetGlobalParameters().
 *
 * \~
 * \see COperate::SetGlobalParameters
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class PLUGIN_EXPORT CParameterPlugin : public CParameter
{
    Q_OBJECT

public:
    explicit CParameterPlugin(QObject *parent = nullptr);
    virtual ~CParameterPlugin();
    
protected:
    /*!
     * \see CClient::LoadSettings
     */
    virtual int OnLoad(QSettings &set) override;
    /*!
     * \see CClient::SaveSettings
     */
    virtual int OnSave(QSettings &set) override;

public:
    bool GetCaptureAllKeyboard() const;
    void SetCaptureAllKeyboard(bool bCapture);
Q_SIGNALS:
    void sigCaptureAllKeyboard();
private:
    bool m_bCaptureAllKeyboard;

public:
    bool GetDesktopShortcutsScript() const;
    void SetDesktopShortcutsScript(bool newDesktopShortcutsScript);
private:
    bool m_bDesktopShortcutsScript;
public:
    QString GetDisableDesktopShortcutsScript() const;
    void SetDisableDesktopShortcutsScript(const QString &newDisableDesktopShortcutsScript);
private:
    QString m_szDisableDesktopShortcutsScript;
public:
    QString GetRestoreDesktopShortcutsScript() const;
    void SetRestoreDesktopShortcutsScript(const QString &newRestoreDesktopShortcutsScript);
private:
    QString m_szRestoreDesktopShortcutsScript;

public:
    bool GetEnableLocalInputMethod() const;
    void SetEnableLocalInputMethod(bool enable);
private:
    bool m_bEnableLocalInputMethod;

public:
    bool GetPromptAdministratorPrivilege();
    void SetPromptAdministratorPrivilege(bool bShow);
Q_SIGNALS:
    void sigPromptAdministratorPrivilege();
private:
    bool m_bPromptAdministratorPrivilege;

    // Enable use the system user as the user
public:
    bool GetEnableSystemUserToUser() const;
    void SetEnableSystemUserToUser(bool enable);
private:
    bool m_bEnableSystemUserToUser;

    //////////////// Password ////////////////
public:
    const QString &GetEncryptKey() const;
    void SetEncryptKey(const QString &newPassword);
Q_SIGNALS:
    void sigEncryptKeyChanged();
private:
    QString m_szEncryptKey; //Don't save to file
    Q_PROPERTY(QString EncryptKey READ GetEncryptKey WRITE SetEncryptKey NOTIFY sigEncryptKeyChanged)

public:
    const bool &GetSavePassword() const;
    void SetSavePassword(bool NewAutoSavePassword);
Q_SIGNALS:
    void sigSavePasswordChanged(bool AutoSavePassword);

private:
    bool m_bSavePassword;
    Q_PROPERTY(bool SavePassword READ GetSavePassword WRITE SetSavePassword NOTIFY sigSavePasswordChanged)
 
public:
    enum class PromptType
    {
        No,
        First
    };
    Q_ENUM(PromptType)
    PromptType GetPromptType() const;
    void SetPromptType(PromptType NewPromptType);
Q_SIGNALS:
    void sigPromptTypeChanged(PromptType PromptType);    
private:
    PromptType m_PromptType;
    Q_PROPERTY(PromptType PromptType READ GetPromptType WRITE SetPromptType NOTIFY sigPromptTypeChanged)
    
public:
    int GetPromptCount() const;
    void SetPromptCount(int NewPromptCount);
Q_SIGNALS:
    void sigPromptCountChanged(int PromptCount);
private:
    int m_nPromptCount;
    Q_PROPERTY(int PromptCount READ GetPromptCount WRITE SetPromptCount NOTIFY sigPromptCountChanged)

public:
    bool GetViewPassowrd() const;
    void SetViewPassowrd(bool NewViewPassowrd);
Q_SIGNALS:
    void sigViewPassowrdChanged(bool ViewPassowrd);
private:
    bool m_bViewPassowrd;
    Q_PROPERTY(bool ViewPassowrd READ GetViewPassowrd WRITE SetViewPassowrd NOTIFY sigViewPassowrdChanged)

public:
    bool GetUseSystemCredential() const;
    void SetUseSystemCredential(bool newUseSystemCredential);
private:
    bool m_bUseSystemCredential;

    //////////////// Password end ////////////////
    
public:
    bool GetShowProtocolPrefix() const;
    void SetShowProtocolPrefix(bool bShowProtocolPrefix);
Q_SIGNALS:
    void sigShowProtocolPrefixChanged();    
private:
    bool m_bShowProtocolPrefix;
    Q_PROPERTY(bool ShowProtocolPrefix READ GetShowProtocolPrefix WRITE SetShowProtocolPrefix NOTIFY sigShowProtocolPrefixChanged)

public:
    bool GetShowIpPortInName() const;
    void SetShowIpPortInName(bool bShowIpPortInName);
Q_SIGNALS:
    void sigSHowIpPortInNameChanged();
private:
    bool m_bShowIpPortInName;
    Q_PROPERTY(bool ShowIpPortInName READ GetShowIpPortInName WRITE SetShowIpPortInName NOTIFY sigSHowIpPortInNameChanged)

public:
    enum SaveSettingsType{
        File,
        Database
    };
    SaveSettingsType GetSaveSettingsType() const;
    void SetSaveSettingsType(const SaveSettingsType& type);
private:
    SaveSettingsType m_SaveSettingsType;

public:
    CFrmViewer::ADAPT_WINDOWS GetAdaptWindows();
    void SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw);
Q_SIGNALS:
    void sigAdaptWindowsChanged();
private:
    CFrmViewer::ADAPT_WINDOWS m_AdaptWindows;
    Q_PROPERTY(CFrmViewer::ADAPT_WINDOWS AdaptWindows READ GetAdaptWindows WRITE SetAdaptWindows NOTIFY sigAdaptWindowsChanged)

public:
    bool GetEnableSetPluginsPath() const;
    void SetEnableSetPluginsPath(bool newEnableSetPluginsPath);
private:
    bool m_bEnableSetPluginsPath;

public:
    QStringList GetPluginsPath() const;
    void SetPluginsPath(const QStringList &newPluginsPath);
    bool GetOnlyLoadInWhitelist() const;
    void SetOnlyLoadInWhitelist(bool newOnlyLoadInWhitelist);
    CParameterFilter m_WhiteList;
    CParameterFilter m_BlackList;
private:
    QStringList m_szPluginsPath;
    bool m_bOnlyLoadInWhitelist;

public:
    CParameterRecord m_Record;
    CParameterMediaDevices m_MediaDevices;
#if defined(HAVE_QTERMWIDGET)
    CParameterTerminal m_Terminal;
#endif
};
