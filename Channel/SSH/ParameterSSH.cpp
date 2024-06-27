#include "ParameterSSH.h"
#include "RabbitCommonTools.h"

CParameterSSH::CParameterSSH(QObject *parent)
    : QObject{parent},
    m_nPort(22),
    m_szUser(RabbitCommon::CTools::Instance()->GetCurrentUser()),
    m_nAuthenticationMethod(SSH_AUTH_METHOD_PASSWORD|SSH_AUTH_METHOD_PUBLICKEY),
    m_PublicKeyHashType(SSH_PUBLICKEY_HASH_SHA1),
    m_nRemotePort(5900),
    m_szSourceHost("localhost"),
    m_nSourcePort(0)
{
    //TODO: remove it
    m_szServer = "192.168.11.28";
    m_szPublicKeyFile = "/home/l/.ssh/id_rsa.pub";
    m_szPrivateKeyFile = "/home/l/.ssh/id_rsa";
    m_szRemoteHost = "localhost";
    m_nRemotePort = 5906;
}

int CParameterSSH::Load(QSettings &set)
{
    set.beginGroup("SSH/Tunnel");
    setServer(set.value("Server", GetServer()).toString());
    SetPort(set.value("Port", GetPort()).toUInt());
    SetUser(set.value("User", GetUser()).toString());
    SetAuthenticationMethod(set.value("Authentication/Method",
                                      GetAuthenticationMethod()).toInt());
    SetPublicKeyHashType(
        (ssh_publickey_hash_type)set.value("PublicKey/Hash/Type",
                                GetPublicKeyHashType()).toInt());
    SetPublicKeyFile(set.value("Authentication/File/PublicKey",
                               GetPublicKeyFile()).toString());
    SetPrivateKeyFile(set.value("Authentication/File/PrivateKey",
                                GetPrivateKeyFile()).toString());
    SetPassphrase(set.value("Authentication/Passphrase",
                            GetPassphrase()).toString());
    SetRemoteHost(set.value("Remote/Host", GetRemoteHost()).toString());
    SetRemotePort(set.value("Remote/Port", GetRemotePort()).toUInt());
    SetSourceHost(set.value("Source/Host", GetSourceHost()).toString());
    SetSourcePort(set.value("Source/Port", GetSourcePort()).toUInt());
    set.endGroup();
    return 0;
}

int CParameterSSH::Save(QSettings &set)
{
    set.beginGroup("SSH/Tunnel");
    set.setValue("Server", GetServer());
    set.setValue("Port", GetPort());
    set.setValue("User", GetUser());
    //set.setValue("Password", GetPassword());
    set.setValue("Authentication/Method", GetAuthenticationMethod());
    set.setValue("Authentication/PublicKey/Hash/Type", GetPublicKeyHashType());
    set.setValue("Authentication/File/PublicKey", GetPublicKeyFile());
    set.setValue("Authentication/File/PrivateKey", GetPrivateKeyFile());
    set.setValue("Authentication/Passphrase", GetPassphrase());
    set.setValue("Remote/Host", GetRemoteHost());
    set.setValue("Remote/Port", GetRemotePort());
    set.setValue("Source/Host", GetSourceHost());
    set.setValue("Source/Port", GetSourcePort());
    set.endGroup();
    return 0;
}

QString CParameterSSH::GetServer() const
{
    return m_szServer;
}

int CParameterSSH::setServer(const QString &szServer)
{
    m_szServer = szServer;
    return 0;
}

quint16 CParameterSSH::GetPort() const
{
    return m_nPort;
}

int CParameterSSH::SetPort(const quint16 nPort)
{
    m_nPort = nPort;
    return 0;
}

QString CParameterSSH::GetUser() const
{
    return m_szUser;
}

int CParameterSSH::SetUser(const QString &szUser)
{
    m_szUser = szUser;
    return 0;
}

QString CParameterSSH::GetPassword() const
{
    return m_szPassword;
}

int CParameterSSH::SetPassword(const QString szPassword)
{
    m_szPassword = szPassword;
    return 0;
}

int CParameterSSH::GetAuthenticationMethod() const
{
    return m_nAuthenticationMethod;
}

int CParameterSSH::SetAuthenticationMethod(int method)
{
    m_nAuthenticationMethod = method;
    return 0;
}

QString CParameterSSH::GetPassphrase() const
{
    return m_szPassphrase;
}

int CParameterSSH::SetPassphrase(const QString passphrase)
{
    m_szPassphrase = passphrase;
    return 0;
}

ssh_publickey_hash_type CParameterSSH::GetPublicKeyHashType() const
{
    return m_PublicKeyHashType;
}

int CParameterSSH::SetPublicKeyHashType(ssh_publickey_hash_type type)
{
    m_PublicKeyHashType = type;
    return 0;
}

QString CParameterSSH::GetPublicKeyFile() const
{
    return m_szPublicKeyFile;
}

int CParameterSSH::SetPublicKeyFile(const QString szFile)
{
    m_szPublicKeyFile = szFile;
    return 0;
}

QString CParameterSSH::GetPrivateKeyFile() const
{
    return m_szPrivateKeyFile;
}

int CParameterSSH::SetPrivateKeyFile(const QString szFile)
{
    m_szPrivateKeyFile = szFile;
    return 0;
}

QString CParameterSSH::GetRemoteHost() const
{
    return m_szRemoteHost;
}

int CParameterSSH::SetRemoteHost(const QString szHost)
{
    m_szRemoteHost = szHost;
    return 0;
}

quint16 CParameterSSH::GetRemotePort() const
{
    return m_nRemotePort;
}

int CParameterSSH::SetRemotePort(const quint16 nPort)
{
    m_nRemotePort = nPort;
    return 0;
}

QString CParameterSSH::GetSourceHost() const
{
    return m_szSourceHost;
}

int CParameterSSH::SetSourceHost(const QString szHost)
{
    m_szSourceHost = szHost;
    return 0;
}

quint16 CParameterSSH::GetSourcePort() const
{
    return m_nSourcePort;
}

int CParameterSSH::SetSourcePort(const quint16 nPort)
{
    m_nSourcePort = nPort;
    return 0;
}