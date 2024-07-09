#include "ParameterProxy.h"

CParameterProxy::CParameterProxy(CParameterConnecter *parent, const QString &szPrefix)
    : CParameterConnecter(parent, szPrefix),
    m_Type(TYPE::None),
    m_Sockes(this, "Proxy/SockesV5"),
    m_SSH(this, "Proxy/SSH/Tunnel")
{
    m_Sockes.SetPort(1080);
    m_Sockes.m_User.SetType(
        (CParameterUser::TYPE)((int)CParameterUser::TYPE::None
                               | (int)CParameterUser::TYPE::Password));

    m_SSH.SetPort(22);
    m_SSH.m_User.SetType(
        (CParameterUser::TYPE)((int)CParameterUser::TYPE::Password
                               | (int)CParameterUser::TYPE::PublicKey));
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