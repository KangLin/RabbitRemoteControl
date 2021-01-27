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
    if(m_pPara) delete m_pPara;
}

QDialog *CConnecterTerminal::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTerminal(m_pPara, parent);
}

CParameterTerminalAppearance* CConnecterTerminal::GetPara()
{
    return m_pPara;
}
