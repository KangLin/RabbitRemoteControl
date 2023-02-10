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
    m_bModified = false;
    return nRet;
}

int CParameter::Save(QString szFile)
{
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
    if(szFile.isEmpty()) return -1;
    QSettings set(szFile, QSettings::IniFormat);
    int nRet = Save(set);
    m_bModified = false;
    return nRet;
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
