// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ParameterTerminalBase.h"

static Q_LOGGING_CATEGORY(log, "Parameter.Terminal.Base")
CParameterTerminalBase::CParameterTerminalBase(CParameterOperate *parent,
                                               const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_Terminal(this)
{
}

int CParameterTerminalBase::OnLoad(QSettings &set)
{
    set.beginGroup("Terminal");
    SetCommands(set.value("Commands", GetCommands()).toStringList());
    set.endGroup();
    return CParameterOperate::OnLoad(set);
}

int CParameterTerminalBase::OnSave(QSettings &set)
{
    set.beginGroup("Terminal");
    set.setValue("Commands", GetCommands());
    set.endGroup();
    return CParameterOperate::OnSave(set);
}

void CParameterTerminalBase::slotSetGlobalParameters()
{
    CParameterPlugin* pPlugin = GetGlobalParameters();
    if(!pPlugin) {
        QString szErr = "The CParameterClient is null";
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "CParameterTerminalBase", szErr.toStdString().c_str());
        return;
    }
    m_Terminal = pPlugin->m_Terminal;
}

QStringList CParameterTerminalBase::GetCommands() const
{
    return m_lstCommands;
}

void CParameterTerminalBase::SetCommands(const QStringList& cmd)
{
    if(m_lstCommands == cmd)
        return;
    m_lstCommands = cmd;
    SetModified(true);
}
