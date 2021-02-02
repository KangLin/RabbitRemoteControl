#include "ConnecterTerminal.h"
#include "DlgSettingsTerminal.h"

CConnecterTerminal::CConnecterTerminal(CPluginFactory *parent) 
    : CConnecterPluginsTerminal(parent)
{
    m_pPara = new CParameterTerminal();
}

CConnecterTerminal::~CConnecterTerminal()
{
    if(m_pPara)
    {
        delete m_pPara;
        m_pPara = nullptr;
    }
}

QDialog *CConnecterTerminal::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTerminal(m_pPara, parent);
}

CParameterTerminal* CConnecterTerminal::GetPara()
{
    return m_pPara;
}

int CConnecterTerminal::OnConnect()
{
    if(m_pConsole)
        m_pConsole->startShellProgram();

    return 0;    
}

int CConnecterTerminal::OnDisConnect()
{
    if(m_pConsole) m_pConsole->close();
    return 0;
}
