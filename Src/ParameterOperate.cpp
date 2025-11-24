// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QCryptographicHash>
#include <QInputDialog>

#include "RabbitCommonEncrypt.h"
#include "RabbitCommonTools.h"
#include "DlgInputPassword.h"
#include "ParameterOperate.h"

static Q_LOGGING_CATEGORY(log, "Parameter.Operate")

CParameterOperate::CParameterOperate(QObject *parent, const QString &szPrefix)
    : CParameter(parent, szPrefix)
    , m_Parent(nullptr)
    , m_pParameterPlugin(nullptr)
{
    bool check = false;
    check = connect(this, SIGNAL(sigSetGlobalParameters()),
                    this, SLOT(slotSetGlobalParameters()));
    Q_ASSERT(check);
    CParameterOperate* p = qobject_cast<CParameterOperate*>(parent);
    if(p) {
        m_Parent = p;
        check = connect(m_Parent, SIGNAL(sigSetGlobalParameters()),
                        this, SIGNAL(sigSetGlobalParameters()));
        Q_ASSERT(check);
    }
}

CParameterPlugin* CParameterOperate::GetGlobalParameters()
{
    if(m_Parent) return m_Parent->GetGlobalParameters();
    return m_pParameterPlugin;
}

int CParameterOperate::SetGlobalParameters(CParameterPlugin *p)
{
    if(m_pParameterPlugin == p) return 0;
    m_pParameterPlugin = p;
    emit sigSetGlobalParameters();
    return 0;
}

void CParameterOperate::slotSetGlobalParameters()
{
    return;
}

QByteArray CParameterOperate::PasswordSum(const std::string &password,
                                          const std::string &key)
{
    std::string pw = "RabbitRemoteControl";
    QCryptographicHash sum(QCryptographicHash::Md5);

#if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
    if(!password.empty())
        sum.addData(password.c_str(), password.length());
    sum.addData(pw.c_str(), pw.length());
    if(!key.empty())
        sum.addData(key.c_str(), key.length());
#else
    if(!password.empty())
        sum.addData(QByteArrayView(password.c_str(), password.length()));
    sum.addData(QByteArrayView(pw.c_str(), pw.length()));
    if(!key.empty())
        sum.addData(QByteArrayView(key.c_str(), key.length()));
#endif
    return sum.result();
}

/*!
 * \brief Restore password
 * \details 
 * if the encryption key is incorrect, Password is set to empty.
 * it proceeds to the normal password entry.
 * flow chart: \image html docs/Image/RestorePassword.svg
 * \see https://github.com/KangLin/RabbitRemoteControl/issues/79
 */
int CParameterOperate::LoadPassword(const QString &szTitle,
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
    if(GetGlobalParameters())
        key = GetGlobalParameters()->GetEncryptKey().toStdString();
    if(!key.empty())
        e.SetPassword(key.c_str());
    
    if(!e.Dencode(pwByte, password)
        && PasswordSum(password.toStdString(), key) == sum)
        return 0;

    qDebug(log) << "Password don't dencode or sum is error";
    if(key.empty()) {
        switch (GetGlobalParameters()->GetPromptType()) {
        case CParameterPlugin::PromptType::First:
        {
            int nCount = GetGlobalParameters()->GetPromptCount();
            if(nCount >= 1)
                return -1;
            GetGlobalParameters()->SetPromptCount(nCount + 1);
            break;
        }
        case CParameterPlugin::PromptType::No:
        default:
            return -1;
        }

        CDlgInputPassword d(GetGlobalParameters()->GetViewPassowrd(), false);
        if(QDialog::Accepted != d.exec())
            return -1;

        key = d.GetPassword().toStdString();
        if(key.empty()) return -1;
        
        GetGlobalParameters()->SetEncryptKey(key.c_str());
        return LoadPassword(szTitle, szKey, password, set);
    }
    return -1;
}

int CParameterOperate::SavePassword(const QString &szKey,
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
    std::string key = GetGlobalParameters()->GetEncryptKey().toStdString();
    if(key.empty())
    {
        switch (GetGlobalParameters()->GetPromptType()) {
        case CParameterPlugin::PromptType::First:
        {
            int nCount = GetGlobalParameters()->GetPromptCount();
            if(nCount >= 1)
                break;
            GetGlobalParameters()->SetPromptCount(nCount + 1);
            QString szKey;
            CDlgInputPassword dlg(GetGlobalParameters()->GetViewPassowrd(), true);
            if(QDialog::Accepted != dlg.exec())
                break;
            szKey = dlg.GetPassword();
            GetGlobalParameters()->SetEncryptKey(szKey);
            key = szKey.toStdString();
            break;
        }
        case CParameterPlugin::PromptType::No:
            break;
        }
    }
    if(!key.empty())
        e.SetPassword(key.c_str());
    if(password.isEmpty())
        return 0;
    e.Encode(password, encryptPassword);
    set.setValue(szKey, encryptPassword);
    set.setValue(szKey + "_sum", PasswordSum(password.toStdString(), key));
    return 0;
}

const QString CParameterOperate::GetName() const
{
    return m_szName;
}

void CParameterOperate::SetName(const QString& szName)
{
    if(m_szName == szName)
        return;
    m_szName = szName;
    SetModified(true);
    emit sigNameChanged(m_szName);
}

int CParameterOperate::OnLoad(QSettings &set)
{
    SetName(set.value("Name", GetName()).toString());
    return 0;
}

int CParameterOperate::OnSave(QSettings &set)
{
    set.setValue("Name", GetName());
    return 0;
}
