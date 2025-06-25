// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ParameterTerminalBase.h"

static Q_LOGGING_CATEGORY(log, "Paramter.Terminal.Base")
CParameterTerminalBase::CParameterTerminalBase(QObject *parent)
    : CParameterOperate{parent}
    , m_Terminal(this)
{}


int CParameterTerminalBase::OnLoad(QSettings &set)
{
    set.beginGroup("Terminal");
    SetShell(set.value("Shell", GetShell()).toString());
    set.endGroup();
    return 0;
}

int CParameterTerminalBase::OnSave(QSettings &set)
{
    set.beginGroup("Terminal");
    set.setValue("Shell", GetShell());
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
    if(m_Shell == shell)
        return 0;
    m_Shell = shell;
    SetModified(true);
    return 0;
}

QString CParameterTerminalBase::GetShell()
{
    return m_Shell;
}
