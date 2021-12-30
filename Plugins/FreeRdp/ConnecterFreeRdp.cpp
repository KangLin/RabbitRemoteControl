// Author: Kang Lin <kl222@126.com>

#include "ConnecterFreeRdp.h"
#include "ConnectFreeRdp.h"
#include <QDebug>
#include "DlgSetFreeRdp.h"

CConnecterFreeRdp::CConnecterFreeRdp(CPluginViewer *parent)
    : CConnecterDesktop(parent)
{
    SetPara(&m_ParameterFreeRdp);
    // 在 freerdp_client_context_free 中释放
    m_ParameterFreeRdp.m_pSettings = freerdp_settings_new(0);
    GetPara()->SetPort(3389);
}

CConnecterFreeRdp::~CConnecterFreeRdp()
{
    qDebug() << "CConnecterFreeRdp::~CConnecterFreeRdp()";
}

qint16 CConnecterFreeRdp::Version()
{
    return 0;
}

QDialog *CConnecterFreeRdp::GetDialogSettings(QWidget *parent)
{
    return new CDlgSetFreeRdp(&m_ParameterFreeRdp, parent);
}

CConnect *CConnecterFreeRdp::InstanceConnect()
{
    return new CConnectFreeRdp(this);
}
