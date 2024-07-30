#include "ParameterProxy.h"

CParameterProxy::CParameterProxy(CParameterConnecter *parent, const QString &szPrefix)
    : CParameterConnecter(parent, szPrefix),
    m_Type(TYPE::None),
    m_SockesV5(this, "Proxy/SockesV5"),
    m_SSH(this, "Proxy/SSH/Tunnel")
{
    m_SockesV5.SetPort(1080);
    QList<CParameterUser::TYPE> lstType;
    lstType << CParameterUser::TYPE::None
        << CParameterUser::TYPE::UserPassword;
    m_SockesV5.m_User.SetType(lstType);

    m_SSH.SetPort(22);
    lstType.clear();
    lstType << CParameterUser::TYPE::UserPassword
            << CParameterUser::TYPE::PublicKey;
    m_SSH.m_User.SetType(lstType);
}

int CParameterProxy::OnLoad(QSettings &set)
{
    set.beginGroup("Proxy");
    SetType((TYPE)set.value("Type", (int)GetType()).toInt());
    set.endGroup();
    return 0;
}

int CParameterProxy::OnSave(QSettings &set)
{
    set.beginGroup("Proxy");
    set.setValue("Type", (int)GetType());
    set.endGroup();
    return 0;
}

CParameterProxy::TYPE CParameterProxy::GetType() const
{
    return m_Type;
}

int CParameterProxy::SetType(TYPE type)
{
    if(m_Type == type)
        return 0;
    m_Type = type;
    SetModified(true);
    return 0;
}