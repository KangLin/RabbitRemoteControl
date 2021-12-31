// Author: Kang Lin <kl222@126.com>

#include "ConnecterTigerVnc.h"
#include <QDebug>
#include "RabbitCommonLog.h"

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
    return new CConnectTigerVnc(this);
}

int CConnecterTigerVnc::Connect()
{
    // Check whether the parameters are complete
    if(GetParameter() && !GetParameter()->GetCheckCompleted())
    {
        if(QDialog::Rejected == OpenDialogSettings())
           return -1;
        emit sigUpdateParamters(this);
    }
    emit sigConnect(this);
    return 0;
}

int CConnecterTigerVnc::DisConnect()
{
    emit sigDisconnect(this);
    return 0;
}
