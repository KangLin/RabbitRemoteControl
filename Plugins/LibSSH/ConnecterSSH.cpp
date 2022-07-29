#include "ConnecterSSH.h"
#include "DlgSettingsSSH.h"
#include "ConnectSSH.h"

CConnecterSSH::CConnecterSSH(CPluginClient *parent)
    : CConnecterTerminal(parent)
{    
    m_pPara = new CParameterSSH();
    m_bThread = true;
    if(m_pPara)
    {
        m_pPara->SetPort(22);
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
    CParameterSSH* pPara = dynamic_cast<CParameterSSH*>(GetParameter());
    return new CDlgSettingsSSH(pPara, parent);
}
