// Author: Kang Lin <kl222@126.com>

#include "ConnecterTigerVnc.h"
#include <QDebug>
#include "RabbitCommonLog.h"

CConnecterTigerVnc::CConnecterTigerVnc(CPluginViewer *parent)
    : CConnecterDesktop(parent),
      m_pConnect(nullptr)
{
    m_pParameter = &m_Para;
}

CConnecterTigerVnc::~CConnecterTigerVnc()
{
    qDebug() << "CConnecterTigerVnc::~CConnecterTigerVnc()";
    if(m_pConnect)
    {
        m_pConnect->deleteLater();
        m_pConnect = nullptr;
    }
}

qint16 CConnecterTigerVnc::Version()
{
    return 0;
}

QString CConnecterTigerVnc::ServerName()
{
    if(CConnecter::ServerName().isEmpty())
    {
        if(m_Para.GetIce())
        {
            if(!m_Para.GetSignalUser().isEmpty())
                return m_Para.GetSignalUser();
        }
        else {
            if(!m_pParameter->GetHost().isEmpty())
                return m_pParameter->GetHost() + ":"
               + QString::number(m_pParameter->GetPort());
        }
        return CConnecter::Name();
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
    emit sigConnect(this);
    return 0;
}

int CConnecterTigerVnc::DisConnect()
{
    emit sigDisconnect(this);
    return 0;
}
