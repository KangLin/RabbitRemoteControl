// Author: Kang Lin <kl222@126.com>
#include "Stats.h"

CStats::CStats(CParameterOperate *parent, const QString &szPrefix)
    : CParameterOperate{parent}
{}

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

void CStats::AddSends(quint64 size)
{
    m_TotalSends += size;
}

void CStats::AddReceives(quint64 size)
{
    m_TotalReceives += size;
}

int CStats::OnLoad(QSettings &set)
{
    return 0;
}

int CStats::OnSave(QSettings &set)
{
    return 0;
}
