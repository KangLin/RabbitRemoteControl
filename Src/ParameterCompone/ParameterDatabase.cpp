// Author: Kang Lin <kl222@126.com>

#include "ParameterDatabase.h"

CParameterDatabase::CParameterDatabase(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_Net(this, "Database")
    , m_szType("QSQLITE")
{
}

int CParameterDatabase::OnLoad(QSettings &set)
{
    set.beginGroup("Database");
    SetType(set.value("Type", GetType()).toString());
    SetDatabaseName(set.value("DatabaseName", GetDatabaseName()).toString());
    SetOptions(set.value("Options", GetOptions()).toString());
    set.endGroup();
    return 0;
}

int CParameterDatabase::OnSave(QSettings &set)
{
    set.beginGroup("Database");
    set.setValue("Type", GetType());
    set.setValue("DatabaseName", GetDatabaseName());
    set.setValue("Options", GetOptions());
    set.endGroup();
    return 0;
}

QString CParameterDatabase::GetType() const
{
    return m_szType;
}

void CParameterDatabase::SetType(const QString &newType)
{
    if(m_szType == newType)
        return;
    m_szType = newType;
    SetModified(true);
}

QString CParameterDatabase::GetDatabaseName() const
{
    return m_szDatabaseName;
}

void CParameterDatabase::SetDatabaseName(const QString &newDatabaseName)
{
    if(m_szDatabaseName == newDatabaseName)
        return;
    m_szDatabaseName = newDatabaseName;
    SetModified(true);
}

QString CParameterDatabase::GetOptions() const
{
    return m_szOptions;
}

void CParameterDatabase::SetOptions(const QString &newOptions)
{
    if(m_szOptions == newOptions)
        return;
    m_szOptions = newOptions;
    SetModified(true);
}
