#include <QDir>
#include "ParameterRecord.h"
#include "RabbitCommonDir.h"

CParameterRecord::CParameterRecord(QObject *parent, const QString &szPrefix)
    : CParameter{parent, szPrefix}
    , m_bEnable(true)
{
    m_szPath = RabbitCommon::CDir::Instance()->GetDirUserImage()
        + QDir::separator()
        + "ScreenShot";
    QDir d(m_szPath);
    if(!d.exists())
        d.mkpath(m_szPath);
}

int CParameterRecord::OnLoad(QSettings &set)
{
    return 0;
}

int CParameterRecord::OnSave(QSettings &set)
{
    return 0;
}

const bool CParameterRecord::GetEnable() const
{
    return m_bEnable;
}

int CParameterRecord::SetEnable(bool bEnable)
{
    if(m_bEnable == bEnable)
        return 0;
    m_bEnable = bEnable;
    SetModified(true);
    return 0;
}

const QString CParameterRecord::GetUrl() const
{
    return m_szUrl;
}

int CParameterRecord::SetUrl(const QString &szFile)
{
    if(m_szUrl == szFile)
        return 0;
    SetModified(true);
    m_szUrl = szFile;
    return 0;
}

const QString CParameterRecord::GetPath() const
{
    return m_szPath;
}

int CParameterRecord::SetPath(const QString &szPath)
{
    if(m_szPath == szPath)
        return 0;
    m_szPath = szPath;
    SetModified(true);
    return 0;
}
