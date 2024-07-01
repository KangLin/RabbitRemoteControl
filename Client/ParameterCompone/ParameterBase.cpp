#include "ParameterBase.h"

CParameterBase::CParameterBase(QObject* parent)
    : CParameterConnecter(parent),
    m_Net(this)
{
    Init();
}

CParameterBase::CParameterBase(CParameterConnecter* parent,
                               const QString& szPrefix)
    : CParameterConnecter(parent, szPrefix),
    m_Net(this)
{
    Init();
}

int CParameterBase::Init()
{
    m_bShowServerName = true;
    m_bOnlyView = false;
    m_bLocalCursor = true;
    m_bClipboard = true;
    return 0;
}

int CParameterBase::onLoad(QSettings &set)
{
    SetName(set.value("Name", GetName()).toString());
    SetServerName(set.value("ServerName", GetServerName()).toString());
    SetShowServerName(set.value("ShowServerName", GetShowServerName()).toBool());
    SetOnlyView(set.value("OnlyView", GetOnlyView()).toBool());
    SetLocalCursor(set.value("LocalCursor", GetLocalCursor()).toBool());
    SetClipboard(set.value("Clipboard", GetClipboard()).toBool());
    return 0;
}

int CParameterBase::onSave(QSettings &set)
{
    set.setValue("Name", GetName());
    set.setValue("ServerName", GetServerName());
    set.setValue("ShowServerName", GetShowServerName());
    set.setValue("OnlyView", GetOnlyView());
    set.setValue("LocalCursor", GetLocalCursor());
    set.setValue("Clipboard", GetClipboard());
    return 0;
}

const QString CParameterBase::GetName() const
{
    return m_szName;
}

void CParameterBase::SetName(const QString& szName)
{
    if(m_szName == szName)
        return;
    m_szName = szName;
    SetModified(true);
    emit sigNameChanged(m_szName);
}

const QString CParameterBase::GetServerName() const
{
    return m_szServerName;
}

void CParameterBase::SetServerName(const QString& szName)
{
    if(m_szServerName == szName)
        return;
    m_szServerName = szName;
    SetModified(true);
}

bool CParameterBase::GetShowServerName() const
{
    return m_bShowServerName;
}

void CParameterBase::SetShowServerName(bool NewShowServerName)
{
    if (m_bShowServerName == NewShowServerName)
        return;
    m_bShowServerName = NewShowServerName;
    SetModified(true);
    emit sigShowServerNameChanged();
}

const bool CParameterBase::GetOnlyView() const
{
    return m_bOnlyView;
}

void CParameterBase::SetOnlyView(bool only)
{
    if(m_bOnlyView == only)
        return;
    m_bOnlyView = only;
    SetModified(true);
}

const bool CParameterBase::GetLocalCursor() const
{
    return m_bLocalCursor;
}

void CParameterBase::SetLocalCursor(bool cursor)
{
    if(m_bLocalCursor == cursor)
        return;
    m_bLocalCursor = cursor;
    SetModified(true);
}

const bool CParameterBase::GetClipboard() const
{
    return m_bClipboard;
}

void CParameterBase::SetClipboard(bool c)
{
    if(m_bClipboard == c)
        return;
    m_bClipboard = c;
    SetModified(true);
}
