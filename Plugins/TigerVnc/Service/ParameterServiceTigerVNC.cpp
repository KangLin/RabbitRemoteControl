#include "ParameterServiceTigerVNC.h"
#include <QSettings>
#include "RabbitCommonLog.h"

CParameterServiceTigerVNC::CParameterServiceTigerVNC(QObject *parent)
    : CParameterService(parent)
{
    setPort(5900);
}

int CParameterServiceTigerVNC::OnLoad(const QString& szFile)
{
    int nRet = CParameterService::OnLoad(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    
    return nRet;
}

int CParameterServiceTigerVNC::OnSave(const QString& szFile)
{
    int nRet = CParameterService::OnSave(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    
    return nRet;
}