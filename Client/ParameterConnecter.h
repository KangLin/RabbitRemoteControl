// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMTERCONNECT_H
#define CPARAMTERCONNECT_H

#pragma once

#include "ParameterClient.h"
#include <QDataStream>

class CConnecter;

/**
 * \~english
 * \brief The parameter interface. It contains basic parameters.
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
 * \brief 连接参数接口。它包含基本参数。此类仅在插件内有效。
 *        应用程序不能直接访问，只能通过 CConnecter::OpenDialogSettings 进行设置。
 * \note
 *  - 此接口仅由插件派生实现和使用。
 *  - 如果它或其派生类需要 CParameterClient 。
 *    请在 CConnecter 的派生类的构造函数中实例化参数，并调用 CConnecter::SetParameter 设置参数指针。
 *    默认会自动为它设置 CParameterClient 。详见: CClient::CreateConnecter 。
 *    如果参数不需要 CParameterClient ，那请在 CConnecter 派生类
 *    重载 CConnecter::SetParameterClient 不设置 CParameterClient 。
 *
 * \~
 * \see CClient::CreateConnecter CConnecter::CConnecter
 *      CConnecter::SetParameterClient CConnecter::SetParameter
 *      CParameterClient
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

    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

    CParameterClient* GetParameterClient();

    /*!
     * \brief Check whether the parameters are complete
     *  to decide whether to open the parameter dialog 
     * \return 
     */
    virtual bool GetCheckCompleted();
    
    const QString GetName() const;
    void SetName(const QString& name);
    
    const QString GetServerName() const;
    void SetServerName(const QString& name);

    bool GetShowServerName() const;
    void SetShowServerName(bool NewShowServerName);
    
    const QString GetHost() const;
    void SetHost(const QString& szHost);
    
    const quint16 GetPort() const;
    void SetPort(quint16 port);
    
    const QString GetUser() const;
    void SetUser(const QString &user);
    
    const QString GetPassword() const;
    void SetPassword(const QString& password);
    
    const bool GetSavePassword() const;
    /*!
     * \brief Set save password
     * \param save
     */
    void SetSavePassword(bool save);

    const bool GetOnlyView() const;
    void SetOnlyView(bool only);
    
    const bool GetLocalCursor() const;
    void SetLocalCursor(bool cursor);
    
    const bool GetClipboard() const;
    void SetClipboard(bool c);
    
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
    QByteArray PasswordSum(const std::string &password, const std::string &key);
    int LoadPassword(const QString &szTitle, const QString &szKey,
                     QString &password, QSettings &set);
    int SavePassword(const QString &szKey, const QString &password,
                     QSettings &set, bool bSave = false);

private:
    friend CConnecter;

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
