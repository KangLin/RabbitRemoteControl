#ifndef CPARAMETERVIEWER_H
#define CPARAMETERVIEWER_H

#pragma once

#include "Parameter.h"
#include "FrmViewer.h"
#include "ParameterRecord.h"

/*!
 * \~chinese 客户端库 (CClient) 参数。仅在客户端库 (CClient) 和插件中使用。
 * \details
 * - 此接口仅由客户端库 (CClient) 实例化，插件和客户端库（ CClient ) 使用。
 * - 插件通过 CParameterConnecter::GetParameterClient 访问。
 * - 应用程序不能访问，但是可以通过 CClient::GetSettingsWidgets 进行设置。
 * - \ref section_Use_CParameterClient
 *
 * \~english The parameters of client
 * \details
 *   - The parameters is valid in the CClient and plugin.
 *   - The interface only is implemented by CClient and used by CClient or plugin.
 *   - Plugin accessed via CParameterConnecter::GetParameterClient .
 *   - The application cannot access it,
 *     but the application can be set it via CClient::GetSettingsWidgets .
 *   - \ref section_Use_CParameterClient
 *
 * \~
 * \see CClient::CreateConnecter
 *      CConnecter::CConnecter
 *      CConnecter::SetParameterClient
 *      CConnecter::SetParameter
 *      CParameterConnecter
 * \ingroup CLIENT_PARAMETER
 */
class CLIENT_EXPORT CParameterClient : public CParameter
{
    Q_OBJECT

public:
    explicit CParameterClient(QObject *parent = nullptr);
    virtual ~CParameterClient();
    
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
    bool GetHookKeyboard() const;
    void SetHookKeyboard(bool newHookKeyboard);
Q_SIGNALS:
    void sigHookKeyboardChanged();
private:
    bool m_bHookKeyboard;
    Q_PROPERTY(bool HookKeyboard READ GetHookKeyboard WRITE SetHookKeyboard
               NOTIFY sigHookKeyboardChanged)
public:
    bool GetNativeWindowReceiveKeyboard() const;
    void SetNativeWindowReceiveKeyboard(bool newNativeWindowRecieveKeyboard);
Q_SIGNALS:
    void sigNativeWindowRecieveKeyboard();
private:
    bool m_bNativeWindowReceiveKeyboard;

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
        First,
        Always,
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
    CFrmViewer::ADAPT_WINDOWS GetAdaptWindows();
    void SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw);
Q_SIGNALS:
    void sigAdaptWindowsChanged();
private:
    CFrmViewer::ADAPT_WINDOWS m_AdaptWindows;
    Q_PROPERTY(CFrmViewer::ADAPT_WINDOWS AdaptWindows READ GetAdaptWindows WRITE SetAdaptWindows NOTIFY sigAdaptWindowsChanged)

public:
    CParameterRecord m_Record;
};

#endif // CPARAMETERVIEWER_H
