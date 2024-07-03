// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QCryptographicHash>
#include <QInputDialog>

#include "RabbitCommonEncrypt.h"
#include "RabbitCommonTools.h"
#include "DlgInputPassword.h"
#include "ParameterConnecter.h"

static Q_LOGGING_CATEGORY(log, "Client.Parameter.Connecter")

CParameterConnecter::CParameterConnecter(QObject *parent, const QString &szPrefix)
    : CParameter(parent, szPrefix),
      m_Parent(nullptr),
      m_pParameterClient(nullptr),
      m_eProxyType(emProxy::No),
      m_nProxyPort(1080)
{
    bool check = false;
    check = connect(this, SIGNAL(sigSetParameterClient()),
                    this, SLOT(slotSetParameterClient()));
    Q_ASSERT(check);
    CParameterConnecter* p = qobject_cast<CParameterConnecter*>(parent);
    if(p) {
        m_Parent = p;
        check = connect(m_Parent, SIGNAL(sigSetParameterClient()),
                        this, SIGNAL(sigSetParameterClient()));
        Q_ASSERT(check);
    }
}

CParameterClient* CParameterConnecter::GetParameterClient()
{
    if(m_Parent) return m_Parent->GetParameterClient();
    return m_pParameterClient;
}

int CParameterConnecter::SetParameterClient(CParameterClient *p)
{
    if(m_pParameterClient == p) return 0;
    m_pParameterClient = p;
    emit sigSetParameterClient();
    return 0;
}

void CParameterConnecter::slotSetParameterClient()
{
    return;
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

int CParameterConnecter::OnLoad(QSettings &set)
{
    SetProxyType(static_cast<emProxy>(set.value("Proxy/Type",
                                    static_cast<int>(GetProxyType())).toInt()));
    SetProxyHost(set.value("Proxy/Host", GetProxyHost()).toString());
    SetProxyPort(set.value("Proxy/Port", GetProxyPort()).toUInt());
    SetProxyUser(set.value("Proxy/User", GetProxyUser()).toString());

    return 0;
}

int CParameterConnecter::OnSave(QSettings &set)
{
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

    qDebug(log) << "Password don't dencode or sum is error";
    CDlgInputPassword d(GetParameterClient()->GetViewPassowrd(), szTitle);
    if(QDialog::Accepted != d.exec())
    {
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
    if(!bSave)
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
    e.Encode(password, encryptPassword);
    set.setValue(szKey, encryptPassword);
    set.setValue(szKey + "_sum", PasswordSum(password.toStdString(), key));
    return 0;
}
