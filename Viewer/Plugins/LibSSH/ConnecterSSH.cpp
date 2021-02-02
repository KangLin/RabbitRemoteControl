#include "ConnecterSSH.h"
#include "DlgSettingsSSH.h"
#include "ConnectSSH.h"

CConnecterSSH::CConnecterSSH(CPluginFactory *parent)
    : CConnecterPluginsTerminal(parent),
      m_pPara(new CParameterSSH())
{
    m_bThread = true;
    if(m_pPara)
    {
        m_pPara->nPort = 22;
        m_pPara->colorScheme = "Linux";
        m_pPara->scrollBarPosition = QTermWidget::NoScrollBar;
    }
}

CConnecterSSH::~CConnecterSSH()
{}

CConnect *CConnecterSSH::InstanceConnect()
{
    return new CConnectSSH(this);
}

QDialog *CConnecterSSH::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsSSH(m_pPara, parent);
}

CParameterTerminal *CConnecterSSH::GetPara()
{
    return m_pPara;
}
