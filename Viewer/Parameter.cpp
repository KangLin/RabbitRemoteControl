// Author: Kang Lin <kl222@126.com>

#include "Parameter.h"
#include "RabbitCommonEncrypt.h"
#include "RabbitCommonTools.h"
#include "RabbitCommonLog.h"
#include "ManagePassword.h"
#include "DlgInputPassword.h"
#include <QCryptographicHash>
#include <QMessageBox>

CParameter::CParameter(QObject *parent) : QObject(parent),
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

const QString CParameter::GetName() const
{
    return m_szName;
}

void CParameter::SetName(const QString& name)
{
    if(m_szName == name)
        return;
    m_szName = name;
}

void CParameter::SetHost(const QString& host)
{
    if (m_szHost == host)
        return;
    m_szHost = host;
}

const QString CParameter::GetHost() const
{
    return m_szHost;
}

const quint16 CParameter::GetPort() const
{
    return m_nPort;
}

void CParameter::SetPort(quint16 port)
{
    if(m_nPort == port)
        return;
    m_nPort = port;
}

const QString CParameter::GetUser() const
{
    return m_szUser;
}

void CParameter::SetUser(const QString &user)
{
    if (m_szUser == user)
        return;
    m_szUser = user;
}

const QString CParameter::GetPassword() const
{
    return m_szPassword;
}

void CParameter::SetPassword(const QString &password)
{
    m_szPassword = password;
}

const bool CParameter::GetSavePassword() const
{
    return m_bSavePassword;
}

void CParameter::SetSavePassword(bool save)
{
    if (m_bSavePassword == save)
        return;
    m_bSavePassword = save;
}

const bool CParameter::GetOnlyView() const
{
    return m_bOnlyView;
}

void CParameter::SetOnlyView(bool only)
{
    if(m_bOnlyView == only)
        return;
    m_bOnlyView = only;
}

const bool CParameter::GetLocalCursor() const
{
    return m_bLocalCursor;
}

void CParameter::SetLocalCursor(bool cursor)
{
    if(m_bLocalCursor == cursor)
        return;
    m_bLocalCursor = cursor;
}

const bool CParameter::GetClipboard() const
{
    return m_bClipboard;
}

void CParameter::SetClipboard(bool c)
{
    m_bClipboard = c;
}

const CParameter::emProxy CParameter::GetProxyType() const
{
    return m_eProxyType;
}

void CParameter::SetProxyType(emProxy type)
{
    if (m_eProxyType == type)
        return;
    m_eProxyType = type;
}

const QString CParameter::GetProxyHost() const
{
    return m_szProxyHost;
}

void CParameter::SetProxyHost(const QString &host)
{
    m_szProxyHost = host;
}

const quint16 CParameter::GetProxyPort() const
{
    return m_nProxyPort;
}

void CParameter::SetProxyPort(quint16 port)
{
    m_nProxyPort = port;
}

const QString CParameter::GetProxyUser() const
{
    return m_szProxyUser;
}

void CParameter::SetProxyUser(const QString &user)
{
    m_szProxyUser = user;
}

const QString CParameter::GetProxyPassword() const
{
    return m_szProxyPassword;
}

void CParameter::SetProxyPassword(const QString &password)
{
    m_szProxyPassword = password;
}

int CParameter::Load(QSettings &set)
{
    SetName(set.value("Name", GetName()).toString());
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

int CParameter::Save(QSettings &set)
{
    set.setValue("Name", GetName());
    set.setValue("Host", GetHost());
    set.setValue("Port", GetPort());
    
    set.setValue("User", GetUser());
    set.setValue("SavePassword", GetSavePassword());
    if(GetSavePassword())
        SavePassword("Password", GetPassword(), set);
    else
        set.remove("Password");
    set.setValue("OnlyView", GetOnlyView());
    set.setValue("LocalCursor", GetLocalCursor());
    set.setValue("Clipboard", GetClipboard());
    
    set.setValue("Proxy/Type", (int)m_eProxyType);
    set.setValue("Proxy/Host", GetProxyHost());
    set.setValue("Proxy/Port", GetProxyPort());
    set.setValue("Proxy/User", GetProxyUser());
    if(GetSavePassword())
        SavePassword("Proxy/Password", GetProxyPassword(), set);
    else
        set.remove("Proxy/Password");
    return 0;
}

QByteArray CParameter::PasswordSum(const std::string &password)
{
    QCryptographicHash sum(QCryptographicHash::Md5);
    sum.addData(password.c_str(), password.length());
    std::string pw = "RabbitRemoteControl";
    sum.addData(pw.c_str(), pw.length());
    std::string key =
       CManagePassword::Instance()->GetPassword().toStdString().c_str(); 
    if(!key.empty())
        sum.addData(key.c_str(), key.length());
    return sum.result();
}

int CParameter::LoadPassword(const QString &szTitle, const QString &szKey, QString &password, QSettings &set)
{
    QByteArray sum = set.value(szKey + "_sum").toByteArray();
    QByteArray pwByte = set.value(szKey).toByteArray();
    RabbitCommon::CEncrypt e;
    
    std::string pw =
            CManagePassword::Instance()->GetPassword().toStdString().c_str();
    if(!pw.empty())
    {
        e.SetPassword(pw.c_str());
        if(!e.Dencode(pwByte, password)
                && PasswordSum(password.toStdString()) == sum)
            return 0;
    }
    if(!e.Dencode(pwByte, password)
            && PasswordSum(password.toStdString()) == sum)
        return 0;
    
    LOG_MODEL_DEBUG("CParameter", "Password don't dencode");
    CDlgInputPassword d(szTitle);
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
    CManagePassword::Instance()->SetPassword(password);
    return LoadPassword(szTitle, szKey, password, set);
}

int CParameter::SavePassword(const QString &szKey, const QString &password, QSettings &set)
{
    QByteArray encryptPassword;
    RabbitCommon::CEncrypt e;
    std::string pw = CManagePassword::Instance()->GetPassword().toStdString();
    if(!pw.empty())
        e.SetPassword(pw.c_str());
    e.Encode(GetPassword(), encryptPassword);
    set.setValue(szKey, encryptPassword);
    set.setValue(szKey + "_sum", PasswordSum(GetPassword().toStdString()));  
    return 0;
}
