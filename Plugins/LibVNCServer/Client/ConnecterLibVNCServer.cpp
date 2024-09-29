#include "ConnecterLibVNCServer.h"
#include "DlgSettingsLibVnc.h"
#include "ConnectLibVNCServer.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "LibVNCServer.Connecter")

CConnecterLibVNCServer::CConnecterLibVNCServer(CPluginClient *plugin)
    : CConnecterThread(plugin)
{
    SetParameter(&m_Para);
}

CConnecterLibVNCServer::~CConnecterLibVNCServer()
{
    qDebug(log) << "CConnecterLibVNCServer::~CConnecterLibVNCServer()";
}

qint16 CConnecterLibVNCServer::Version()
{
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
