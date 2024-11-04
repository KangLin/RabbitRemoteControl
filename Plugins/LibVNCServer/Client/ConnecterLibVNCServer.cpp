#include "ConnecterLibVNCServer.h"
#include "DlgSettingsLibVnc.h"
#include "ConnectLibVNCServer.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "LibVNCServer.Connecter")

CConnecterLibVNCServer::CConnecterLibVNCServer(CPluginClient *plugin)
    : CConnecterThread(plugin)
{
    qDebug(log) << __FUNCTION__;
}

CConnecterLibVNCServer::~CConnecterLibVNCServer()
{
    qDebug(log) << __FUNCTION__;
}

qint16 CConnecterLibVNCServer::Version()
{
    return 0;
}

int CConnecterLibVNCServer::OnInitial()
{
    qDebug(log) << __FUNCTION__;
    SetParameter(&m_Para);
    return 0;
}

int CConnecterLibVNCServer::OnClean()
{
    qDebug(log) << __FUNCTION__;
    return 0;
}

QDialog *CConnecterLibVNCServer::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsLibVnc(&m_Para, parent);
}

CConnect* CConnecterLibVNCServer::InstanceConnect()
{
    CConnectLibVNCServer* p = new CConnectLibVNCServer(this);
    return p;
}
