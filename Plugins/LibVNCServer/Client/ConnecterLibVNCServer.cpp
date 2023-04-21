#include "ConnecterLibVNCServer.h"
#include "DlgSettingsLibVnc.h"
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(LibVNCServer)

CConnecterLibVNCServer::CConnecterLibVNCServer(CPluginClient *parent)
    : CConnecterDesktopThread(parent)
{
    SetParameter(&m_Para);
}

CConnecterLibVNCServer::~CConnecterLibVNCServer()
{
    qDebug(LibVNCServer) << "CConnecterLibVNCServer::~CConnecterLibVNCServer()";
}

qint16 CConnecterLibVNCServer::Version()
{
    return 0;
}

QDialog *CConnecterLibVNCServer::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsLibVnc(this, parent);
}

CConnect* CConnecterLibVNCServer::InstanceConnect()
{
    CConnectLibVNCServer* p = new CConnectLibVNCServer(this);
    return p;
}
