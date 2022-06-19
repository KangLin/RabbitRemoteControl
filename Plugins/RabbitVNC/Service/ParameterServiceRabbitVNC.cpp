// Author: Kang Lin <kl222@126.com>

#include "ParameterServiceRabbitVNC.h"
#include <QSettings>
#include "RabbitCommonLog.h"

CParameterServiceRabbitVNC::CParameterServiceRabbitVNC(QObject *parent)
    : CParameterService(parent)
{
    setPort(5900);
    m_bIce = false;
}

CParameterServiceRabbitVNC::~CParameterServiceRabbitVNC()
{
    LOG_MODEL_DEBUG("CParameterServiceTigerVNC", "CParameterServiceTigerVNC::~CParameterServiceTigerVNC");
}

int CParameterServiceRabbitVNC::Load(const QString& szFile)
{
    int nRet = CParameterService::Load(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    m_bIce = set.value("ICE/Enable", m_bIce).toBool();
    return nRet;
}

int CParameterServiceRabbitVNC::Save(const QString& szFile)
{
    int nRet = CParameterService::Save(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    set.setValue("ICE/Enable", m_bIce);
    set.sync();
    return nRet;
}

bool CParameterServiceRabbitVNC::getIce() const
{
    return m_bIce;
}

void CParameterServiceRabbitVNC::setIce(bool newBIce)
{
    if (m_bIce == newBIce)
        return;
    m_bIce = newBIce;
    emit sigIceChanged();
}
