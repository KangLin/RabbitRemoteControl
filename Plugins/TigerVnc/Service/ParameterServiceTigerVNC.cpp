// Author: Kang Lin <kl222@126.com>

#include "ParameterServiceTigerVNC.h"
#include <QSettings>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(TigerVNC)

CParameterServiceTigerVNC::CParameterServiceTigerVNC(QObject *parent)
    : CParameterService(parent)
{
    setPort(5900);
    m_bIce = false;
    m_bEnableSocket = true;
}

CParameterServiceTigerVNC::~CParameterServiceTigerVNC()
{
    qDebug(TigerVNC) << "CParameterServiceTigerVNC::~CParameterServiceTigerVNC";
}

int CParameterServiceTigerVNC::Load(const QString& szFile)
{
    int nRet = CParameterService::Load(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    m_bIce = set.value("Channel/Enable/ICE", m_bIce).toBool();
    m_bEnableSocket = set.value("Channel/Enable/Socket", GetEnableSocket()).toBool();
    return nRet;
}

int CParameterServiceTigerVNC::Save(const QString& szFile)
{
    int nRet = CParameterService::Save(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    set.setValue("Channel/Enable/ICE", m_bIce);
    set.setValue("Channel/Enable/Socket", m_bEnableSocket);
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

bool CParameterServiceTigerVNC::GetEnableSocket() const
{
    return m_bEnableSocket;
}

void CParameterServiceTigerVNC::SetEnableSocket(bool newSocket)
{
    if (m_bEnableSocket == newSocket)
        return;
    m_bEnableSocket = newSocket;
    emit sigEnableSocketChanged(m_bEnableSocket);
}
