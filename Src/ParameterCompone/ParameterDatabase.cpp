// Author: Kang Lin <kl222@126.com>

#include <QSqlDatabase>
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

QSet<QString> CParameterDatabase::GetSupportDatabase()
{
    QSet<QString> supportDrivers;
    supportDrivers << "QSQLITE" << "QMYSQL" << "QODBC";
    return supportDrivers;
}

const QString CParameterDatabase::Details() const
{
    auto supportDrivers = GetSupportDatabase();
    QString szDetail = "## " + tr("Database drivers") + "\n";
    foreach(auto d, QSqlDatabase::drivers()){
        szDetail += "  - " + d;
        if(GetType() == d) {
            szDetail += " (" + tr("Current used") + ")";
        } else if(supportDrivers.contains(d))
            szDetail += " (" + tr("Supported") + ")";
        szDetail += "\n";
    }
    return szDetail;
}

CParameterDatabase& CParameterDatabase::operator=(const CParameterDatabase &in)
{
    m_Net = in.m_Net;

    m_szType = in.m_szType;
    m_szDatabaseName = in.m_szDatabaseName;
    m_szOptions = in.m_szOptions;

    return *this;
}
