#include "ConnecterLibVNCServer.h"
#include "DlgSettingsLibVnc.h"
#include <QDebug>
#include "DlgGetUserPassword.h"

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
    bool check = false;
    check = connect(p, SIGNAL(sigGetUserPassword(char**, char**)),
                    SLOT(slotGetUserPassword(char**, char**)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    return p;
}

void CConnecterLibVNCServer::slotGetUserPassword(char** user, char** password)
{
    CParameterLibVNCServer* p = qobject_cast<CParameterLibVNCServer*>(GetParameter());
    
    CDlgGetUserPassword dlg;
    dlg.SetUser(p->GetUser());
    dlg.SetPassword(p->GetPassword());
    dlg.SetSavePassword(p->GetSavePassword());
    if(QDialog::Rejected == dlg.exec())
    {
        return;
    }
    
    if(user)
        *user = strdup(dlg.GetUser().toStdString().c_str());
    if(password)
        *password = strdup(dlg.GetPassword().toStdString().c_str());
    
    p->SetUser(dlg.GetUser());
    p->SetPassword(dlg.GetPassword());
    p->SetSavePassword(dlg.GetSavePassword());
    if(dlg.GetSavePassword())
    {
        emit sigUpdateParamters(this);
    }
}
