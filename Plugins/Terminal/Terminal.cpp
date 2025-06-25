#include "Terminal.h"
#include "DlgSettingsTerminal.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Terminal.Plugin.Operate")

CTerminal::CTerminal(CPlugin *parent)
    : COperateTerminal(parent)
{
    qDebug(log) << Q_FUNC_INFO;
    SetParameter(&m_Parameters);
}

CTerminal::~CTerminal()
{
    qDebug(log) << Q_FUNC_INFO;
}

QDialog *CTerminal::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTerminal(&m_Parameters, parent);
}

int CTerminal::Start()
{
    slotUpdateParameter(this);
    if(m_pConsole) {
        if(!m_Parameters.GetShell().isEmpty())
            m_pConsole->setShellProgram(m_Parameters.GetShell());
        if(!m_Parameters.GetShellParameters().isEmpty())
            m_pConsole->setArgs(QStringList() << m_Parameters.GetShellParameters());
        m_pConsole->startShellProgram();
    }
    emit sigRunning();
    return 0;
}

int CTerminal::Stop()
{
    if(m_pConsole)
        m_pConsole->close();
    emit sigFinished();
    return 0;
}

CBackend *CTerminal::InstanceBackend()
{
    return nullptr;
}
