#include "ParameterProxy.h"

CParameterProxy::CParameterProxy(CParameterOperate *parent, const QString &szPrefix)
    : CParameterOperate(parent, szPrefix)
    , m_SockesV5(this, "Proxy/SockesV5")
    , m_Http(this, "Proxy/Http")
    , m_SSH(this, "Proxy/SSH/Tunnel")
    , m_UsedType(TYPE::None)
{
    m_Type << TYPE::None << TYPE::System << TYPE::SockesV5 << TYPE::Http;
#if HAVA_LIBSSH
    m_Type << TYPE::SSHTunnel;
#endif
    m_TypeName = {{TYPE::None, tr("None")},
                  {TYPE::System, tr("System settings")},
                  {TYPE::SockesV5, tr("Sockes V5")},
                  {TYPE::Http, tr("Http")},
                  {TYPE::SSHTunnel, tr("SSH tunnel")}};

    QList<CParameterUser::TYPE> lstType;
    m_Http.SetPort(80);
    lstType.clear();
    lstType << CParameterUser::TYPE::None
            << CParameterUser::TYPE::UserPassword;
    m_Http.m_User.SetType(lstType);
    m_Http.SetPrompt(tr("The host is empty in \"Proxy->Http\". please set it"));
    
    m_SockesV5.SetPort(1080);
    lstType.clear();
    lstType << CParameterUser::TYPE::None
        << CParameterUser::TYPE::UserPassword;
    m_SockesV5.m_User.SetType(lstType);
    m_SockesV5.SetPrompt(tr("The host is empty in \"Proxy->SockesV5\". please set it"));

    m_SSH.m_Net.SetPort(22);
    lstType.clear();
    lstType << CParameterUser::TYPE::UserPassword
            << CParameterUser::TYPE::PublicKey;
    m_SSH.m_Net.m_User.SetType(lstType);
    m_SSH.m_Net.SetPrompt(tr("The host is empty in \"Proxy->SSH tunnel\". please set it"));
}

int CParameterProxy::OnLoad(QSettings &set)
{
    set.beginGroup("Proxy");
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
    set.endGroup();
    return 0;
}

int CParameterProxy::OnSave(QSettings &set)
{
    set.beginGroup("Proxy");
    QStringList type;
    foreach(auto t, GetType()) {
        type.push_back(QString::number((int)t));
    }
    set.setValue("Type", type);
    set.setValue("Type/Used", (int)GetUsedType());
    set.endGroup();
    return 0;
}

QList<CParameterProxy::TYPE> CParameterProxy::GetType() const
{
    return m_Type;
}

int CParameterProxy::SetType(QList<TYPE> type)
{
    if(m_Type == type)
        return 0;
    m_Type = type;
    SetModified(true);
    return 0;
}

CParameterProxy::TYPE CParameterProxy::GetUsedType() const
{
    return m_UsedType;
}

int CParameterProxy::SetUsedType(TYPE type)
{
    if(m_UsedType == type)
        return 0;
    m_UsedType = type;
    SetModified(true);
    return 0;
}

QString CParameterProxy::ConvertTypeToName(TYPE t)
{
    return m_TypeName[t];
}

int CParameterProxy::SetTypeName(TYPE t, const QString& szName)
{
    m_TypeName[t] = szName;
    return 0;
}
