#include "ParameterDatabase.h"
#include "ParameterGlobal.h"

CParameterGlobal::CParameterGlobal(QObject *parent, const QString &szPrefix)
    : CParameter{parent, szPrefix}
    , m_Database(this)
    , m_SaveSettingsType(SaveSettingsType::File)
{
}

CParameterGlobal::~CParameterGlobal()
{
}

int CParameterGlobal::OnLoad(QSettings &set)
{
    SetSaveSettingsType((SaveSettingsType)set.value("SaveSettingsType", GetSaveSettingsType()).toInt());
    return 0;
}

int CParameterGlobal::OnSave(QSettings &set)
{
    set.setValue("SaveSettingsType", GetSaveSettingsType());
    return 0;
}


CParameterGlobal::SaveSettingsType CParameterGlobal::GetSaveSettingsType() const
{
    return m_SaveSettingsType;
}

void CParameterGlobal::SetSaveSettingsType(const SaveSettingsType &type)
{
    if(m_SaveSettingsType == type)
        return;
    m_SaveSettingsType = type;
    SetModified(true);
}
