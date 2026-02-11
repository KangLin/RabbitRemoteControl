#include "ParameterDatabase.h"
#include "ParameterGlobal.h"

CParameterGlobal::CParameterGlobal(QObject *parent, const QString &szPrefix)
    : CParameter{parent, szPrefix}
    , m_pDatabase(nullptr)
    , m_SaveSettingsType(SaveSettingsType::File)
{
    m_pDatabase = new CParameterDatabase(this);
}

CParameterGlobal::~CParameterGlobal()
{
    if(m_pDatabase) {
        delete m_pDatabase;
        m_pDatabase = nullptr;
    }
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
