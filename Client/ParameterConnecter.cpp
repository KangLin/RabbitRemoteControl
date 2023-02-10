// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QCryptographicHash>
#include <QInputDialog>

#include "ParameterConnecter.h"
#include "RabbitCommonEncrypt.h"
#include "RabbitCommonTools.h"
#include "DlgInputPassword.h"

Q_DECLARE_LOGGING_CATEGORY(Client);

CParameterConnecter::CParameterConnecter(QObject *parent)
    : CParameter(parent),
      m_pParameterClient(nullptr),
      m_bShowServerName(true),
      m_nPort(0),
      m_bSavePassword(false),
      m_bOnlyView(false),
      m_bLocalCursor(true),
      m_bClipboard(true),
      m_eProxyType(emProxy::No),
      m_nProxyPort(1080)
{
    SetUser(RabbitCommon::CTools::GetCurrentUser());
}

CParameterClient* CParameterConnecter::GetParameterClient()
{
    return m_pParameterClient;
}

bool CParameterConnecter::GetCheckCompleted()
{
    if(GetSavePassword()) return true;
    return false;
}

const QString CParameterConnecter::GetName() const
{
    return m_szName;
}

void CParameterConnecter::SetName(const QString& name)
{
    if(m_szName == name)
        return;
    m_szName = name;
    SetModified(true);
    emit sigNameChanged(m_szName);
}

const QString CParameterConnecter::GetServerName() const
{
    return m_szServerName;
}

void CParameterConnecter::SetServerName(const QString& name)
{
    if(m_szServerName == name)
        return;
    m_szServerName = name;
    SetModified(true);
}

bool CParameterConnecter::GetShowServerName() const
{
    return m_bShowServerName;
}

void CParameterConnecter::SetShowServerName(bool NewShowServerName)
{
    if (m_bShowServerName == NewShowServerName)
        return;
    m_bShowServerName = NewShowServerName;
    SetModified(true);
    emit sigShowServerNameChanged();
}

void CParameterConnecter::SetHost(const QString& host)
{
    if (m_szHost == host)
        return;
    m_szHost = host;
    SetModified(true);
}

const QString CParameterConnecter::GetHost() const
{
    return m_szHost;
}

const quint16 CParameterConnecter::GetPort() const
{
    return m_nPort;
}

void CParameterConnecter::SetPort(quint16 port)
{
    if(m_nPort == port)
        return;
    m_nPort = port;
    SetModified(true);
}

const QString CParameterConnecter::GetUser() const
{
    return m_szUser;
}

void CParameterConnecter::SetUser(const QString &user)
{
    if (m_szUser == user)
        return;
    m_szUser = user;
    SetModified(true);
}

const QString CParameterConnecter::GetPassword() const
{
    return m_szPassword;
}

void CParameterConnecter::SetPassword(const QString &password)
{
    if(m_szPassword == password)
        return;
    m_szPassword = password;
    SetModified(true);
}

const bool CParameterConnecter::GetSavePassword() const
{
    return m_bSavePassword;
}

void CParameterConnecter::SetSavePassword(bool save)
{
    if (m_bSavePassword == save)
        return;
    m_bSavePassword = save;
    SetModified(true);
}

const bool CParameterConnecter::GetOnlyView() const
{
    return m_bOnlyView;
}

void CParameterConnecter::SetOnlyView(bool only)
{
    if(m_bOnlyView == only)
        return;
    m_bOnlyView = only;
    SetModified(true);
}

const bool CParameterConnecter::GetLocalCursor() const
{
    return m_bLocalCursor;
}

void CParameterConnecter::SetLocalCursor(bool cursor)
{
    if(m_bLocalCursor == cursor)
        return;
    m_bLocalCursor = cursor;
    SetModified(true);
}

const bool CParameterConnecter::GetClipboard() const
{
    return m_bClipboard;
}

void CParameterConnecter::SetClipboard(bool c)
{
    if(m_bClipboard == c)
        return;
    m_bClipboard = c;
    SetModified(true);
}

const CParameterConnecter::emProxy CParameterConnecter::GetProxyType() const
{
    return m_eProxyType;
}

void CParameterConnecter::SetProxyType(emProxy type)
{
    if (m_eProxyType == type)
        return;
    m_eProxyType = type;
    SetModified(true);
}

const QString CParameterConnecter::GetProxyHost() const
{
    return m_szProxyHost;
}

void CParameterConnecter::SetProxyHost(const QString &host)
{
    if(m_szProxyHost == host)
        return;
    m_szProxyHost = host;
    SetModified(true);
}

const quint16 CParameterConnecter::GetProxyPort() const
{
    return m_nProxyPort;
}

void CParameterConnecter::SetProxyPort(quint16 port)
{
    if(m_nProxyPort == port)
        return;
    m_nProxyPort = port;
    SetModified(true);
}

const QString CParameterConnecter::GetProxyUser() const
{
    return m_szProxyUser;
}

void CParameterConnecter::SetProxyUser(const QString &user)
{
    if(m_szProxyUser == user)
        return;
    m_szProxyUser = user;
    SetModified(true);
}

const QString CParameterConnecter::GetProxyPassword() const
{
    return m_szProxyPassword;
}

void CParameterConnecter::SetProxyPassword(const QString &password)
{
    if(m_szProxyPassword == password)
        return;
    m_szProxyPassword = password;
    SetModified(true);
}

int CParameterConnecter::Load(QSettings &set)
{
    SetName(set.value("Name", GetName()).toString());
    SetServerName(set.value("ServerName", GetServerName()).toString());
    SetShowServerName(set.value("ShowServerName", GetShowServerName()).toBool());
    SetHost(set.value("Host", GetHost()).toString());
    SetPort(set.value("Port", GetPort()).toUInt());
    SetUser(set.value("User", GetUser()).toString());
    SetSavePassword(set.value("SavePassword", GetSavePassword()).toBool());
    if(GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Password"), "Password", szPassword, set))
            SetPassword(szPassword);
    }
    SetOnlyView(set.value("OnlyView", GetOnlyView()).toBool());
    SetLocalCursor(set.value("LocalCursor", GetLocalCursor()).toBool());
    SetClipboard(set.value("Clipboard", GetClipboard()).toBool());
    
    SetProxyType(static_cast<emProxy>(set.value("Proxy/Type",
                                    static_cast<int>(GetProxyType())).toInt()));
    SetProxyHost(set.value("Proxy/Host", GetProxyHost()).toString());
    SetProxyPort(set.value("Proxy/Port", GetProxyPort()).toUInt());
    SetProxyUser(set.value("Proxy/User", GetProxyUser()).toString());
    
    if(GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Proxy password"), "Proxy/Password", szPassword, set))
            SetProxyPassword(szPassword);
    }
    
    return 0;
}

int CParameterConnecter::Save(QSettings &set)
{
    set.setValue("Name", GetName());
    set.setValue("ServerName", GetServerName());
    set.setValue("ShowServerName", GetShowServerName());
    set.setValue("Host", GetHost());
    set.setValue("Port", GetPort());
    set.setValue("User", GetUser());
    SavePassword("Password", GetPassword(), set, true);
    set.setValue("OnlyView", GetOnlyView());
    set.setValue("LocalCursor", GetLocalCursor());
    set.setValue("Clipboard", GetClipboard());
    
    set.setValue("Proxy/Type", (int)m_eProxyType);
    set.setValue("Proxy/Host", GetProxyHost());
    set.setValue("Proxy/Port", GetProxyPort());
    set.setValue("Proxy/User", GetProxyUser());
    SavePassword("Proxy/Password", GetProxyPassword(), set);
    
    return 0;
}

QByteArray CParameterConnecter::PasswordSum(const std::string &password,
                                          const std::string &key)
{
    QCryptographicHash sum(QCryptographicHash::Md5);
    sum.addData(password.c_str(), password.length());
    std::string pw = "RabbitRemoteControl";
    sum.addData(pw.c_str(), pw.length());
    if(!key.empty())
        sum.addData(key.c_str(), key.length());
    return sum.result();
}

int CParameterConnecter::LoadPassword(const QString &szTitle,
                                    const QString &szKey,
                                    QString &password,
                                    QSettings &set)
{
    QByteArray sum = set.value(szKey + "_sum").toByteArray();
    QByteArray pwByte = set.value(szKey).toByteArray();
    RabbitCommon::CEncrypt e;

    std::string key;
    if(GetParameterClient())
        key = GetParameterClient()->GetEncryptKey().toStdString().c_str();
    if(!key.empty())
        e.SetPassword(key.c_str());

    if(!e.Dencode(pwByte, password)
            && PasswordSum(password.toStdString(), key) == sum)
            return 0;

    qDebug(Client) << "Password don't dencode or sum is error";
    CDlgInputPassword d(GetParameterClient()->GetViewPassowrd(), szTitle);
    if(QDialog::Accepted != d.exec())
    {
        SetSavePassword(false);
        return -1;
    }

    CDlgInputPassword::InputType t;
    int nRet = d.GetValue(t, password);
    if(nRet) return nRet;
    if(CDlgInputPassword::InputType::Password == t)
        return 0;
    GetParameterClient()->SetEncryptKey(password);
    return LoadPassword(szTitle, szKey, password, set);
}

int CParameterConnecter::SavePassword(const QString &szKey,
                                    const QString &password,
                                    QSettings &set, bool bSave)
{
    if(bSave)
        set.setValue("SavePassword", GetSavePassword());
    if(!GetSavePassword())
    {
        set.remove(szKey);
        set.remove(szKey + "_sum");
        return 0;
    }

    QByteArray encryptPassword;
    RabbitCommon::CEncrypt e;
    std::string key = GetParameterClient()->GetEncryptKey().toStdString();
    if(key.empty())
    {
        switch (GetParameterClient()->GetPromptType()) {
        case CParameterClient::PromptType::First:
        {
            int nCount = GetParameterClient()->GetPromptCount();
            if(nCount >= 1)
                break;
            GetParameterClient()->SetPromptCount(nCount + 1);
        }
        case CParameterClient::PromptType::Always:
        {
            QString szKey;
            CDlgInputPassword::InputType t = CDlgInputPassword::InputType::Encrypt;
            CDlgInputPassword dlg(GetParameterClient()->GetViewPassowrd());
            if(QDialog::Accepted == dlg.exec())
                dlg.GetValue(t, szKey);
            if(CDlgInputPassword::InputType::Encrypt == t)
                GetParameterClient()->SetEncryptKey(szKey);
            break;
        }
        case CParameterClient::PromptType::No:
            break;
        }
    } else
        e.SetPassword(key.c_str());
    e.Encode(GetPassword(), encryptPassword);
    set.setValue(szKey, encryptPassword);
    set.setValue(szKey + "_sum", PasswordSum(GetPassword().toStdString(), key));
    return 0;
}
