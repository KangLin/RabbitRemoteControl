#ifndef CPARAMETERCHANNELSSH_H
#define CPARAMETERCHANNELSSH_H

#pragma once

#include <QObject>
#include <QSettings>
#include "libssh/libssh.h"
#include "channel_export.h"

class CHANNEL_EXPORT CParameterChannelSSH : public QObject
{
    Q_OBJECT

public:
    explicit CParameterChannelSSH(QObject *parent = nullptr);

    CParameterChannelSSH(const CParameterChannelSSH& c);

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
    
    bool GetUseSystemFile() const;
    int SetUseSystemFile(bool use);
    
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
    
    QString GetPcapFile() const;
    int SetPcapFile(const QString& szFile);

private:
    QString m_szServer;
    quint16 m_nPort;
    QString m_szUser;
    QString m_szPassword;
    
    int m_nAuthenticationMethod;
    QString m_szPublicKeyFile;
    QString m_szPrivateKeyFile;
    QString m_szPassphrase;
    bool m_bUseSystemFile;
    
    QString m_szRemoteHost;
    quint16 m_nRemotePort;
    
    QString m_szSourceHost;
    quint16 m_nSourcePort;
    
    QString m_pcapFile;
};

#endif // CPARAMETERCHANNELSSH_H
