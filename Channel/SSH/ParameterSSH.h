#ifndef CPARAMETERSSH_H
#define CPARAMETERSSH_H

#include <QObject>
#include <QSettings>
#include "libssh/libssh.h"

class CParameterSSH : public QObject
{
    Q_OBJECT

public:
    explicit CParameterSSH(QObject *parent = nullptr);

    CParameterSSH(const CParameterSSH& c);
    
    virtual int Load(QSettings &set);
    virtual int Save(QSettings &set);

    QString GetServer() const;
    int setServer(const QString &szServer);
    
    quint16 GetPort() const;
    int SetPort(const quint16 nPort);
    
    QString GetUser() const;
    int SetUser(const QString &szUser);
    
    QString GetPassword() const;
    int SetPassword(const QString szPassword);
    
    int GetAuthenticationMethod() const;
    int SetAuthenticationMethod(int method);
    
    QString GetPassphrase() const;
    int SetPassphrase(const QString passphrase);

    ssh_publickey_hash_type GetPublicKeyHashType() const;
    int SetPublicKeyHashType(ssh_publickey_hash_type type);
    
    QString GetPublicKeyFile() const;
    int SetPublicKeyFile(const QString szFile);
    
    QString GetPrivateKeyFile() const;
    int SetPrivateKeyFile(const QString szFile);

    QString GetRemoteHost() const;
    int SetRemoteHost(const QString szHost);
    
    quint16 GetRemotePort() const;
    int SetRemotePort(const quint16 nPort);
    
    QString GetSourceHost() const;
    int SetSourceHost(const QString szHost);
    
    quint16 GetSourcePort() const;
    int SetSourcePort(const quint16 nPort);
    
private:
    QString m_szServer;
    quint16 m_nPort;
    QString m_szUser;
    QString m_szPassword;
    
    int m_nAuthenticationMethod;
    ssh_publickey_hash_type m_PublicKeyHashType;
    QString m_szPublicKeyFile;
    QString m_szPrivateKeyFile;
    QString m_szPassphrase;
    
    QString m_szRemoteHost;
    quint16 m_nRemotePort;
    
    QString m_szSourceHost;
    quint16 m_nSourcePort;
};

#endif // CPARAMETERSSH_H
