#include "ConnecterLibVNCServer.h"
#include "DlgSettingsLibVnc.h"
#include <QDebug>

CConnecterLibVNCServer::CConnecterLibVNCServer(CPluginViewer *parent)
    : CConnecterDesktopThread(parent)
{
    SetParameter(&m_Para);
}

CConnecterLibVNCServer::~CConnecterLibVNCServer()
{
    qDebug() << "CConnecterLibVNCServer::~CConnecterLibVNCServer()";
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
