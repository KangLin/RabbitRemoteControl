#include "ConnecterLibVNCServer.h"
#include "DlgSettingsLibVnc.h"
#include <QDebug>

CConnecterLibVNCServer::CConnecterLibVNCServer(CPluginViewer *parent) : CConnecterDesktop(parent)
{
    m_pParameter = &m_Para;
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

int CConnecterLibVNCServer::OnLoad(QSettings &set)
{
    return m_Para.OnLoad(set);
}

int CConnecterLibVNCServer::OnSave(QSettings &set)
{
    return m_Para.OnSave(set);
}

CConnect* CConnecterLibVNCServer::InstanceConnect()
{
    return new CConnectLibVNCServer(this);
}
