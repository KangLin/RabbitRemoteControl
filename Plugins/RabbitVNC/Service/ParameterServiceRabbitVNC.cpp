// Author: Kang Lin <kl222@126.com>

#include "ParameterServiceRabbitVNC.h"
#include <QSettings>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(RabbitVNC)

CParameterServiceRabbitVNC::CParameterServiceRabbitVNC(QObject *parent)
    : CParameterService(parent)
{
    setPort(5900);
    m_bIce = false;
    m_bEnableSocket = true;
}

CParameterServiceRabbitVNC::~CParameterServiceRabbitVNC()
{
    qDebug(RabbitVNC) << "CParameterServiceTigerVNC::~CParameterServiceTigerVNC";
}

int CParameterServiceRabbitVNC::Load(const QString& szFile)
{
    int nRet = CParameterService::Load(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    m_bIce = set.value("Channel/Enable/ICE", m_bIce).toBool();
    m_bEnableSocket = set.value("Channel/Enable/Socket", GetEnableSocket()).toBool();
    return nRet;
}

int CParameterServiceRabbitVNC::Save(const QString& szFile)
{
    int nRet = CParameterService::Save(szFile);
    QSettings set(szFile, QSettings::IniFormat);
    set.setValue("Channel/Enable/ICE", m_bIce);
    set.setValue("Channel/Enable/Socket", m_bEnableSocket);
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

bool CParameterServiceRabbitVNC::GetEnableSocket() const
{
    return m_bEnableSocket;
}

void CParameterServiceRabbitVNC::SetEnableSocket(bool newSocket)
{
    if (m_bEnableSocket == newSocket)
        return;
    m_bEnableSocket = newSocket;
    emit sigEnableSocketChanged(m_bEnableSocket);
}
