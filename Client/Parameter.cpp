#include "Parameter.h"
#include "RabbitCommonDir.h"

CParameter::CParameter(QObject *parent) : QObject(parent)
{}

CParameter::~CParameter()
{}

int CParameter::Load(QString szFile)
{
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
    if(szFile.isEmpty()) return -1;
    QSettings set(szFile, QSettings::IniFormat);
    return Load(set);
}

int CParameter::Save(QString szFile)
{
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
    if(szFile.isEmpty()) return -1;
    QSettings set(szFile, QSettings::IniFormat);
    return Save(set);
}
