#include "Parameter.h"

CParameter::CParameter(QObject *parent)
    : QObject(parent)
{}

CParameter::~CParameter()
{}

int CParameter::Load(const QString& szFile)
{
    if(szFile.isEmpty())
        return -1;
    QSettings set(szFile, QSettings::IniFormat);
    return Load(set);
}

int CParameter::Save(const QString& szFile)
{
    if(szFile.isEmpty())
        return -1;
    QSettings set(szFile, QSettings::IniFormat);
    return Save(set);
}
