// Author: Kang Lin <kl222@126.com>
#include "Stats.h"

CStats::CStats(CParameterOperate *parent, const QString &szPrefix)
    : CParameterOperate{parent}
    , m_tmInterval(1)
    , m_lastTime(QDateTime::currentDateTime())
    , m_dbSendRate(0)
    , m_dbReceiveRate(0)
{
    SetInterval();
}

QString CStats::Convertbytes(quint64 bytes)
{
    QString szBytes;
    if(1024 >= bytes)
        szBytes = QString::number(bytes) + " " + tr("B");
    else if(1024* 1024 >= bytes)
        szBytes = QString::number(bytes / 1024.0, 'f', 2) + " " + tr("KB");
    else if(1024 * 1024 * 1024 >= bytes)
        szBytes = QString::number(bytes / (1024.0 * 1024.0), 'f', 2) + " " + tr("MB");
    else
        szBytes = QString::number(bytes / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " " + tr(" GB");
    return szBytes;
}

QString CStats::TotalSends()
{
    return Convertbytes(GetTotalSends());
}

QString CStats::TotalReceives()
{
    return Convertbytes(GetTotalReceives());
}

quint64 CStats::GetTotalSends()
{
    return m_TotalSends;
}

quint64 CStats::GetTotalReceives()
{
    return m_TotalReceives;
}

void CStats::AddSends(quint64 size)
{
    m_TotalSends += size;
}

void CStats::AddReceives(quint64 size)
{
    m_TotalReceives += size;
}

QString CStats::SendRate()
{
    return Convertbytes(GetSendRate()) + "/" + tr("S");
}

QString CStats::ReceiveRate()
{
    return Convertbytes(GetReceiveRate()) + "/" + tr("S");
}

double CStats::GetSendRate()
{
    return m_dbSendRate;
}

double CStats::GetReceiveRate()
{
    return m_dbReceiveRate;
}

int CStats::GetInterval()
{
    return m_tmInterval;
}

int CStats::SetInterval(int interval)
{
    if(m_tmInterval == interval)
        return m_tmInterval;
    int old = interval;
    m_tmInterval = interval;
    SetModified(true);
    return old;
}

void CStats::slotCalculating()
{
    if(m_lastTime.secsTo(QDateTime::currentDateTime()) < m_tmInterval)
        return;

    m_dbSendRate = (double)(m_TotalSends - m_lastSends) / (double)m_tmInterval;
    m_lastSends = m_TotalSends;
    m_dbReceiveRate = (double)(m_TotalReceives - m_lastReceives) / (double)m_tmInterval;
    m_lastReceives = m_TotalReceives;
}

int CStats::OnLoad(QSettings &set)
{
    return 0;
}

int CStats::OnSave(QSettings &set)
{
    return 0;
}
