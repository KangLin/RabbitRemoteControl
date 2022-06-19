// Author: Kang Lin <kl222@126.com>

#include "ParameterServiceTigerVNC.h"
#include <QSettings>
#include "RabbitCommonLog.h"

CParameterServiceTigerVNC::CParameterServiceTigerVNC(QObject *parent)
    : CParameterService(parent)
{
    setPort(5900);
    m_bIce = false;
}

CParameterServiceTigerVNC::~CParameterServiceTigerVNC()
{
    LOG_MODEL_DEBUG("CParameterServiceTigerVNC", "CParameterServiceTigerVNC::~CParameterServiceTigerVNC");
}

int CParameterServiceTigerVNC::Load(const QString& szFile)
{
    int nRet = CParameterService::Load(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    m_bIce = set.value("ICE/Enable", m_bIce).toBool();
    return nRet;
}

int CParameterServiceTigerVNC::Save(const QString& szFile)
{
    int nRet = CParameterService::Save(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    set.setValue("ICE/Enable", m_bIce);
    return nRet;
}

bool CParameterServiceTigerVNC::getIce() const
{
    return m_bIce;
}

void CParameterServiceTigerVNC::setIce(bool newBIce)
{
    if (m_bIce == newBIce)
        return;
    m_bIce = newBIce;
    emit sigIceChanged();
}
