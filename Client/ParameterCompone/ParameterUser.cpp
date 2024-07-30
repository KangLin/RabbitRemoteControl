#include "ParameterUser.h"
#include "RabbitCommonTools.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Parameter.User")

CParameterUser::CParameterUser(CParameterConnecter *parent, const QString &szPrefix)
    : CParameterConnecter(parent, szPrefix),
    m_Type(QList<TYPE>() << TYPE::UserPassword),
    m_UsedType(TYPE::UserPassword),
    m_bSavePassword(false),
    m_bUseSystemFile(true),
    m_bSavePassphrase(false),
    m_cbConvertTypeToName(nullptr)
{
    SetUser(RabbitCommon::CTools::GetCurrentUser());
    m_TypeName = {tr("None"),
                  tr("Password"),
                  tr("Username and password"),
                  tr("Public key")};
}

int CParameterUser::OnLoad(QSettings &set)
{
    set.beginGroup("User");
    SetUser(set.value("Name", GetUser()).toString());
    
    set.beginGroup("Authentication");
    QStringList type;
    foreach(auto t, GetType()) {
        type.push_back(QString::number((int)t));
    }
    QStringList vType;
    vType = set.value("Type", type).toStringList();
    QList<TYPE> lstType;
    foreach(auto s, vType)
    {
        lstType.push_back((TYPE)s.toInt());
    }
    SetType(lstType);
    SetUsedType((TYPE)set.value("Type/Used", (int)GetUsedType()).toInt());
    
    SetSavePassword(set.value("SavePassword", GetSavePassword()).toBool());
    if(GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Password"), "Password", szPassword, set))
            SetPassword(szPassword);
    }
    
    set.beginGroup("PublicKey");
    set.beginGroup("File");
    SetUseSystemFile(set.value("UseSystemFile", GetUseSystemFile()).toBool());
    SetPublicKeyFile(set.value("PublicKey",
                               GetPublicKeyFile()).toString());
    SetPrivateKeyFile(set.value("PrivateKey",
                                GetPrivateKeyFile()).toString());
    SetSavePassphrase(set.value("SavePassphrase", GetSavePassphrase()).toBool());
    if(GetSavePassphrase()) {
        QString szPassword;
        if(!LoadPassword(tr("Passphrase"), "Passphrase", szPassword, set))
            SetPassphrase(szPassword);
    }
    set.endGroup();
    
    set.endGroup();
    set.endGroup();
    set.endGroup();
    return 0;
}

int CParameterUser::OnSave(QSettings &set)
{
    set.beginGroup("User");
    set.setValue("Name", GetUser());
    
    set.beginGroup("Authentication");
    QStringList type;
    foreach(auto t, GetType()) {
        type.push_back(QString::number((int)t));
    }
    set.setValue("Type", type);
    set.setValue("Type/Used", (int)GetUsedType());

    set.setValue("SavePassword", GetSavePassword());
    SavePassword("Password", GetPassword(), set, GetSavePassword());
    
    set.beginGroup("PublicKey");
    set.beginGroup("File");
    set.setValue("UseSystemFile", GetUseSystemFile());
    set.setValue("PublicKey", GetPublicKeyFile());
    set.setValue("PrivateKey", GetPrivateKeyFile());
    set.setValue("SavePassphrase", GetSavePassphrase());
    SavePassword("Passphrase", GetPassphrase(), set, GetSavePassphrase());
    set.endGroup();
    
    set.endGroup();
    set.endGroup();
    set.endGroup();
    return 0;
}

QList<CParameterUser::TYPE> CParameterUser::GetType() const
{
    return m_Type;
}

int CParameterUser::SetType(QList<TYPE> type)
{
    if(m_Type == type)
        return 0;
    m_Type = type;
    SetModified(true);
    return 0;
}

CParameterUser::TYPE CParameterUser::GetUsedType() const
{
    return m_UsedType;
}

int CParameterUser::SetUsedType(TYPE type)
{
    if(m_UsedType == type)
        return 0;
    m_UsedType = type;
    SetModified(true);
    return 0;
}

const QString CParameterUser::GetUser() const
{
    return m_szUser;
}

void CParameterUser::SetUser(const QString &szUser)
{
    if (m_szUser == szUser)
        return;
    m_szUser = szUser;
    SetModified(true);
}

const QString CParameterUser::GetPassword() const
{
    return m_szPassword;
}

void CParameterUser::SetPassword(const QString &szPassword)
{
    if(m_szPassword == szPassword)
        return;
    m_szPassword = szPassword;
    SetModified(true);
}

const bool CParameterUser::GetSavePassword() const
{
    return m_bSavePassword;
}

void CParameterUser::SetSavePassword(bool save)
{
    if (m_bSavePassword == save)
        return;
    m_bSavePassword = save;
    SetModified(true);
}

//! [Initialize parameter after set CParameterClient]
void CParameterUser::slotSetParameterClient()
{
    if(!GetParameterClient()) {
        QString szErr = "The CParameterClient is null";
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "CParameterUser", szErr.toStdString().c_str());
        return;
    }

    if(GetParameterClient())
    {   
        SetSavePassword(GetParameterClient()->GetSavePassword());
        SetSavePassphrase(GetParameterClient()->GetSavePassword());
    }

    return;
}
//! [Initialize parameter after set CParameterClient]

bool CParameterUser::GetUseSystemFile() const
{
    return m_bUseSystemFile;
}

int CParameterUser::SetUseSystemFile(bool use)
{
    if(m_bUseSystemFile == use)
        return 0;
    m_bUseSystemFile = use;
    SetModified(true);
    return 0;
}

QString CParameterUser::GetPassphrase() const
{
    return m_szPassphrase;
}

int CParameterUser::SetPassphrase(const QString passphrase)
{
    if(m_szPassphrase == passphrase)
        return 0;
    m_szPassphrase = passphrase;
    SetModified(true);
    return 0;
}

bool CParameterUser::GetSavePassphrase() const
{
    return m_bSavePassphrase;
}

int CParameterUser::SetSavePassphrase(bool bSave)
{
    if(m_bSavePassphrase == bSave)
        return 0;
    m_bSavePassphrase = bSave;
    SetModified(true);
    return 0;
}

QString CParameterUser::GetPublicKeyFile() const
{
    return m_szPublicKeyFile;
}

int CParameterUser::SetPublicKeyFile(const QString szFile)
{
    if(m_szPublicKeyFile == szFile)
        return 0;
    m_szPublicKeyFile = szFile;
    SetModified(true);
    return 0;
}

QString CParameterUser::GetPrivateKeyFile() const
{
    return m_szPrivateKeyFile;
}

int CParameterUser::SetPrivateKeyFile(const QString szFile)
{
    if(m_szPrivateKeyFile == szFile)
        return 0;
    m_szPrivateKeyFile = szFile;
    SetModified(true);
    return 0;
}

QString CParameterUser::ConvertTypeToName(TYPE t)
{
    QString name;
    if(m_cbConvertTypeToName)
        name = m_cbConvertTypeToName(t);
    if(name.isEmpty())
        name = m_TypeName.at((int)t);
    return name;
}

void CParameterUser::SetConvertTypeToNameCallBack(std::function<QString (TYPE)> cb)
{
    m_cbConvertTypeToName = cb;
}