// Author: Kang Lin <kl222@126.com>

#include "ConnecterFreeRdp.h"
#include "ConnectFreeRdp.h"
#include <QDebug>
#include <QInputDialog>
#include "DlgSetFreeRdp.h"
#include "DlgGetUserPassword.h"

CConnecterFreeRdp::CConnecterFreeRdp(CPluginViewer *parent)
    : CConnecterDesktopThread(parent)
{
    SetParameter(&m_ParameterFreeRdp);
    // 在 freerdp_client_context_free 中释放
    m_ParameterFreeRdp.m_pSettings = freerdp_settings_new(0);
    GetParameter()->SetPort(3389);
}

CConnecterFreeRdp::~CConnecterFreeRdp()
{
    qDebug() << "CConnecterFreeRdp::~CConnecterFreeRdp()";
}

qint16 CConnecterFreeRdp::Version()
{
    return 0;
}

QDialog *CConnecterFreeRdp::GetDialogSettings(QWidget *parent)
{
    return new CDlgSetFreeRdp(&m_ParameterFreeRdp, parent);
}

CConnect *CConnecterFreeRdp::InstanceConnect()
{
    CConnectFreeRdp* p = new CConnectFreeRdp(this);
    bool check = false;
    check = connect(p, SIGNAL(sigGetUserPassword(char**, char**, char**)),
                    SLOT(slotGetUserPassword(char**, char**, char**)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigBlockShowMessage(QString, QString, QMessageBox::StandardButtons, QMessageBox::StandardButton&)),
                    SLOT(slotBlockShowMessage(QString, QString, QMessageBox::StandardButtons, QMessageBox::StandardButton&)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    return p;
}

void CConnecterFreeRdp::slotGetUserPassword(char** user, char** password, char** domain)
{
    CParameterFreeRdp* p = qobject_cast<CParameterFreeRdp*>(GetParameter());
    if(!p) return;
    
    CDlgGetUserPassword dlg(GetViewer());
    if(user && *user)
        dlg.SetUser(*user);
    if(password && *password)
        dlg.SetPassword(*password);
    if(domain && *domain)
        dlg.SetDomain(*domain);
    dlg.SetSavePassword(p->GetSavePassword());
    if(QDialog::Rejected == dlg.exec())
        return;
    
    if(domain)
        *domain = _strdup(dlg.GetDomain().toStdString().c_str());
    if(user)
        *user = _strdup(dlg.GetUser().toStdString().c_str());
    if(password)
        *password = _strdup(dlg.GetPassword().toStdString().c_str());
    
    p->SetUser(dlg.GetUser());
    p->SetPassword(dlg.GetPassword());
    p->SetSavePassword(dlg.GetSavePassword());
    if(dlg.GetSavePassword())
    {
        emit sigUpdateParamters(this);
    }
}
