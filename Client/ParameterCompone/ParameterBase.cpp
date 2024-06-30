#include "ParameterBase.h"

CParameterBase::CParameterBase(QObject* parent)
    : CParameterConnecter(parent),
    m_Net(this)
{}

CParameterBase::CParameterBase(CParameterConnecter* parent,
                               const QString& szPrefix)
    : CParameterConnecter(parent, szPrefix),
    m_Net(this)
{}


const QString CParameterBase::GetName() const
{
    return m_szName;
}

void CParameterBase::SetName(const QString& szName)
{
    if(m_szName == szName)
        return;
    m_szName = szName;
    SetModified(true);
    emit sigNameChanged(m_szName);
}

int CParameterBase::onLoad(QSettings &set)
{
    SetName(set.value("Name", GetName()).toString());
    return 0;
}

int CParameterBase::onSave(QSettings &set)
{
    set.setValue("Name", GetName());
    return 0;
}
