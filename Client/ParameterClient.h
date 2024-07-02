#ifndef CPARAMETERVIEWER_H
#define CPARAMETERVIEWER_H

#pragma once

#include "Parameter.h"

/*!
 * \~english The parameters of client
 * \details
 *   - The parameters is valid in the CClient and plugin.
 *   - The interface only is implemented by CClient and used by CClient or plugin.
 *   - Plugin accessed via CParameterConnecter::GetParameterClient .
 *   - The application cannot access it,
 *     but the application can be set it via CClient::GetSettingsWidgets .
 *
 * \subsection sub_Use_CParameterClient Use CParameterClient
 * \subsubsection sub_Set_CParameterClient_in_CParameterConnecter Set CParameterClient for CParameterConnecter
 *  - If the parameters of connecter ( CParameterConnecter or it's derived class ) requires a CParameterClient.
 *    - Please instantiate the parameters of connecter in the constructor of the CConnecter or it's derived class.
 *    - Call CConnecter::SetParameter to set the parameters.
 *    - Default set the CParameterClient for the parameters of connecter. See: CClient::CreateConnecter .
 *    - If you are sure to the parameter does not need CParameterClient.
 *      please overload the CConnecter::SetParameterClient in the CConnecter derived class.
 *      don't set it.
 * \subsubsection sub_Use_CParameterClient_in_CParameterConnecter Use CParameterClient in CParameterConnecter
 *  - Use CParameterConnecter::GetParameterClient() get CParameterClient
 *    in the parameters of connecter ( CParameterConnecter or it's derived class )
 *  - The parameters of connecter( CParameterConnecter or it's derived class )
 *    use the value of CParameterClient as its initial value. \n
 *    Please override CParameterConnecter::slotSetParameterClient and initialize the relevant values in it.
 *    For example, Whether to save the password with the member value of CParameterClient as its initialization value.
 *    \snippet Client/ParameterCompone/ParameterUser.cpp Initialize parameter after set CParameterClient
 *    \see CParameterUser::OnLoad
 *
 *
 * \~chinese 客户端库 (CClient) 参数。仅在客户端库 (CClient) 和插件中使用。
 * \details
 * - 此接口仅由客户端库 (CClient) 实例化，插件和客户端库（ CClient ) 使用。
 * - 插件通过 CParameterConnecter::GetParameterClient 访问。
 * - 应用程序不能访问，但是可以通过 CClient::GetSettingsWidgets 进行设置。
 *
 * \subsection sub_Use_CParameterClient 使用客户端库参数 (CParameterClient)
 * \subsubsection sub_Set_CParameterClient_in_CParameterConnecter 为连接参数 ( CParameterConnecter 或其派生类 ) 设置客户端库参数 (CParameterClient)
 *  - 如果连接参数 ( CParameterConnecter 或其派生类 ) 需要客户端库参数 (CParameterClient) 。
 *    - 请在 CConnecter 派生类的构造函数中实例化连接参数。
 *    - 调用 CConnecter::SetParameter 设置参数指针。
 *    - 默认会自动为连接参数设置 CParameterClient 。详见: CClient::CreateConnecter 。
 *    - 如果参数不需要 CParameterClient ，
 *      那请在 CConnecter 派生类重载 CConnecter::SetParameterClient 不设置它。
 * \subsubsection sub_Use_CParameterClient_in_CParameterConnecter 在连接参数 ( CParameterConnecter 或其派生类 ) 中使用客户端库参数 (CParameterClient)
 *   - 在连接参数 ( CParameterConnecter 或其派生类 ) 使用 CParameterConnecter::GetParameterClient() 来得到客户端库参数 (CParameterClient) 。
 *   - 连接参数 ( CParameterConnecter 或其派生类 ) 使用客户端库参数 (CParameterClient) 的值做为其初始值。
 *     请重载 CParameterConnecter::slotSetParameterClient 并在其中初始化相关的值。\n
 *     例如：是否保存密码以 CParameterClient 的成员值做为其初始化值。
 *     \snippet Client/ParameterCompone/ParameterUser.cpp Initialize parameter after set CParameterClient
 *     \see CParameterUser::OnLoad
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

};

#endif // CPARAMETERVIEWER_H
