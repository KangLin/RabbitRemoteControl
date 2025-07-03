// Author: Kang Lin <kl222@126.com>

#include "Terminal.h"
#include "DlgSettingsTerminal.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Plugin.Terminal.Operate")

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
    if(m_pTerminal) {
        if(!m_Parameters.GetShell().isEmpty())
            m_pTerminal->setShellProgram(m_Parameters.GetShell());
        if(!m_Parameters.GetShellParameters().isEmpty())
            m_pTerminal->setArgs(QStringList() << m_Parameters.GetShellParameters());
        m_pTerminal->startShellProgram();
    }
    emit sigRunning();
    return 0;
}

int CTerminal::Stop()
{
    if(m_pTerminal)
        m_pTerminal->close();
    emit sigFinished();
    return 0;
}

CBackend *CTerminal::InstanceBackend()
{
    return nullptr;
}
