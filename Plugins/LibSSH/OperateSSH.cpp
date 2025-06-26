#include "OperateSSH.h"
#include "DlgSettingsSSH.h"
#include "BackendSSH.h"
#undef SetPort

COperateSSH::COperateSSH(CPlugin *parent)
    : COperateTerminal(parent)
{
    m_Parameters.m_Net.SetPort(22);
    QList<CParameterUser::TYPE> lstType;
    lstType << CParameterUser::TYPE::UserPassword
            << CParameterUser::TYPE::PublicKey;
    m_Parameters.m_Net.m_User.SetType(lstType);
    m_Parameters.m_Net.SetPrompt(tr("The host is empty in \"Proxy->SSH tunnel\". please set it"));
}

COperateSSH::~COperateSSH()
{}

int COperateSSH::Initial()
{
    int nRet = 0;
    nRet = COperateTerminal::Initial();
    if(nRet) return nRet;
    return SetParameter(&m_Parameters);
}

int COperateSSH::Clean()
{
    return 0;
}

QDialog *COperateSSH::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsSSH(GetParameter(), parent);
}

CBackend *COperateSSH::InstanceBackend()
{
    return new CBackendSSH(this);
}
