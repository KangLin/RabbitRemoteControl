#include "ParameterHistory.h"

CParameterHistory::CParameterHistory(QObject *parent, const QString &szPrefix)
    : CParameter{parent, szPrefix}
    , m_Limit(100)
    , m_WindowSize(600, 450)
{}

int CParameterHistory::GetLimit() const
{
    return m_Limit;
}

void CParameterHistory::SetLimit(int newLimit)
{
    if(m_Limit == newLimit)
        return;
    m_Limit = newLimit;
    SetModified(true);
}

QSize CParameterHistory::GetWindowSize() const
{
    return m_WindowSize;
}

void CParameterHistory::SetWindowSize(const QSize &newWindowSize)
{
    m_WindowSize = newWindowSize;
}

int CParameterHistory::OnLoad(QSettings &set)
{
    SetLimit(set.value("Limit", GetLimit()).toInt());
    SetWindowSize(set.value("WindowSize", GetWindowSize()).toSize());
    return 0;
}

int CParameterHistory::OnSave(QSettings &set)
{
    set.setValue("Limit", GetLimit());
    set.setValue("WindowSize", GetWindowSize());
    return 0;
}
