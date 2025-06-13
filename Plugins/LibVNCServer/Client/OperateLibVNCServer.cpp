#include <QLoggingCategory>

#include "OperateLibVNCServer.h"
#include "DlgSettingsLibVnc.h"
#include "BackendLibVNCServer.h"

static Q_LOGGING_CATEGORY(log, "LibVNCServer.Operate")

COperateLibVNCServer::COperateLibVNCServer(CPlugin *plugin)
    : COperateDesktop(plugin)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateLibVNCServer::~COperateLibVNCServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const qint16 COperateLibVNCServer::Version() const
{
    return 0;
}

int COperateLibVNCServer::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = COperateDesktop::Initial();
    if(nRet) return nRet;
    nRet = SetParameter(&m_Para);
    return nRet;
}

int COperateLibVNCServer::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = COperateDesktop::Clean();
    return nRet;
}

QDialog *COperateLibVNCServer::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsLibVnc(&m_Para, parent);
}

CBackend *COperateLibVNCServer::InstanceBackend()
{
    CBackendLibVNCServer* p = new CBackendLibVNCServer(this);
    return p;
}
