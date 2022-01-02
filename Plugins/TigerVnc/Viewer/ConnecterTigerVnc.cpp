// Author: Kang Lin <kl222@126.com>

#include "ConnecterTigerVnc.h"
#include <QDebug>
#include "RabbitCommonLog.h"
#include "DlgGetUserPassword.h"
#include <QMessageBox>

CConnecterTigerVnc::CConnecterTigerVnc(CPluginViewer *parent)
    : CConnecterDesktop(parent)
{
    SetParameter(&m_Para);
}

CConnecterTigerVnc::~CConnecterTigerVnc()
{
    qDebug() << "CConnecterTigerVnc::~CConnecterTigerVnc()";
}

qint16 CConnecterTigerVnc::Version()
{
    return 0;
}

QString CConnecterTigerVnc::ServerName()
{
    if(GetParameter())
        if(!GetParameter()->GetShowServerName()
                || CConnecter::ServerName().isEmpty())
    {
        if(m_Para.GetIce())
        {
            if(!m_Para.GetSignalUser().isEmpty())
                return m_Para.GetSignalUser();
        }
        else {
            if(!GetParameter()->GetHost().isEmpty())
                return GetParameter()->GetHost() + ":"
               + QString::number(GetParameter()->GetPort());
        }
    }
    return CConnecter::ServerName();
}

QDialog *CConnecterTigerVnc::GetDialogSettings(QWidget *parent)
{
    Q_UNUSED(parent)
    CDlgSettingsTigerVnc* p = new CDlgSettingsTigerVnc(&m_Para);
    return p;
}

CConnect* CConnecterTigerVnc::InstanceConnect()
{
    CConnectTigerVnc* p = new CConnectTigerVnc(this);
    bool check = false;
    check = connect(p, SIGNAL(sigGetUserPassword(char**, char**)),
                    SLOT(slotGetUserPassword(char**, char**)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    return p;
}

void CConnecterTigerVnc::slotGetUserPassword(char** user, char** password)
{
    CParameterTigerVnc* p = qobject_cast<CParameterTigerVnc*>(GetParameter());
    
    CDlgGetUserPassword dlg(GetViewer());
    dlg.SetUser(p->GetUser());
    dlg.SetPassword(p->GetPassword());
    dlg.SetSavePassword(p->GetSavePassword());
    if(QDialog::Rejected == dlg.exec())
    {
        *password = rfb::strDup("");
        return;
    }
    
    if(user)
        *user = rfb::strDup(dlg.GetUser().toStdString().c_str());
    if(password)
        *password = rfb::strDup(dlg.GetPassword().toStdString().c_str());
    
    p->SetUser(dlg.GetUser());
    p->SetPassword(dlg.GetPassword());
    p->SetSavePassword(dlg.GetSavePassword());
    if(dlg.GetSavePassword())
    {
        emit sigUpdateParamters(this);
    }
}

void CConnecterTigerVnc::slotShowMessage(const QString& title, const QString& text, bool* reture)
{
    QMessageBox::information(GetViewer(), title, text);
}
