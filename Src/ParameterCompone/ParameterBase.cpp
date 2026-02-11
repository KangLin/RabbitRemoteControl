// Author: Kang Lin <kl222@126.com>

#include "ParameterBase.h"
#include "ParameterPlugin.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Parameter.Base")

CParameterBase::CParameterBase(QObject* parent)
    : CParameterOperate(parent)
    , m_Net(this)
    , m_Proxy(this)
    , m_Record(this)
    , m_bEnableLocalInputMethod(false)
{
    Init();
}

CParameterBase::CParameterBase(CParameterOperate* parent,
                               const QString& szPrefix)
    : CParameterOperate(parent, szPrefix)
    , m_Net(this)
    , m_Proxy(this)
    , m_Record(this)
{
    Init();
}

int CParameterBase::Init()
{
    m_bShowServerName = true;
    m_bOnlyView = false;
    m_bLocalCursor = true;
    m_bCursorPosition = true;
    m_bClipboard = true;
    m_bSupportsDesktopResize = true;
    m_bLedState = true;
    m_AdaptWindows = CFrmViewer::ADAPT_WINDOWS::ZoomToWindow;
    m_dbZoomFactor = 1;
    return 0;
}

int CParameterBase::OnLoad(QSettings &set)
{
    SetServerName(set.value("ServerName", GetServerName()).toString());
    SetShowServerName(set.value("ShowServerName", GetShowServerName()).toBool());
    SetOnlyView(set.value("OnlyView", GetOnlyView()).toBool());
    SetLocalCursor(set.value("LocalCursor", GetLocalCursor()).toBool());
    SetCursorPosition(set.value("CursorPosition", GetCursorPosition()).toBool());
    SetClipboard(set.value("Clipboard", GetClipboard()).toBool());
    SetSupportsDesktopResize(set.value("SupportsDesktopResize",
                                       GetSupportsDesktopResize()).toBool());
    SetLedState(set.value("LedState", GetLedState()).toBool());
    SetZoomFactor(set.value("Viewer/ZoomFactor", GetZoomFactor()).toDouble());
    SetAdaptWindows(
        (CFrmViewer::ADAPT_WINDOWS)
        set.value("Viewer/AdaptType",
                  (int)GetGlobalParameters()->GetAdaptWindows()).toInt());
    SetEnableLocalInputMethod(set.value("LocalInputMethod", GetEnableLocalInputMethod()).toBool());
    return CParameterOperate::OnLoad(set);
}

int CParameterBase::OnSave(QSettings &set)
{
    set.setValue("ServerName", GetServerName());
    set.setValue("ShowServerName", GetShowServerName());
    set.setValue("OnlyView", GetOnlyView());
    set.setValue("LocalCursor", GetLocalCursor());
    set.setValue("CursorPosition", GetCursorPosition());
    set.setValue("Clipboard", GetClipboard());
    set.setValue("SupportsDesktopResize", GetSupportsDesktopResize());
    set.setValue("LedState", GetLedState());
    set.setValue("Viewer/ZoomFactor", GetZoomFactor());
    set.setValue("Viewer/AdaptType", (int)GetAdaptWindows());
    set.setValue("LocalInputMethod", GetEnableLocalInputMethod());
    return CParameterOperate::OnSave(set);
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

bool CParameterBase::GetOnlyView() const
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

const bool CParameterBase::GetCursorPosition() const
{
    return m_bCursorPosition;
}

void CParameterBase::SetCursorPosition(bool pos)
{
    if(m_bCursorPosition == pos)
        return;
    m_bCursorPosition = pos;
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

bool CParameterBase::GetSupportsDesktopResize() const
{
    return m_bSupportsDesktopResize;
}

void CParameterBase::SetSupportsDesktopResize(bool newSupportsDesktopResize)
{
    if(m_bSupportsDesktopResize == newSupportsDesktopResize)
        return;
    m_bSupportsDesktopResize = newSupportsDesktopResize;
    SetModified(true);
}

bool CParameterBase::GetLedState() const
{
    return m_bLedState;
}

void CParameterBase::SetLedState(bool state)
{
    if(m_bLedState == state)
        return;
    m_bLedState = state;
    SetModified(true);
}

CFrmViewer::ADAPT_WINDOWS CParameterBase::GetAdaptWindows()
{
    return m_AdaptWindows;
}

void CParameterBase::SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw)
{
    if(m_AdaptWindows == aw)
        return;
    m_AdaptWindows = aw;
    SetModified(true);
    emit sigAdaptWindowsChanged(m_AdaptWindows);
}

double CParameterBase::GetZoomFactor() const
{
    return m_dbZoomFactor;
}

void CParameterBase::SetZoomFactor(double newZoomFactor)
{
    if(m_dbZoomFactor == newZoomFactor)
        return;
    m_dbZoomFactor = newZoomFactor;
    SetModified(true);
    emit sigZoomFactorChanged(m_dbZoomFactor);
    return;
}

void CParameterBase::slotSetGlobalParameters()
{
    CParameterPlugin* pPlugin = GetGlobalParameters();
    if(!pPlugin) {
        QString szErr = "The CParameterClient is null";
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "CParameterBase", szErr.toStdString().c_str());
        return;
    }

    SetAdaptWindows(pPlugin->GetAdaptWindows());

    SetEnableLocalInputMethod(pPlugin->GetEnableLocalInputMethod());

    m_Record = pPlugin->m_Record;

    return;
}

bool CParameterBase::GetEnableLocalInputMethod() const
{
    return m_bEnableLocalInputMethod;
}

void CParameterBase::SetEnableLocalInputMethod(bool enable)
{
    if(m_bEnableLocalInputMethod == enable)
        return;
    m_bEnableLocalInputMethod = enable;
    SetModified(true);
    emit sigEnableInputMethod(m_bEnableLocalInputMethod);
}
