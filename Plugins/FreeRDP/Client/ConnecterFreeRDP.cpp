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
    qDebug(log) << Q_FUNC_INFO;
    //WLog_SetLogLevel(WLog_GetRoot(), WLOG_TRACE);

    // 在 freerdp_client_context_free 中释放
    //m_ParameterFreeRdp.m_pSettings = freerdp_settings_new(0);
}
//! [Set the parameter]

CConnecterFreeRDP::~CConnecterFreeRDP()
{
    qDebug(log) << Q_FUNC_INFO;
}

qint16 CConnecterFreeRDP::Version()
{
    return 0;
}

int CConnecterFreeRDP::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = CConnecterThread::Initial();
    if(nRet) return nRet;
    nRet = SetParameter(&m_ParameterFreeRdp);
    return nRet;
}

int CConnecterFreeRDP::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = CConnecterThread::Clean();
    return nRet;
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
        qDebug(log) << Q_FUNC_INFO << "exception";
        return nullptr;
    }
}
