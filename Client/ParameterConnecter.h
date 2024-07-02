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
 * \brief 连接参数接口。仅在插件内有效。
 *  
 * \details
 *  - 应用程序不能访问，只能通过 CConnecter::OpenDialogSettings 进行设置。
 *  - 插件通过 CConnecterDesktopThread::GetParameter 访问
 *  - 此接口仅由插件派生实现和使用。
 *  - \ref sub_Use_CParameterClient
 *
 *  因为可能会有很多参数，所以需要按参数类型分类。每个分类可以从此类派生出一个单独的类。\n
 *  因为连接参数都有一些基本的参数（例如网络地址等），所以则从 CParameterBase 派生。
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
 *   : CParameterBase(parent),
 *     m_User(this, "Host"), // 需要 CFrmParameterClient， 在这里初始化，并且第一个参数必须设置为 this
 *     m_Video()             // 不需要 CFrmParameterClient， 在这里初始化，则不需要设置参数
 *  {}
 * \endcode
 *
 *  - \ref sub_Use_CParameterClient_in_CParameterConnecter
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
    explicit CParameterConnecter(QObject *parent = nullptr);
    /*!
     * \param parent 如果需要 CParameterClient ，则设置 parent
     */
    explicit CParameterConnecter(
        CParameterConnecter *parent,
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
