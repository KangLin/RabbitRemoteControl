// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterNet.h"

class PLUGIN_EXPORT CParameterServer : public CParameterOperate
{
    Q_OBJECT
    
public:
    explicit CParameterServer(QObject *parent = nullptr,
                              const QString& szPrefix = QString());

    CParameterNet m_Net;

public:
    bool GetAnonymousLogin() const;
    void SetAnonymousLogin(bool newAnonymousLogin);
private:
    bool m_bAnonymousLogin;
public:
    bool GetReadOnly() const;
    void SetReadOnly(bool newReadOnly);
private:
    bool m_bReadOnly;
public:
    QString GetRoot() const;
    void SetRoot(const QString &newRoot);
private:
    QString m_szRoot;
public:
    QString GetHostKeyFile() const;
    void SetHostKeyFile(const QString& key);
private:
    QString m_szHostKeyFile; // Private key
public:
    /*!
     * \brief Get authenticate time out
     * \return Authenticate time out(Unit: milliseconds)
     */
    qint64 GetAuthenticateTime() const;
    /*!
     * \brief Set authenticate time out
     * \param tm: Authenticate time out(Unit: milliseconds)
     */
    void SetAuthenticateTime(qint64 tm);
private:
    qint64 m_tmAuthenticate;
public:
    /*!
     * \brief Get authenticate attempts
     * \return Attempts
     */
    int GetAuthenticateAttempts() const;
    /*!
     * \brief Set authenticate attempts
     * \param attempts
     */
    void SetAuthenticateAttempts(int attempts);
private:
    int m_AuthAttempts;
public:
    /*!
     * \brief Connect count.
     * \return Connect count.
     *        -1: enable all connect
     */
    int GetConnectCount() const;
    void SetConnectCount(int newConnectCount);
private:
    int m_ConnectCount;
public:
    bool GetListenAll() const;
    void SetListenAll(bool newListenAll);
private:
    bool m_bListenAll;
public:
    QStringList GetListen() const;
    void SetListen(const QStringList &newListen);
private:
    QStringList m_Listen;
public:
    QStringList GetWhitelist() const;
    void SetWhitelist(const QStringList &newWhitelist);
    QStringList GetBlacklist() const;
    void SetBlacklist(const QStringList &newBlacklist);
private:
    QStringList m_Whitelist;
    QStringList m_Blacklist;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
