#include <QLoggingCategory>
#include "TerminalParameter.h"

static Q_LOGGING_CATEGORY(log, "Terminal.Parameter")
CTerminalParameter::CTerminalParameter(CParameterOperate *parent, const QString &szPrefix)
    : CParameterTerminalBase{parent, szPrefix}
    , m_bTitleChanged(true)
    , m_bRestoreDirectory(true)
{
#if defined(Q_OS_UNIX)
    m_szShell = qgetenv("SHELL");
    if(m_szShell.isEmpty()) {
        m_szShell = "/bin/sh";
        m_szShellName = "sh";
    }
#elif defined(Q_OS_WIN)
    m_szShell = qgetenv("ComSpec");
    if(m_szShell.isEmpty()) {
        m_szShell = "C:\\Windows\\System32\\cmd.exe";
        m_szShellName = "CMD";
    }
#endif
}

int CTerminalParameter::OnLoad(QSettings &set)
{
    set.beginGroup("Terminal");
    SetShell(set.value("Shell", GetShell()).toString());
    SetShellName(set.value("Shell/Name", GetShellName()).toString());
    SetShellParameters(set.value("Shell/Parameters", GetShellParameters()).toString());
    SetLasterDirectory(set.value("Directory/Laster", GetLasterDirectory()).toString());
    SetRestoreDirectory(set.value("Directory/Restore", GetRestoreDirectory()).toBool());
    SetEnableTitleChanged(set.value("EnableTitleChanged", GetEnableTitleChanged()).toBool());
    set.endGroup();
    return CParameterTerminalBase::OnLoad(set);
}

int CTerminalParameter::OnSave(QSettings &set)
{
    set.beginGroup("Terminal");
    set.setValue("Shell", GetShell());
    set.setValue("Shell/Name", GetShellName());
    set.setValue("Shell/Parameters", GetShellParameters());
    set.setValue("Directory/Laster", GetLasterDirectory());
    set.setValue("Directory/Restore", GetRestoreDirectory());
    set.setValue("EnableTitleChanged", GetEnableTitleChanged());
    
    set.endGroup();
    return CParameterTerminalBase::OnSave(set);
}

int CTerminalParameter::SetShell(const QString& shell)
{
    if(m_szShell == shell)
        return 0;
    m_szShell = shell;
    SetModified(true);
    return 0;
}

const QString CTerminalParameter::GetShell() const
{
    return m_szShell;
}

const QString CTerminalParameter::GetShellName() const
{
    return m_szShellName;
}

int CTerminalParameter::SetShellName(const QString& name)
{
    if(m_szShellName == name)
        return 0;
    m_szShellName = name;
    SetModified(true);
    return 0;
}

int CTerminalParameter::SetShellParameters(const QString &para)
{
    if(m_szShellParameters == para)
        return 0;
    m_szShellParameters = para;
    SetModified(true);
    return 0;
}

const QString CTerminalParameter::GetShellParameters() const
{
    return m_szShellParameters;
}

const QString CTerminalParameter::GetLasterDirectory() const
{
    return m_szLasterDirectory;
}

int CTerminalParameter::SetLasterDirectory(const QString& d)
{
    if(m_szLasterDirectory == d)
        return 0;
    m_szLasterDirectory = d;
    SetModified(true);
    return 0;
}

bool CTerminalParameter::GetRestoreDirectory() const
{
    return m_bRestoreDirectory;
}

int CTerminalParameter::SetRestoreDirectory(bool bEnable)
{
    if(m_bRestoreDirectory == bEnable)
        return 0;
    m_bRestoreDirectory = bEnable;
    SetModified(true);
    return 0;
}

bool CTerminalParameter::GetEnableTitleChanged() const
{
    return m_bTitleChanged;
}

void CTerminalParameter::SetEnableTitleChanged(bool newTitleChanged)
{
    if(m_bTitleChanged == newTitleChanged)
        return;
    m_bTitleChanged = newTitleChanged;
    SetModified(true);
    emit sigEnableTitleChanged(m_bTitleChanged);
    return;
}
