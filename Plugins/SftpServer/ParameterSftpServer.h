// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterServer.h"

class CParameterSftpServer : public CParameterServer
{
    Q_OBJECT

public:
    explicit CParameterSftpServer(QObject *parent = nullptr,
                                  const QString& szPrefix = QString());

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

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
