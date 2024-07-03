// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMTERCONNECT_H
#define CPARAMTERCONNECT_H

#pragma once

#include "ParameterClient.h"

/*!
 * \~chinese
 * \defgroup CLIENT_PARAMETER_COMPONE 参数组件
 * \brief 参数组件
 * \~english
 * \defgroup CLIENT_PARAMETER_COMPONE Parameter compone
 * \brief Parameter compone
 * \~
 * \ingroup CLIENT_PARAMETER
 */

/*!
 * \~english
 * \brief the parameters of connecter interface. It only valid in plugin.
 *        
 * \note
 *  - The interface only is implemented and used by plugin.
 *  - The application cannot access it directly,
 *    it can only be set via CConnecter::OpenDialogSettings.
 *  - The plugin can access via CConnecterDesktopThread::GetParameter
 *
 * \details
 * \subsection sub_Use_CParameterClient Use CParameterClient
 * \subsubsection sub_Set_CParameterClient_in_CParameterConnecter Set CParameterClient for CParameterConnecter
 *  - If the parameters of connecter ( CParameterConnecter or it's derived class ) requires a CParameterClient.
 *    - Please instantiate the parameters of connecter in the constructor of the CConnecter or it's derived class. ag:
 *      \snippet Plugins/FreeRDP/Client/ConnecterFreeRDP.h Initialize parameter
 *    - Call CConnecter::SetParameter to set the parameters. ag:
 *      \snippet Plugins/FreeRDP/Client/ConnecterFreeRDP.cpp Set the parameter
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
 * \~chinese
 * \brief 连接参数接口。仅在插件内有效。
 *  
 * \note
 *  - 仅在插件中实现和使用
 *  - 应用程序不能访问，只能通过 CConnecter::OpenDialogSettings 进行设置。
 *  - 插件通过 CConnecterDesktopThread::GetParameter 访问
 *
 * \details
 * \subsection sub_Use_CParameterClient 使用客户端库参数 (CParameterClient)
 * \subsubsection sub_Set_CParameterClient_in_CParameterConnecter 为连接参数 ( CParameterConnecter 或其派生类 ) 设置客户端库参数 (CParameterClient)
 *  - 如果连接参数 ( CParameterConnecter 或其派生类 ) 需要客户端库参数 (CParameterClient) 。
 *    - 请在 CConnecter 派生类的构造函数中实例化连接参数。例如：
 *      \snippet Plugins/FreeRDP/Client/ConnecterFreeRDP.h Initialize parameter
 *    - 调用 CConnecter::SetParameter 设置参数指针。例如：
 *      \snippet Plugins/FreeRDP/Client/ConnecterFreeRDP.cpp Set the parameter
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
 * \subsection sub_CParameterConnecter_CATEGORY_USAGE 参数分类使用
 *  因为可能会有很多参数，所以需要按参数类型分类。每个分类可以从此类派生出一个单独的类。\n
 *  因为连接参数都有一些基本的参数（例如网络地址等），所以插件的连接参数从 CParameterBase 派生。
 *  而每个分类做为它的成员变量。
 *
 *  例如：\n
 *  连接参数包括以下几种类型：
 *  - 基本参数 (class CParameterBase : public CParameterConnecter)
 *  - 用户参数 (class CParameterUser : public CParameterConnecter)
 *  - 视频参数 (class CParameterVideo : public CParameterConnecter)
 *  - 音频参数 (class CParameterAudio : public CParameterConnecter)
 *
 *  其中 CParameterUser 它需要 CFrmParameterClient ，其它的类型则不需要。
 *
 *  那么连接参数可以是以上类型的集合：
 *
 *  \code
 *  class CParameterConnect : public CParameterBase
 *  {
 *  public:
 *      explicit CParameterConnect::CParameterConnect(
 *          CParameterConnecter *parent = nullptr);
 *
 *      CParameterUser m_User;
 *      CParameterVideo m_Video;
 *      CParameterAudio m_Audio;
 *  };
 *
 *  CParameterConnect::CParameterConnect(CParameterConnecter *parent = nullptr)
 *   : CParameterBase(parent),
 *     m_User(this, "Host"),
 *     m_Video(this),
 *     m_Audio(this)
 *  {}
 *  \endcode
 *
 *  \note 当成员实例化时，必须设置构造函数的参数 parent 为 this
 * 
 *  - \ref sub_Set_CParameterClient_in_CParameterConnecter
 *
 * \~
 * \see CParameterBase
 *      CClient::CreateConnecter
 *      CConnecter::CConnecter
 *      CConnecter::SetParameterClient
 *      CConnecter::SetParameter
 *      CParameterClient
 *      CParameterConnecter::GetParameterClient
 * \ingroup CLIENT_PARAMETER
 */
class CLIENT_EXPORT CParameterConnecter : public CParameter
{
    Q_OBJECT
    Q_PROPERTY(emProxy ProxyType READ GetProxyType WRITE SetProxyType)
    Q_PROPERTY(QString ProxyHost READ GetProxyHost WRITE SetProxyHost)
    Q_PROPERTY(qint16 ProxyPort READ GetProxyPort WRITE SetProxyPort)
    Q_PROPERTY(QString ProxyUser READ GetProxyUser WRITE SetProxyUser)
    Q_PROPERTY(QString ProxyPassword READ GetProxyPassword WRITE SetProxyPassword)
    
public:
    explicit CParameterConnecter(
        QObject *parent = nullptr,
        const QString& szPrefix = QString());

    //! Get CParameterClient
    CParameterClient* GetParameterClient();
    int SetParameterClient(CParameterClient* p);

protected:
    //! Load parameters from settings
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

Q_SIGNALS:
    void sigSetParameterClient();

protected Q_SLOTS:
    /*!
     * \~chinese 设置 CParameterClient 后调用，用于初始化与 CParameterClient 有关的操作。
     *
     * \~english Call after set CParameterClient.
     * Used to initialize operations related to CParameterClient.
     *
     * \~
     * \ref sub_Use_CParameterClient_in_CParameterConnecter
     */
    virtual void slotSetParameterClient();

protected:
    QByteArray PasswordSum(const std::string &password, const std::string &key);
    int LoadPassword(const QString &szTitle, const QString &szKey,
                     QString &password, QSettings &set);
    int SavePassword(const QString &szKey, const QString &password,
                     QSettings &set, bool bSave = false);

private:
    CParameterConnecter* m_Parent;
    /*!
     * \see CClient::CreateConnecter CConnecter::SetParameterClient
     */
    CParameterClient* m_pParameterClient;

public:
        
    enum class emProxy {
        No,
        SocksV4,
        SocksV5,
        Http,
        User = 100
    };
    
    const emProxy GetProxyType() const;
    void SetProxyType(emProxy type);
    const QString GetProxyHost() const;
    void SetProxyHost(const QString& host);
    const quint16 GetProxyPort() const;
    void SetProxyPort(quint16 port);
    const QString GetProxyUser() const;
    void SetProxyUser(const QString& user);
    const QString GetProxyPassword() const;
    void SetProxyPassword(const QString& password);
        
Q_SIGNALS:
    void sigNameChanged(const QString &name = QString());
    void sigShowServerNameChanged();

private:

    emProxy m_eProxyType;
    QString m_szProxyHost;
    quint16 m_nProxyPort;
    QString m_szProxyUser;
    QString m_szProxyPassword;    
};

#endif // CPARAMTERCONNECT_H
