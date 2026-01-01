#include "ParameterFilter.h"

CParameterFilter::CParameterFilter(QObject *parent, const QString &szPrefix)
    : CParameter{parent, szPrefix}
{}

bool CParameterFilter::contains(const QString &szKey)
{
    if(m_Key.contains(szKey))
        return true;
    return false;
}

int CParameterFilter::AddKey(const QString& szKey)
{
    m_Key.insert(szKey);
    return 0;
}

int CParameterFilter::RemoveKey(const QString& szKey)
{
    m_Key.remove(szKey);
    return 0;
}

int CParameterFilter::Clear()
{
    m_Key.clear();
    return 0;
}

int CParameterFilter::OnLoad(QSettings &set)
{
    QStringList s = set.value("Key").toStringList();
    foreach(auto k, s) {
        AddKey(k);
    }
    return 0;
}

int CParameterFilter::OnSave(QSettings &set)
{
    QStringList s;
    foreach (auto k, m_Key) {
        s << k;
    }
    set.setValue("Key", s);
    return 0;
}
