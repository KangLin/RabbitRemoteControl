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
 * \brief the parameters of connecter interface.
 *        It only valid in plugin.
 *        The application cannot access it directly,
 *        it can only be set via CConnecter::OpenDialogSettings.
 * \note
 *  - The interface only is implemented and used by plugin.
 *  - If it or its derived class requires a CParameterClient.
 *    Please instantiate the parameters and call CConnecter::SetParameter
 *    in the constructor of the CConnecter derived class
 *    to set the parameters pointer.
 *    Default set the CParameterClient for it. See: CClient::CreateConnecter .
 *    If you are sure to it does not need CParameterClient.
 *    please overload the CConnecter::SetParameterClient in the CConnecter derived class.
 *    don't set it.
 *
 * \~chinese
 * \brief 连接参数接口。包括基本参数。此类仅在插件内有效。
 *  
 * \note
 *  - 应用程序不能直接访问，只能通过 CConnecter::OpenDialogSettings 进行设置。
 *  - 插件通过 CConnecterDesktopThread::GetParameter 访问
 *  - 此接口仅由插件派生实现和使用。
 *  - 如果它或其派生类需要 CParameterClient 。
 *    - 请在 CConnecter 的派生类的构造函数中实例化 CParameterConnecter 或其派生类，
 *      并调用 CConnecter::SetParameter 设置参数指针。
 *    - 默认会自动为它设置 CParameterClient 。详见: CClient::CreateConnecter 。
 *    - 如果参数不需要 CParameterClient ，那请在 CConnecter 派生类
 *    - 重载 CConnecter::SetParameterClient 不设置 CParameterClient 。
 *
 *
 * \details
 *  连接参数接口。包括基本参数（网络参数等）。可以按参数类型分类建立派生类。
 *
 *  例如：\n
 *  连接参数包括以下几种类型：
 *  - 用户参数 (class CParameterUser : public CParameterConnecter)
 *  - 视频参数 (class CParameterVideo : public CParameterConnecter)
 *  - 音频参数 (class CParameterAudio : public CParameterConnecter)
 *  
 *  其中 CParameterUser 它需要 CFrmParameterClient ，其它的类型则不需要。
 *
 *  - 那么连接参数可以是以上类型的集合：
 *
 *  \code
 *  class CParameterConnect : public CParameterConnecter
 *  {
 *  public:
 *      explicit CParameterConnect::CParameterConnect(
 *          CParameterConnecter *parent = nullptr);
 *
 *      CParameterUser m_User;
 *      CParameterVideo m_Video;
 *      CParameterAudio m_Audio;
 *  };
 *  \endcode
 *
 *  - 对需要 CFrmParameterClient 的 CParameterUser 使用其构造函数
 *    CParameterConnecter(CParameterConnecter *parent, const QString& szPrefix)
 *    进行实例化。
 *  - 对不需要 CFrmParameterClient 的 CParameterVideo 使用其默认构造函数
 *    CParameterConnecter(QObject *parent = nullptr) 进行实例化。
 *
 *  \code
 *  CParameterConnect::CParameterConnect(CParameterConnecter *parent = nullptr)
 *   : CParameterConnecter(parent),
 *     m_User(this, "Host"), // 需要 CFrmParameterClient， 在这里初始化，并且第一个参数需要设置为 this
 *     m_Video()             // 不需要 CFrmParameterClient， 在这里初始化，则不需要设置参数
 *  {}
 * \endcode
 *
 *  - 连接参数使用 CFrmParameterClient 的值做为其初始值。
 *    请在 CParameterConnecter::onLoad 中初始化。\n
 *    例如：保存密码可以以它为初始化值。
 *    \snippet Client/ParameterCompone/ParameterUser.cpp Initialize parameter
 *    \see CParameterUser::onLoad
 *
 * \~
 * \see CClient::CreateConnecter
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
    Q_PROPERTY(QString Name READ GetName WRITE SetName NOTIFY sigNameChanged)
    Q_PROPERTY(QString ServerName READ GetServerName WRITE SetServerName)
    Q_PROPERTY(bool ShowServerName READ GetShowServerName WRITE SetShowServerName NOTIFY sigShowServerNameChanged)
    Q_PROPERTY(QString Host READ GetHost WRITE SetHost)
    Q_PROPERTY(quint16 Port READ GetPort WRITE SetPort)
    Q_PROPERTY(QString User READ GetUser WRITE SetUser)
    Q_PROPERTY(QString Password READ GetPassword WRITE SetPassword)
    Q_PROPERTY(bool SavePassword READ GetSavePassword WRITE SetSavePassword)
    Q_PROPERTY(bool OnlyView READ GetOnlyView WRITE SetOnlyView)
    Q_PROPERTY(bool LocalCursor READ GetLocalCursor WRITE SetLocalCursor)
    Q_PROPERTY(bool Clipboard READ GetClipboard WRITE SetClipboard)
    Q_PROPERTY(emProxy ProxyType READ GetProxyType WRITE SetProxyType)
    Q_PROPERTY(QString ProxyHost READ GetProxyHost WRITE SetProxyHost)
    Q_PROPERTY(qint16 ProxyPort READ GetProxyPort WRITE SetProxyPort)
    Q_PROPERTY(QString ProxyUser READ GetProxyUser WRITE SetProxyUser)
    Q_PROPERTY(QString ProxyPassword READ GetProxyPassword WRITE SetProxyPassword)
    
public:
    explicit CParameterConnecter(QObject *parent = nullptr);
    /*!
     * \param parent 如果需要 CParameterClient ，则设置 parent
     */
    explicit CParameterConnecter(
        CParameterConnecter *parent,
        const QString& szPrefix = QString());

    CParameterClient* GetParameterClient();
    int SetParameterClient(CParameterClient* p);

    /*!
     * \brief Check whether the parameters are complete
     *  to decide whether to open the parameter dialog 
     * \return 
     */
    virtual bool GetCheckCompleted();
    
    const QString GetName() const;
    void SetName(const QString& szName);
    
    const QString GetServerName() const;
    void SetServerName(const QString& szName);

    bool GetShowServerName() const;
    void SetShowServerName(bool NewShowServerName);
    
    virtual const QString GetHost() const;
    virtual void SetHost(const QString& szHost);
    
    virtual const quint16 GetPort() const;
    virtual void SetPort(quint16 port);
    
    virtual const QString GetUser() const;
    virtual void SetUser(const QString& szUser);
    
    virtual const QString GetPassword() const;
    virtual void SetPassword(const QString& szPassword);
    
    const bool GetSavePassword() const;
    /*!
     * \brief Set save password
     * \param save
     */
    void SetSavePassword(bool save);

    const bool GetOnlyView() const;
    virtual void SetOnlyView(bool only);
    
    virtual const bool GetLocalCursor() const;
    virtual void SetLocalCursor(bool cursor);
    
    virtual const bool GetClipboard() const;
    virtual void SetClipboard(bool c);
    
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
    
protected:
    virtual int onLoad(QSettings &set) override;
    virtual int onSave(QSettings &set) override;
    
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

    QString m_szName;
    QString m_szServerName;
    bool m_bShowServerName;
    QString m_szHost;
    quint16 m_nPort;
    
    QString m_szUser;
    QString m_szPassword;
    bool m_bSavePassword;
    
    bool m_bOnlyView;
    bool m_bLocalCursor;
    bool m_bClipboard;
    
    emProxy m_eProxyType;
    QString m_szProxyHost;
    quint16 m_nProxyPort;
    QString m_szProxyUser;
    QString m_szProxyPassword;    
};

#endif // CPARAMTERCONNECT_H
