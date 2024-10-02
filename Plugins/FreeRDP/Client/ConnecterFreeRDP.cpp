// Author: Kang Lin <kl222@126.com>

#include "ConnecterFreeRDP.h"
#include "ConnectFreeRDP.h"
#include "DlgSetFreeRDP.h"
#include <QInputDialog>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "FreeRDP.Connecter")
//! [Set the parameter]
CConnecterFreeRDP::CConnecterFreeRDP(CPluginClient *plugin)
    : CConnecterThread(plugin)
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
    qDebug(log) << "CConnecterFreeRdp::~CConnecterFreeRdp()";
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
        qDebug(log) << "CConnecterFreeRDP::InstanceConnect() exception";
        return nullptr;
    }
}
