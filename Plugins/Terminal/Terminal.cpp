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
    if(m_pConsole)
        m_pConsole->startShellProgram();
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
