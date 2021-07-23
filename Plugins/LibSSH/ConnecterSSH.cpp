#include "ConnecterSSH.h"
#include "DlgSettingsSSH.h"
#include "ConnectSSH.h"

CConnecterSSH::CConnecterSSH(CPluginViewer *parent)
    : CConnecterPluginsTerminal(parent),
      m_pPara(new CParameterSSH())
{
    m_bThread = true;
    if(m_pPara)
    {
        m_pPara->nPort = 22;
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
