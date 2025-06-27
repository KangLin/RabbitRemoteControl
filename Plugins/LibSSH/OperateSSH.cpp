// Author: Kang Lin <kl222@126.com>

#include "OperateSSH.h"
#include "DlgSettingsSSH.h"
#include "BackendSSH.h"

#include <QLoggingCategory>

#undef SetPort
static Q_LOGGING_CATEGORY(log, "Plugin.SSH.Operate")

COperateSSH::COperateSSH(CPlugin *parent)
    : COperateTerminal(parent)
{
    qDebug(log) << Q_FUNC_INFO;
    auto &net = m_Parameters.m_SSH.m_Net;
    net.SetPort(22);
    QList<CParameterUser::TYPE> lstType;
    lstType << CParameterUser::TYPE::UserPassword
            << CParameterUser::TYPE::PublicKey;
    net.m_User.SetType(lstType);
}

COperateSSH::~COperateSSH()
{
    qDebug(log) << Q_FUNC_INFO;
}

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
