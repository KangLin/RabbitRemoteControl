#include "ParameterProxy.h"

CParameterProxy::CParameterProxy(CParameterConnecter *parent, const QString &szPrefix)
    : CParameterConnecter(parent, szPrefix),
    m_Type(TYPE::No),
    m_Socket5(parent, "Proxy/Socket5")
{
    m_Socket5.SetPort(1080);
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