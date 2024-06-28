#include "Parameter.h"
#include "RabbitCommonDir.h"

CParameter::CParameter(QObject *parent) : QObject(parent), m_bModified(false)
{}

CParameter::~CParameter()
{}

int CParameter::Load(QString szFile)
{
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
    if(szFile.isEmpty()) return -1;
    QSettings set(szFile, QSettings::IniFormat);
    int nRet = Load(set);
    SetModified(false);
    return nRet;
}

int CParameter::Save(QString szFile, bool bForce)
{
    if(!GetModified() && !bForce) return 0;
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
    if(szFile.isEmpty()) return -1;
    QSettings set(szFile, QSettings::IniFormat);
    int nRet = Save(set);
    SetModified(false);
    return nRet;
}

int CParameter::Load(QSettings &set)
{
    return onLoad(set);
}

int CParameter::Save(QSettings &set, bool bForce)
{
    if(GetModified()) emit sigChanged();
    if(!GetModified() && !bForce) return 0;
    return onSave(set);
}

bool CParameter::CheckCompleted()
{
    return true;
}

bool CParameter::GetModified()
{
    return m_bModified;
}

int CParameter::SetModified(bool bModified)
{
    if(m_bModified == bModified)
        return 0;
    m_bModified = bModified;
    return 0;
}
