// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ParameterTerminalBase.h"

static Q_LOGGING_CATEGORY(log, "Parameter.Terminal.Base")
CParameterTerminalBase::CParameterTerminalBase(CParameterOperate *parent,
                                               const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_Terminal(this)
    , m_bTitleChanged(true)
{
#if defined(Q_OS_UNIX)
    m_szShell = qgetenv("SHELL");
    if(m_szShell.isEmpty())
        m_szShell = "/bin/sh";
#elif defined(Q_OS_WIN)
    m_szShell = qgetenv("ComSpec");
    if(m_szShell.isEmpty())
        m_szShell = "C:\\Windows\\System32\\cmd.exe";
#endif
}

int CParameterTerminalBase::OnLoad(QSettings &set)
{
    set.beginGroup("Terminal");
    SetShell(set.value("Shell", GetShell()).toString());
    SetShellParameters(set.value("Shell/Parameters", GetShellParameters()).toString());
    SetEnableTitleChanged(set.value("EnableTitleChanged", GetEnableTitleChanged()).toBool());
    set.endGroup();
    return 0;
}

int CParameterTerminalBase::OnSave(QSettings &set)
{
    set.beginGroup("Terminal");
    set.setValue("Shell", GetShell());
    set.setValue("Shell/Parameters", GetShellParameters());
    set.setValue("EnableTitleChanged", GetEnableTitleChanged());
    set.endGroup();
    return 0;
}

void CParameterTerminalBase::slotSetGlobalParameters()
{
    CParameterPlugin* pPlugin = GetGlobalParameters();
    if(!pPlugin) {
        QString szErr = "The CParameterClient is null";
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "CParameterBase", szErr.toStdString().c_str());
        return;
    }
    m_Terminal = pPlugin->m_Terminal;
}

int CParameterTerminalBase::SetShell(const QString& shell)
{
    if(m_szShell == shell)
        return 0;
    m_szShell = shell;
    SetModified(true);
    return 0;
}

QString CParameterTerminalBase::GetShell()
{
    return m_szShell;
}

int CParameterTerminalBase::SetShellParameters(const QString &para)
{
    if(m_szShellParameters == para)
        return 0;
    m_szShellParameters = para;
    SetModified(true);
    return 0;
}

QString CParameterTerminalBase::GetShellParameters()
{
    return m_szShellParameters;
}

bool CParameterTerminalBase::GetEnableTitleChanged() const
{
    return m_bTitleChanged;
}

void CParameterTerminalBase::SetEnableTitleChanged(bool newTitleChanged)
{
    if(m_bTitleChanged == newTitleChanged)
        return;
    m_bTitleChanged = newTitleChanged;
    SetModified(true);
    return;
}
