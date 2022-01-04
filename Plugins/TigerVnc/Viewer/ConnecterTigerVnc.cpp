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
    return p;
}
