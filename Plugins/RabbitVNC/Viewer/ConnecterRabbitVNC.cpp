// Author: Kang Lin <kl222@126.com>

#include "ConnecterRabbitVNC.h"
#include <QDebug>
#include "RabbitCommonLog.h"
#include "DlgGetPasswordRabbitVNC.h"
#include <QMessageBox>

CConnecterRabbitVNC::CConnecterRabbitVNC(CPluginViewer *parent)
    : CConnecterDesktop(parent)
{
    SetParameter(&m_Para);
}

CConnecterRabbitVNC::~CConnecterRabbitVNC()
{
    qDebug() << "CConnecterRabbitVnc::~CConnecterRabbitVnc()";
}

qint16 CConnecterRabbitVNC::Version()
{
    return 0;
}

QString CConnecterRabbitVNC::ServerName()
{
    if(GetParameter())
        if(!GetParameter()->GetShowServerName()
                || CConnecter::ServerName().isEmpty())
    {
        if(m_Para.GetIce())
        {
            if(!m_Para.GetPeerUser().isEmpty())
                return m_Para.GetPeerUser();
        }
        else {
            if(!GetParameter()->GetHost().isEmpty())
                return GetParameter()->GetHost() + ":"
               + QString::number(GetParameter()->GetPort());
        }
    }
    return CConnecter::ServerName();
}

QDialog *CConnecterRabbitVNC::GetDialogSettings(QWidget *parent)
{
    Q_UNUSED(parent)
    CDlgSettingsRabbitVNC* p = new CDlgSettingsRabbitVNC(&m_Para);
    return p;
}

CConnect* CConnecterRabbitVNC::InstanceConnect()
{
    return new CConnectRabbitVNC(this);
}
