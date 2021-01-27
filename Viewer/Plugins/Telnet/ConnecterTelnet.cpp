#include "ConnecterTelnet.h"
#include "DlgSettingsTelnet.h"

CConnecterTelnet::CConnecterTelnet(CPluginFactory *parent)
    : CConnecterPluginsTerminal(parent),
      m_pPara(nullptr)
{
    m_pPara = new CParameterTelnet();
    if(m_pPara)
    {
        m_pPara->font = m_pConsole->getTerminalFont();
        m_pPara->colorScheme = "Linux";
        m_pPara->scrollBarPosition = QTermWidget::NoScrollBar;
    }
}

QDialog *CConnecterTelnet::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTelnet(m_pPara, parent);
}

CParameterTerminalAppearance *CConnecterTelnet::GetPara()
{
    return m_pPara;
}
