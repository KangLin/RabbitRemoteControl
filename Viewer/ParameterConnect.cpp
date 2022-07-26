// Author: Kang Lin <kl222@126.com>

#include "ParameterConnect.h"
#include "RabbitCommonEncrypt.h"
#include "RabbitCommonTools.h"
#include "RabbitCommonLog.h"
#include "ManagePassword.h"
#include "DlgInputPassword.h"
#include <QCryptographicHash>
#include <QInputDialog>

CParameterConnect::CParameterConnect(QObject *parent) : QObject(parent),
    m_bShowServerName(true),
    m_nPort(0),
    m_bSavePassword(CManagePassword::Instance()->GetSavePassword()),
    m_bOnlyView(false),
    m_bLocalCursor(true),
    m_bClipboard(true),
    m_eProxyType(emProxy::No),
    m_nProxyPort(1080)
{
    SetUser(RabbitCommon::CTools::GetCurrentUser());
}

bool CParameterConnect::GetCheckCompleted()
{
    if(GetSavePassword()) return true;
    return false;
}

const QString CParameterConnect::GetName() const
{
    return m_szName;
}

void CParameterConnect::SetName(const QString& name)
{
    if(m_szName == name)
        return;
    m_szName = name;
    emit sigNameChanged(m_szName);
}

const QString CParameterConnect::GetServerName() const
{
    return m_szServerName;
}

void CParameterConnect::SetServerName(const QString& name)
{
    m_szServerName = name;
}

bool CParameterConnect::GetShowServerName() const
{
    return m_bShowServerName;
}

void CParameterConnect::SetShowServerName(bool NewShowServerName)
{
    if (m_bShowServerName == NewShowServerName)
        return;
    m_bShowServerName = NewShowServerName;
    emit sigShowServerNameChanged();
}

void CParameterConnect::SetHost(const QString& host)
{
    if (m_szHost == host)
        return;
    m_szHost = host;
}

const QString CParameterConnect::GetHost() const
{
    return m_szHost;
}

const quint16 CParameterConnect::GetPort() const
{
    return m_nPort;
}

void CParameterConnect::SetPort(quint16 port)
{
    if(m_nPort == port)
        return;
    m_nPort = port;
}

const QString CParameterConnect::GetUser() const
{
    return m_szUser;
}

void CParameterConnect::SetUser(const QString &user)
{
    if (m_szUser == user)
        return;
    m_szUser = user;
}

const QString CParameterConnect::GetPassword() const
{
    return m_szPassword;
}

void CParameterConnect::SetPassword(const QString &password)
{
    m_szPassword = password;
}

const bool CParameterConnect::GetSavePassword() const
{
    return m_bSavePassword;
}

void CParameterConnect::SetSavePassword(bool save)
{
    if (m_bSavePassword == save)
        return;
    m_bSavePassword = save;
}

const bool CParameterConnect::GetOnlyView() const
{
    return m_bOnlyView;
}

void CParameterConnect::SetOnlyView(bool only)
{
    if(m_bOnlyView == only)
        return;
    m_bOnlyView = only;
}

const bool CParameterConnect::GetLocalCursor() const
{
    return m_bLocalCursor;
}

void CParameterConnect::SetLocalCursor(bool cursor)
{
    if(m_bLocalCursor == cursor)
        return;
    m_bLocalCursor = cursor;
}

const bool CParameterConnect::GetClipboard() const
{
    return m_bClipboard;
}

void CParameterConnect::SetClipboard(bool c)
{
    m_bClipboard = c;
}

const CParameterConnect::emProxy CParameterConnect::GetProxyType() const
{
    return m_eProxyType;
}

void CParameterConnect::SetProxyType(emProxy type)
{
    if (m_eProxyType == type)
        return;
    m_eProxyType = type;
}

const QString CParameterConnect::GetProxyHost() const
{
    return m_szProxyHost;
}

void CParameterConnect::SetProxyHost(const QString &host)
{
    m_szProxyHost = host;
}

const quint16 CParameterConnect::GetProxyPort() const
{
    return m_nProxyPort;
}

void CParameterConnect::SetProxyPort(quint16 port)
{
    m_nProxyPort = port;
}

const QString CParameterConnect::GetProxyUser() const
{
    return m_szProxyUser;
}

void CParameterConnect::SetProxyUser(const QString &user)
{
    m_szProxyUser = user;
}

const QString CParameterConnect::GetProxyPassword() const
{
    return m_szProxyPassword;
}

void CParameterConnect::SetProxyPassword(const QString &password)
{
    m_szProxyPassword = password;
}

int CParameterConnect::Load(QSettings &set)
{
    SetName(set.value("Name", GetName()).toString());
    SetName(set.value("ServerName", GetServerName()).toString());
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

int CParameterConnect::Save(QSettings &set)
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

QByteArray CParameterConnect::PasswordSum(const std::string &password)
{
    QCryptographicHash sum(QCryptographicHash::Md5);
    sum.addData(password.c_str(), password.length());
    std::string pw = "RabbitRemoteControl";
    sum.addData(pw.c_str(), pw.length());
    std::string key =
       CManagePassword::Instance()->GetEncryptKey().toStdString().c_str(); 
    if(!key.empty())
        sum.addData(key.c_str(), key.length());
    return sum.result();
}

int CParameterConnect::LoadPassword(const QString &szTitle, const QString &szKey, QString &password, QSettings &set)
{
    QByteArray sum = set.value(szKey + "_sum").toByteArray();
    QByteArray pwByte = set.value(szKey).toByteArray();
    RabbitCommon::CEncrypt e;
    
    std::string pw =
            CManagePassword::Instance()->GetEncryptKey().toStdString().c_str();
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
    
    LOG_MODEL_DEBUG("CParameterConnect", "Password don't dencode");
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
    CManagePassword::Instance()->SetEncryptKey(password);
    return LoadPassword(szTitle, szKey, password, set);
}

int CParameterConnect::SavePassword(const QString &szKey, const QString &password, QSettings &set, bool bSave)
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
    std::string pw = CManagePassword::Instance()->GetEncryptKey().toStdString();
    if(pw.empty())
    {
        switch (CManagePassword::Instance()->GetPromptType()) {
        case CManagePassword::PromptType::First:
            if(CManagePassword::Instance()->GetPromptCount() >= 1)
                break;
            CManagePassword::Instance()->SetPromptCount(
                        CManagePassword::Instance()->GetPromptCount() + 1);
        case CManagePassword::PromptType::Always:
        {
            QString szKey;
            CDlgInputPassword::InputType t = CDlgInputPassword::InputType::Encrypt;
            CDlgInputPassword dlg;
            if(QDialog::Accepted == dlg.exec())
                dlg.GetValue(t, szKey);
            if(CDlgInputPassword::InputType::Encrypt == t)
                CManagePassword::Instance()->SetEncryptKey(szKey);
            break;
        }
        case CManagePassword::PromptType::No:
            break;
        }
    } else
        e.SetPassword(pw.c_str());
    e.Encode(GetPassword(), encryptPassword);
    set.setValue(szKey, encryptPassword);
    set.setValue(szKey + "_sum", PasswordSum(GetPassword().toStdString()));  
    return 0;
}