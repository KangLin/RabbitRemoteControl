// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterNet.h"
#include "ParameterOperate.h"

class CParameterSftpServer : public CParameterOperate
{
    Q_OBJECT

public:
    explicit CParameterSftpServer(QObject *parent = nullptr,
                                  const QString& szPrefix = QString());

    CParameterNet m_Net;

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
    qint64 GetAuthenticateTime() const;
    void SetAuthenticateTime(qint64 tm);
private:
    qint64 m_tmAuthenticate;
public:
    int GetAuthenticateAttempts() const;
    void SetAuthenticateAttempts(int attempts);
private:
    int m_AuthAttempts;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
