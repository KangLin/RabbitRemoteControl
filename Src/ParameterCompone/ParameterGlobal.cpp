#include "RabbitCommonEncrypt.h"
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

    QByteArray baRandom = set.value("Password/Random").toByteArray();
    QString szRandom;
    if(!baRandom.isEmpty()) {
        RabbitCommon::CEncrypt e;
        e.Dencode(baRandom, szRandom);
        m_szRandom = szRandom.toStdString().c_str();
    }

    return 0;
}

int CParameterGlobal::OnSave(QSettings &set)
{
    set.setValue("SaveSettingsType", GetSaveSettingsType());

    RabbitCommon::CEncrypt e;
    QByteArray baRandom;
    e.Encode(GetRandom().c_str(), baRandom);
    set.setValue("Password/Random", baRandom);

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
    emit sigSaveSettingsTypeChanged();
}

std::string CParameterGlobal::GetRandom()
{
    if(m_szRandom.empty()) {
        m_szRandom = RabbitCommon::CPasswordGenerator::GeneratePassword();
    }
    return m_szRandom;
}
