#include "ConnecterPluginTerminal.h"
#include "DlgSettingsTerminal.h"

CConnecterPluginTerminal::CConnecterPluginTerminal(CPluginClient *parent) 
    : CConnecterTerminal(parent)
{
    m_pPara = new CParameterTerminal();
}

CConnecterPluginTerminal::~CConnecterPluginTerminal()
{
    if(m_pPara)
    {
        delete m_pPara;
        m_pPara = nullptr;
    }
}

QDialog *CConnecterPluginTerminal::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTerminal(m_pPara, parent);
}

int CConnecterPluginTerminal::OnConnect()
{
    if(m_pConsole)
        m_pConsole->startShellProgram();

    return 0;    
}

int CConnecterPluginTerminal::OnDisConnect()
{
    if(m_pConsole) m_pConsole->close();
    return 0;
}
