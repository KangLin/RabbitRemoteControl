// Author: Kang Lin <kl222@126.com>

#include "OperateFreeRDP.h"
#include "BackendFreeRDP.h"
#include "DlgSetFreeRDP.h"
#include <QInputDialog>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "FreeRDP.Operate")

COperateFreeRDP::COperateFreeRDP(CPlugin *plugin)
    : COperateDesktop(plugin)
{
    qDebug(log) << Q_FUNC_INFO;
    //WLog_SetLogLevel(WLog_GetRoot(), WLOG_TRACE);

    // 在 freerdp_client_context_free 中释放
    //m_ParameterFreeRdp.m_pSettings = freerdp_settings_new(0);
}


COperateFreeRDP::~COperateFreeRDP()
{
    qDebug(log) << Q_FUNC_INFO;
}

const qint16 COperateFreeRDP::Version() const
{
    return 0;
}

//! [Set the parameter]
int COperateFreeRDP::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = COperateDesktop::Initial();
    if(nRet) return nRet;
    nRet = SetParameter(&m_ParameterFreeRdp);
    return nRet;
}
//! [Set the parameter]

int COperateFreeRDP::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = COperateDesktop::Clean();
    return nRet;
}

QDialog* COperateFreeRDP::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSetFreeRDP(&m_ParameterFreeRdp, parent);
}

CBackend *COperateFreeRDP::InstanceBackend()
{
    try{
        CBackendFreeRDP* p = new CBackendFreeRDP(this);
        return p;
    } catch(...) {
        qDebug(log) << Q_FUNC_INFO << "exception";
        return nullptr;
    }
}
