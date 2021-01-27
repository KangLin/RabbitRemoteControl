#include "ConnecterTerminal.h"
#include "DlgSettingsTerminal.h"

CConnecterTerminal::CConnecterTerminal(CPluginFactory *parent) 
    : CConnecterPluginsTerminal(parent)
{
    m_pPara = new CParameterTerminalAppearance();
    if(m_pPara)
    {
        m_pPara->font = m_pConsole->getTerminalFont();
        m_pPara->colorScheme = "Linux"; // "GreenOnBlack";
        m_pPara->scrollBarPosition = QTermWidget::NoScrollBar;
    }
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

CParameterTerminalAppearance* CConnecterTerminal::GetPara()
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
