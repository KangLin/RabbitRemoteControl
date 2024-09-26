// Author: Kang Lin <kl222@126.com>

#include "ConnecterFreeRDP.h"
#include "ConnectFreeRDP.h"
#include <QInputDialog>
#include "DlgSetFreeRDP.h"

//! [Set the parameter]
CConnecterFreeRDP::CConnecterFreeRDP(CPluginClient *parent)
    : CConnecterDesktopThread(parent),
      m_Logger("FreeRDP.Connecter")
{
    //WLog_SetLogLevel(WLog_GetRoot(), WLOG_TRACE);

    // Set the parameter pointer
    SetParameter(&m_ParameterFreeRdp);

    // 在 freerdp_client_context_free 中释放
    //m_ParameterFreeRdp.m_pSettings = freerdp_settings_new(0);
}
//! [Set the parameter]

CConnecterFreeRDP::~CConnecterFreeRDP()
{
    qDebug(m_Logger) << "CConnecterFreeRdp::~CConnecterFreeRdp()";
}

qint16 CConnecterFreeRDP::Version()
{
    return 0;
}

QDialog* CConnecterFreeRDP::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSetFreeRDP(&m_ParameterFreeRdp, parent);
}

CConnect* CConnecterFreeRDP::InstanceConnect()
{
    try{
        CConnectFreeRDP* p = new CConnectFreeRDP(this);
        return p;
    } catch(...) {
        qDebug(m_Logger) << "CConnecterFreeRDP::InstanceConnect() exception";
        return nullptr;
    }
}
