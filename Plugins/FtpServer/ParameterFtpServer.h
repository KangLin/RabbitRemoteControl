// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterNet.h"

class CParameterFtpServer : public CParameterOperate
{
    Q_OBJECT

public:
    explicit CParameterFtpServer(
        QObject *parent = nullptr,
        const QString& szPrefix = QString());
    
    CParameterNet m_Net;

    bool GetAnonymousLogin() const;
    void SetAnonymousLogin(bool newAnonymousLogin);
    bool GetReadOnly() const;
    void SetReadOnly(bool newReadOnly);
    
    QString GetRoot() const;
    void SetRoot(const QString &newRoot);
    /*!
     * \brief Connect count.
     * \return Connect count.
     *        -1: enable all connect
     */
    int GetConnectCount() const;
    void SetConnectCount(int newConnectCount);
    
    bool GetListenAll() const;
    void SetListenAll(bool newListenAll);
    
    QStringList GetListen() const;
    void SetListen(const QStringList &newListen);
    
    QStringList GetWhitelist() const;
    void SetWhitelist(const QStringList &newWhitelist);
    
    QStringList GetBlacklist() const;
    void SetBlacklist(const QStringList &newBlacklist);
    
private:
    QString m_szRoot;
    bool m_bAnonymousLogin;
    bool m_bReadOnly;
    int m_ConnectCount;
    bool m_bListenAll;
    QStringList m_Listen;
    QStringList m_Whitelist;
    QStringList m_Blacklist;
    
    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
