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
      m_pParameterClient(nullptr)
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

QByteArray CParameterConnecter::PasswordSum(const std::string &password,
                                          const std::string &key)
{
    QCryptographicHash sum(QCryptographicHash::Md5);
    if(!password.empty())
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
    if(pwByte.isEmpty())
        return 0;
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
    if(password.isEmpty())
        return 0;
    e.Encode(password, encryptPassword);
    set.setValue(szKey, encryptPassword);
    set.setValue(szKey + "_sum", PasswordSum(password.toStdString(), key));
    return 0;
}
